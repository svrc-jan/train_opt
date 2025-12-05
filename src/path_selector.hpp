#pragma once

#include <vector>
#include <queue>

#include "instance.hpp"

using namespace std;

#define PATH_SELECT_MAX 1e100


class Path_selector
{
public:
	Path_selector(const Instance& inst);
	~Path_selector();

	inline bool select_path(vector<int>& path, const int train_idx,
		const vector<double>& op_costs)
	{ return this->select_path(path, train_idx, op_costs.data(), op_costs.size()); }

	bool select_path(vector<int>& path, const int train_idx,
		const double op_costs[], int n_train_ops);

	bool select_path_by_res_imp(vector<int>& path, const int train_idx,
		vector<double>& res_imp);

	void get_op_importance(vector<double>& op_imp);
	void get_res_importance(vector<double>& res_imp, const vector<double>& op_imp);

private:
	const Instance& inst;
};



