#include "lin_prog.hpp"

#include "coin/CbcOrClpParam.hpp"
#include "coin/CoinBuild.hpp"

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

bool Lin_prog::find_res_col(pair<Res_use, Res_use>& res_col)
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
		for (int i = 0; i < n_ints; i++) {
			auto& ru = this->res_uses[r][i];
			ints[i] = {
				.idx = i,
				.start = col_solution[ru.level_lock],
				.end = col_solution[ru.level_unlock] + ru.res_time
			};
		}

#ifndef NO_VLA
		sort(ints, ints + n_ints);
#else
		sort(ints.begin(), ints.end());
#endif
		bool search_done = false;
		for (int i = 0; i < n_ints && !search_done; i++) {
			for (int j = i + 1; j < n_ints && !search_done; j++) {
				if (ints[i].end > ints[j].start) {
					if (ints[j].start < res_col_time) {
						res_col.first = this->res_uses[r][ints[i].idx];
						res_col.second = this->res_uses[r][ints[j].idx];

						res_col_time = ints[j].start;

					}
					

					search_done = true;
				}
				else {
					break;
				}
			}	
		}
	}

	return res_col_time == DBL_MAX;
}


bool Lin_prog::dfs_resolve(int depth)
{
	if (depth == 0) {
		ClpSolve solve_opts;
		solve_opts.setSolveType(ClpSolve::useDual);
		this->lp->initialSolve(solve_opts);
	}
	else {
		this->lp->dual();
	}

	// infeasible
	if (this->lp->status() != 0) {
		return false;
	}

	pair<Res_use, Res_use> res_col;
	if (this->find_res_col(res_col)) {
		return true;
	}

	int n_cols = this->lp->numberColumns();
	int n_rows = this->lp->numberRows();

	ClpSimplex::Status col_status[n_cols];
	ClpSimplex::Status row_status[n_rows];
	
	this->get_basis_status(col_status, row_status, n_cols, n_rows);

	int cut_cols[2];
	double cut_elems[2] = { 1.0, -1.0 };
	double cut_bound;

	int cut_idx = n_rows;
	
	cut_cols[0] = res_col.first.level_unlock;
	cut_cols[1] = res_col.second.level_lock;
	cut_bound = -res_col.first.res_time;
	
	this->lp->addRow(2, cut_cols, cut_elems, -DBL_MAX, cut_bound);
	this->lp->setRowStatus(cut_idx, ClpSimplex::basic);
	if (this->dfs_resolve(depth + 1)) {
		return true;
	};
	this->lp->deleteRows(1, &cut_idx);
	this->set_basis_status(col_status, row_status, n_cols, n_rows);

	cut_cols[0] = res_col.second.level_unlock;
	cut_cols[1] = res_col.first.level_lock;
	cut_bound = -res_col.second.res_time;
	
	this->lp->addRow(2, cut_cols, cut_elems, -DBL_MAX, cut_bound);
	this->lp->setRowStatus(cut_idx, ClpSimplex::basic);
	if (this->dfs_resolve(depth + 1)) {
		return true;
	};
	this->lp->deleteRows(1, &cut_idx);

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
