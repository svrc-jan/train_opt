#pragma once

#include "coin/ClpSimplex.hpp"
#include "instance.hpp"
#include "preprocess.hpp"


struct Res_use
{
	int train;
	int level_lock;
	int level_unlock;
	double res_time;
};


class Lin_prog
{
public:
	Lin_prog(const Instance& inst, const Preprocess& prepr);
	~Lin_prog();

	void init_model();
	void add_path(const int train_idx, const vector<int> path);


	bool dfs_resolve(int depth=0);


private:
	const Instance& inst;
	const Preprocess& prepr;

	ClpSimplex* lp = nullptr;

	vector<vector<int>> paths = {};
	vector<vector<Res_use>> res_uses = {};

	bool find_res_col(pair<Res_use, Res_use>& res_col);

	void get_basis_status(ClpSimplex::Status* col_status, 
		ClpSimplex::Status* row_status, const int n_cols, const int n_rows);

	void set_basis_status(const ClpSimplex::Status* col_status, 
		const ClpSimplex::Status* row_status, const int n_cols, const int n_rows);
};


