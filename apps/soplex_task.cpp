#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include "soplex.h"

#include "utils/rand_int.hpp"
#include "utils/stl_print.hpp"

using namespace soplex;
using namespace std;


struct Candidate
{
	double lb;
	LPRowSet cuts;

	bool operator<(const Candidate& other) const { return this->lb > other.lb; }
};

struct Task_problem
{
	int n_trains = 0;
	int n_ops = 0;
	int n_res = 0;

	vector<vector<int>> op_dur = {};
	vector<vector<int>> op_res = {};

	vector<vector<pair<int, int>>> res_op = {};

	int get_var_idx(int t, int o) const { return t*(this->n_ops + 1) + o; }
};


Task_problem create_task_problem(int n_trains, int n_ops, int n_res)
{
	Task_problem prob = {
		.n_trains = n_trains,
		.n_ops = n_ops,
		.n_res = n_res
	};

	assert(n_ops <= n_res);

	vector<int> res_aux(n_res);
	for (int r = 0; r < n_res; r++) {
		res_aux[r] = r;

		prob.res_op.push_back({});
	}

	Rand_int_gen rand_int;
	default_random_engine rand_eng(43);

	for (int t = 0; t < n_trains; t++) {
		shuffle(res_aux.begin(), res_aux.end(), rand_eng);

		prob.op_dur.push_back({});
		prob.op_res.push_back({});

		for (int o = 0; o < n_ops; o++) {
			int dur = rand_int(25, 76);
			prob.op_dur[t].push_back(dur);

			int r = res_aux[o];

			prob.op_res[t].push_back(r);
			prob.res_op[r].push_back({t, o});
		}
	}
}

void create_model(SoPlex& splx, const Task_problem& prob)
{
	DSVector dummycol(0);
	for (int t = 0; t < prob.n_res; t++) {
		for (int o = 0; o < prob.n_ops; o++) {
			splx.addColReal(LPCol(0, dummycol, infinity, 0));
		}

		splx.addColReal(LPCol(1, dummycol, infinity, 0));
	}

	DSVector row(2);
	for (int t = 0; t < prob.n_res; t++) {
		for (int o = 0; o < prob.n_ops; o++) {
			row.clear();
			row.add(prob.get_var_idx(t, o), 1);
			row.add(prob.get_var_idx(t, o+1), -1);

			splx.addRowReal(LPRow(-infinity, row, prob.op_dur[t][o]));
		}
	}
}


int main(int argc, char const *argv[])
{
	int n_trains = 5;
	int n_ops = 5;
	int n_res = 5;

	if (argc >= 4) {
		n_trains = atoi(argv[1]);
		n_ops = atoi(argv[1]);
		n_res = atoi(argv[1]);
	}

	Task_problem prob = create_task_problem(n_trains, n_ops, n_res);

	SoPlex splx;
 
	/* set the objective sense */
	splx.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);
	splx.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_WARNING);
	splx.setIntParam(SoPlex::SYNCMODE, SoPlex::SYNCMODE_ONLYREAL);
	splx.setIntParam(SoPlex::REPRESENTATION, SoPlex::REPRESENTATION_COLUMN);
	splx.setIntParam(SoPlex::ALGORITHM, SoPlex::ALGORITHM_DUAL);


	create_model()

	int n_model_rows = 
	vector<int> row_perm;

	for (int i = 0; i < N; i++) {
		row1.clear();
		row2.clear();
		for (int j = 0; j < N; j++) {

			if (i == j) {
				continue;
			}

			row1.add(i*N + j, 1.0);
			row2.add(i + j*N, 1.0);
		}

		mysoplex.addRowReal(LPRow(1.0, row1, 1.0));
		row_perm.push_back(row_perm.size());

		mysoplex.addRowReal(LPRow(1.0, row2, 1.0));
		row_perm.push_back(row_perm.size());
	}

	assert(row_perm.size() == N*2);
 
	/* write LP in .lp format */
	// mysoplex.writeFileReal("dump_real.lp", NULL, NULL, NULL);

	double eps = 1e-6;


	priority_queue<Candidate> pq;
	
	pq.push({
		.lb = -infinity,
		.cuts = LPRowSet()
	});


	double ub = tsp_heuristic(coord);

	cout << "heur obj: " << ub << endl;

	DVector prim(N*N);
	LPRow cut;
	SPxSolver::Status stat;

	while (!pq.empty()) {
		auto curr = pq.top();
		pq.pop();

		if (curr.lb >= ub) {
			continue;
		}
		
		mysoplex.removeRowsReal(row_perm.data());
		assert(mysoplex.numRows() == N*2);
		
		mysoplex.addRowsReal(curr.cuts);
		while (row_perm.size() < mysoplex.numRows()) {
			row_perm.push_back(-1);
		}

		stat = mysoplex.optimize();
		if (stat == SPxSolver::INFEASIBLE) {

			continue;
		}

		if (stat != SPxSolver::OPTIMAL) {
			mysoplex.writeFileReal("dump_real.lp", NULL, NULL, NULL);
			cout << "failed to solve" << endl;
			exit(1);
		}

		double obj = mysoplex.objValueReal();
		if (obj >= ub) {
			continue;
		}

		if (!mysoplex.getPrimal(prim)) {
			cout << "failed to get primal sol" << endl;
			exit(2);
		}

		int branch_var = select_branch_var(prim, N, eps);
		if (branch_var < 0) {
			if (!make_subtour_cut(cut, prim, N)) {
				if (obj < ub) {
					ub = obj;
					cout << "new best obj: " << obj << endl;
				}
			}
			else {
				Candidate cand_subtour = {
					.lb = obj,
					.cuts = curr.cuts
				};

				cand_subtour.cuts.add(cut);
				pq.push(cand_subtour);
			}
		}

		else {
			Candidate cand_down = {
				.lb = obj,
				.cuts = curr.cuts
			};

			make_branch_cut(cut, branch_var, false);
			cand_down.cuts.add(cut);

			pq.push(cand_down);

			Candidate cand_up = {
				.lb = obj,
				.cuts = curr.cuts
			};

			make_branch_cut(cut, branch_var, true);
			cand_up.cuts.add(cut);

			pq.push(cand_up);
		}
	}
	
	std::cout << "Objective value is " << ub << ".\n";
}

