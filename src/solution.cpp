#include "solution.hpp"

Solution::Solution(const Instance& inst, Rand_int_gen& rng)
	: inst(inst), rng(rng)
{
	this->paths = vector<vector<int>>(inst.n_train);
}


vector<int>& Solution::make_random_path(int train)
{
	vector<int>& path = this->paths[train];
	path.clear();

	path.push_back(inst.trains[train].begin_idx);
	const Operation* op = &inst.ops[path.back()];

	while (op->n_succ > 0) {
		if (op->n_succ == 1) {
			path.push_back(op->succ[0]);
		}
		else {
			path.push_back(op->succ[rng(op->n_succ)]);
		}

		op = &inst.ops[path.back()];
	}

	return path;
}


vector<vector<int>>& Solution::make_all_random_paths()
{
	for (int train = 0; train < inst.n_train; train++) {
		this->make_random_path(train);
	}

	return this->paths;
}

std::map<int, int> Solution::get_earliest_starts()
{
	std::map<int, int> earliest_start;

	
}
