#include "lin_prog.hpp"

#include <print>
#include "coin/CbcOrClpParam.hpp"
#include "coin/CoinBuild.hpp"

using namespace std;

#define MIN_RES_TIME 0.1

Lin_prog::Lin_prog(const Instance& inst, const Preprocess& prepr) 
	: inst(inst), prepr(prepr)
{
	this->paths.resize(this->inst.n_trains(), vector<int>());
	this->res_uses.resize(this->inst.n_res(), vector<Res_use>());
}

Lin_prog::~Lin_prog()
{
	if (this->lp != nullptr) {
		delete this->lp;
	}
}


void Lin_prog::init_model()
{
	this->lp = new ClpSimplex;
	this->lp->setLogLevel(0);
	
	this->lp->resize(0, this->prepr.n_levels());

	for (int l = 0; l < this->prepr.n_levels(); l++) {
		auto& level = this->prepr.levels[l];
		this->lp->setColBounds(l, (double)level.time_lb,
			(level.time_ub < INT_MAX) ? (double)level.time_ub : DBL_MAX);

		if (level.ops_out.size == 0) {
			this->lp->setObjCoeff(l, 1.0);
		}
	}
}


void Lin_prog::add_path(const int train_idx, const vector<int> path)
{
	for (int o : path) {
		auto& op = this->inst.ops[o];

		for (auto& res : op.res) {
			auto& ru = this->res_uses[res.idx];
			if (ru.size() > 0 && ru.back().train == train_idx) {
				ru.back().level_unlock = this->prepr.op_level[o].second;
				ru.back().res_time = max((double)res.time, MIN_RES_TIME);
			}
			else {
				ru.push_back({
					.train = train_idx,
					.level_lock = this->prepr.op_level[o].first,
					.level_unlock = this->prepr.op_level[o].second,
					.res_time = max((double)res.time, MIN_RES_TIME)
				});
			}
		}
	}

	CoinBuild row_build;

	int cols[2];
	double elems[2] = {1.0, -1.0};
	for (int o : path) {
		cols[0] = this->prepr.op_level[o].first;
		cols[1] = this->prepr.op_level[o].second;
		row_build.addRow(2, cols, elems, -COIN_DBL_MAX, -this->inst.ops[o].dur);
	}

	this->lp->addRows(row_build);

	this->n_rows = this->lp->numberRows();
}


struct Interval
{
	int idx;
	double start;
	double end;
};

inline bool operator<(const Interval& a, const Interval& b)
{
	return (a.start < b.start) ? true : ((a.start > b.start ) ? false : (a.end < b.end));
}

bool Lin_prog::generate_res_cuts(std::pair<Res_cut, Res_cut>& cuts)
{
	const double* col_solution = this->lp->getColSolution();
	double res_col_time = DBL_MAX;

	for (int r = 0; r < this->inst.n_res(); r++) {
		int n_ints = this->res_uses[r].size();
#ifndef NO_VLA
		Interval ints[n_ints];
#else
		vector<Interval> ints(n_ints);
#endif

		auto& ru = this->res_uses[r];
		for (int i = 0; i < n_ints; i++) {
			ints[i] = {
				.idx = i,
				.start = col_solution[ru[i].level_lock],
				.end = col_solution[ru[i].level_unlock] + ru[i].res_time
			};
		}

#ifndef NO_VLA
		sort(ints, ints + n_ints);
#else
		sort(ints.begin(), ints.end());
#endif
		bool search_done = false;
		for (int i = 0; i < n_ints && !search_done; i++) {
			auto& int_a = ints[i];
			for (int j = i + 1; j < n_ints && !search_done; j++) {
				auto int_b = ints[j];
				if (int_a.end > res_col_time) {
					break;
				}

				if (int_a.end > int_b.start) {
					if (int_b.start < res_col_time) {
						cuts.first = {
							.level_from = ru[int_a.idx].level_unlock,
							.level_to = ru[int_b.idx].level_lock,
							.time = ru[int_a.idx].res_time
						};
						cuts.second = {
							.level_from = ru[int_b.idx].level_unlock,
							.level_to = ru[int_a.idx].level_lock,
							.time = ru[int_b.idx].res_time
						};

						res_col_time = int_b.start;
					}
					
					search_done = true;
				}
				else {
					break;
				}
			}	
		}
	}

	if (res_col_time == DBL_MAX) {
		return false; // no collisions
	}

	bool diff1 = col_solution[cuts.first.level_from] - col_solution[cuts.first.level_to];
	bool diff2 = col_solution[cuts.second.level_from] - col_solution[cuts.second.level_to];
	

	if (diff2 < diff1) {
		swap(cuts.first, cuts.second);
	}

	return true;
}


