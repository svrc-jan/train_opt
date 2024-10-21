#include "solution.hpp"


Solution::Solution(const Instance *inst) 
	: inst(inst), n_train(inst->n_train), n_fork(inst->n_fork)
{
}

void Solution::build_path(const uint train_idx)
{
	std::vector<uint>& path = this->paths[train_idx];
	auto train = this->get_train(train_idx); 

	for (uint fork_idx = train.fork_begin; 
		fork_idx < train.fork_end; fork_idx++) {
			
		this->fork_used[fork_idx] = false;
	}

	path.clear();
	path.push_back(0);

	while (true) {
		auto op = this->get_op(train_idx, path.back());

		// last element
		if (op.n_succ == 0) {
			break;
		}

		uint succ_idx;
		
		if (op.n_succ == 1) {
			succ_idx = op.succ[0];
		}
		else {
			succ_idx = op.succ[this->fork_choice[op.fork_idx]];
			this->fork_used[op.fork_idx] = true;
		}

		path.push_back(succ_idx);
	}
}


void Solution::build_all_paths()
{
	this->paths.clear();
	for (uint train_idx = 0; train_idx < this->n_train; train_idx++) {
		if (this->paths.size() <= train_idx) {
			this->paths.push_back(std::vector<uint>());
		}
		this->build_path(train_idx);
	}
}

void Solution::make_random_fork_choice()
{
	this->fork_choice.resize(this->n_fork);
	this->fork_used.resize(this->n_fork);

	for(uint fork_id = 0; fork_id < this->n_fork; fork_id++) {
		auto op = this->get_op(this->get_fork(fork_id));
		std::uniform_int_distribution<uint> dist(0, op.n_succ-1);
		this->fork_choice[fork_id] = dist(*this->inst->rng);
	}
	this->build_all_paths();
}

