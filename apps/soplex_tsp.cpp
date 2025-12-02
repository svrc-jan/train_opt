#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
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


inline double get_dist(vector<pair<double, double>> coord, int i, int j)
{
	return sqrt(pow(coord[i].first - coord[j].first, 2) + pow(coord[i].second - coord[j].second, 2));
}


bool make_subtour_cut(LPRow& cut, const DVector& prim, const int N)
{
	vector<bool> seen(N, false);
	vector<int> tour(N);
	
	for (int i = 0; i < N; i++) {
		int found = 0;
		for (int j = 0; j < N; j++) {
			if (prim[i*N + j] > 0.5) {
				tour[i] = j;
				found += 1;
			}
		}

		assert(found == 1);
	}

	int best_len = N+1;
	int best_start = -1;


	for (int start = 0; start < N; start++) {
		if (seen[start]) {
			continue;
		}

		int len = 1;
		int curr = start;
		int next = tour[curr];
		while (next != start) {
			assert(!seen[next]);
			seen[next] = true;
			curr = next;
			next = tour[curr];
			len += 1;
		}

		if (best_len > len) {
			best_len = len;
			best_start = start;
		}
	}

	if (best_len == N) {
		return false;
	}
	
	vector<int> sub_tour;
	sub_tour.reserve(best_len);
	sub_tour.push_back(best_start);

	while (tour[sub_tour.back()] != best_start) {
		sub_tour.push_back(tour[sub_tour.back()]);
	}

	assert((int)sub_tour.size() == best_len);

	// cout << "adding subtour cons " << sub_tour << endl;

	DSVector row(best_len*(best_len - 1));
	for (int i = 0; i < best_len; i++) {
		for (int j = 0; j < best_len; j++) {
			if (i == j) {
				continue;
			}

			row.add(sub_tour[i]*N + sub_tour[j], 1.0);
		}
	}

	cut.setRowVector(row);
	cut.setLhs(-infinity);
	cut.setRhs(best_len - 1);

	return true;
}


int select_branch_var(const DVector& prim, const int N, double eps)
{
	int best_var = -1;
	double best_diff = 1.0;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			int var = i*N + j;
			if (abs(prim[var]) > eps && abs(prim[var] - 1) > eps) {
				double diff = abs(prim[var] - 0.5);
				
				if (diff < best_diff) {
					best_diff = diff;
					best_var = var;
				}
			}
		}
	}

	return best_var;
}


void make_branch_cut(LPRow& cut, const int var, bool is_up_dir)
{
	DSVector row(1);
	row.add(var, 1.0);

	cut.setRowVector(row);

	if (is_up_dir) {
		cut.setLhs(1.0);
		cut.setRhs(infinity);
	}
	else {
		cut.setLhs(-infinity);
		cut.setRhs(0.0);
	}

}


double tsp_heuristic(vector<pair<double, double>>& coord)
{
	int N = coord.size();

	vector<bool> seen(N, false);

	vector<int> tour;
	double tour_len = 0;

	tour.reserve(N);
	tour.push_back(0);

	seen[0] = true;

	while ((int)tour.size() < N){
		int best_idx = -1;
		double best_dist = infinity;

		int i = tour.back();
		for (int j = 0; j < N; j++) {
			if (seen[j]) {
				continue;
			}

			double c = get_dist(coord, i, j);
			if (c < best_dist) {
				best_dist = c;
				best_idx = j;
			}
		}

		assert(best_idx >= 0);
		seen[best_idx] = true;

		tour.push_back(best_idx);
		tour_len += best_dist;
	}

	tour_len += get_dist(coord, 0, tour.back());
	
	return tour_len;
}



int main(int argc, char const *argv[])
{
	SoPlex mysoplex;
 
	/* set the objective sense */
	mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);
	mysoplex.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_WARNING);
	mysoplex.setIntParam(SoPlex::SYNCMODE, SoPlex::SYNCMODE_ONLYREAL);
	mysoplex.setIntParam(SoPlex::REPRESENTATION, SoPlex::REPRESENTATION_COLUMN);
	mysoplex.setIntParam(SoPlex::ALGORITHM, SoPlex::ALGORITHM_DUAL);

	int N = 100;

	if (argc >= 2) {
		N = atoi(argv[1]);
	}

	Rand_int_gen rng(43);

	DSVector dummycol(0);

	vector<pair<double, double>> coord(N);

	for (int i = 0; i < N; i++) {
		coord[i] = {rng(N), rng(N)};
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			double c = get_dist(coord, i, j);
			mysoplex.addColReal(LPCol(c, dummycol, (i != j) ? 1.0 : 0.0, 0.0));
		}
	}

	DSVector row1(N-1);
	DSVector row2(N-1);

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

