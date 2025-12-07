#pragma once

#include <unordered_map>
#include "utils/hasher.hpp"
#include "gurobi_c++.h"
#include "preprocess.hpp"


class Gurobi_model
{
public:
	const Instance& inst;
	const Preprocess& prepr;

	Gurobi_model(const Preprocess& prepr, const GRBEnv& grb_env);
	~Gurobi_model();

	void init_model();
	void add_path(int train_idx, const std::vector<int>& path);
	bool solve();

private:
	struct Res_use;
	using Res_col = std::pair<Res_use, Res_use>;
	
	const GRBEnv& grb_env;
	GRBModel* model = nullptr;

	std::vector<GRBVar> var_time = {};
	std::unordered_map<std::pair<int, int>, GRBVar,
		Pair_hasher> var_order;
		
	std::vector<std::vector<Res_use>> res_uses = {};

	bool find_res_col(Res_col& res_col);
	bool add_res_constr(const Res_col& res_col);
};


struct Gurobi_model::Res_use
{
	int train;
	int level_lock;
	int level_unlock;
	double res_time;
};

