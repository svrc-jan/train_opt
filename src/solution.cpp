#include "solution.hpp"


Solution::Solution(const Instance *inst) 
	: inst(inst)
{

}

void Solution::build_path(const uint train_id)
{
	std::vector<uint>& path = this->paths[train_id];		
	path.push_back(this->inst->trains[train_id].begin_idx);

	while (true) {
		const Operation& op = this->inst->ops[path.back()];
		if (op.n_succ == 0) {
			break;
		}
		else if (op.n_succ == 1) {
			path.push_back(op.succ[0]);
		}
		else {
			path.push_back(op.succ[this->branching_choice[op.branching_idx]]);
		}
	}
}

void Solution::build_all_paths()
{
	this->paths.clear();
	for (uint train_id = 0; train_id < this->inst->n_train; train_id++) {
		this->build_path(train_id);
	}
}

std::vector<uint> Solution::get_random_branching_choice()
{

}