bool Lin_prog::dfs_resolve()
{
	vector<pair<Res_cut, int>> cut_stack = {};
	cut_stack.push_back({Res_cut(), 0});

	while (!cut_stack.empty()) {
		auto curr = cut_stack.back();
		cut_stack.pop_back();

		Res_cut curr_cut = curr.first;
		int curr_depth = curr.second;

		int cut_idx = -1;
		if (curr_depth == 0) {
			ClpSolve solve_opts;
			solve_opts.setSolveType(ClpSolve::useDual);
			this->lp->initialSolve(solve_opts);
		}
		else {
			int n_curr_rows = this->lp->numberRows();
			int n_extra_rows = n_curr_rows - this->n_rows - curr_depth + 1;

			if (n_extra_rows > 0) {
#if defined STATIC_LOG_LEVEL && STATIC_LOG_LEVEL >= 100
				println("removing {} cuts", n_extra_rows);
#endif
				int extra_rows[n_extra_rows];

				for (int i = 0; i < n_extra_rows; i++) {
					extra_rows[i] = n_curr_rows - n_extra_rows + i;
				}

				this->lp->deleteRows(n_extra_rows, extra_rows);
			}

		
			int cut_cols[2];
			double cut_elems[2] = { 1.0, -1.0 };
			double cut_bound;

			cut_cols[0] = curr_cut.level_from;
			cut_cols[1] = curr_cut.level_to;
			cut_bound = -curr_cut.time;

			this->lp->addRow(2, cut_cols, cut_elems, -DBL_MAX, cut_bound);

#if defined STATIC_LOG_LEVEL && STATIC_LOG_LEVEL >= 100
		println("cut {} -({:.1f})> {}",
			curr_cut.level_from,
			curr_cut.time,
			curr_cut.level_to
		);
#endif
			this->lp->dual();
		}
		
		if (this->lp->status() == 0) {
			std::pair<Res_cut, Res_cut> new_cuts;
			if (!this->generate_res_cuts(new_cuts)) {
				return true; // no cuts necessary solution found
			}
			
			int train1 = this->prepr.levels[new_cuts.first.level_from].train;
			int train2 = this->prepr.levels[new_cuts.second.level_from].train;

			if (train1 < train2) {
				cut_stack.push_back({new_cuts.second, curr_depth + 1});
				cut_stack.push_back({new_cuts.first, curr_depth + 1});
			}
			else {
				cut_stack.push_back({new_cuts.first, curr_depth + 1});
				cut_stack.push_back({new_cuts.second, curr_depth + 1});
			}
		}
	}

	// infeasible
	return false;
}


void Lin_prog::get_basis_status(ClpSimplex::Status* col_status, 
	ClpSimplex::Status* row_status, const int n_cols, const int n_rows)
{
	for (int c = 0; c < n_cols; c++) {
		col_status[c] = this->lp->getColumnStatus(c);
	}

	for (int r = 0; r < n_rows; r++) {
		row_status[r] = this->lp->getRowStatus(r);
	}
}

void Lin_prog::set_basis_status(const ClpSimplex::Status* col_status, 
	const ClpSimplex::Status* row_status, const int n_cols, const int n_rows)
{
	for (int c = 0; c < n_cols; c++) {
		this->lp->setColumnStatus(c, col_status[c]);
	}

	for (int r = 0; r < n_rows; r++) {
		this->lp->setRowStatus(r, row_status[r]);
	}
}
