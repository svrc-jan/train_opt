#pragma once

#include "coin/ClpSimplex.hpp"
#include "preprocess.hpp"



class Lin_prog
{
public:
	struct Res_use;
	struct Res_cut;

	Lin_prog(const Instance& inst, const Preprocess& prepr);
	~Lin_prog();

	void init_model();
	void add_path(const int train_idx, const std::vector<int> path);


	bool dfs_resolve();

private:
	const Instance& inst;
	const Preprocess& prepr;

	ClpSimplex* lp = nullptr;
	int n_rows = 0;

	std::vector<std::vector<int>> paths = {};
	std::vector<std::vector<Res_use>> res_uses = {};

	bool generate_res_cuts(std::pair<Res_cut, Res_cut>& cuts);

	void get_basis_status(ClpSimplex::Status* col_status, 
		ClpSimplex::Status* row_status, const int n_cols, const int n_rows);

	void set_basis_status(const ClpSimplex::Status* col_status, 
		const ClpSimplex::Status* row_status, const int n_cols, const int n_rows);
};


struct Lin_prog::Res_use
{
	int train = -1;
	int level_lock = -1;
	int level_unlock = -1;
	double res_time = -1;
};


struct Lin_prog::Res_cut
{
	int level_from = -1;
	int level_to = -1;
	double time = 0.0;
};
