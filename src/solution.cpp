#include "solution.hpp"


Solution::Solution(const Instance *inst) 
	: inst(inst)
{

}

void Solution::build_path(const uint train_id)
{
	std::vector<Sol_op>& path = this->paths[train_id];
	const Train& train = this->inst->trains[train_id]; 

	for (uint fork_idx = train.fork_begin; 
		fork_idx < train.fork_end; fork_idx++) {
			
		this->fork_used[fork_idx] = false;
	}

	path.clear();
	
	uint op_idx = this->inst->trains[train_id].op_begin;
	path.push_back(Sol_op());
	path[0].idx = op_idx;
	path[0].start = this->inst->ops[op_idx].start_lb;
	path[0].prev = nullptr;

	while (true) {
		const Operation& op = this->inst->ops[path.back().idx];
		const Sol_op& sol_op = path.back();

		if (op.n_succ == 0) {
			break;
		}

		Sol_op sol_succ;
		uint succ_idx;
		sol_succ.idx = succ_idx;

		if (op.n_succ == 1) {
			succ_idx = op.succ[0];
		}
		else {
			succ_idx = op.succ[this->fork_choice[op.fork_idx]];
			this->fork_used[op.fork_idx] = true;
		}

		const Operation& succ = this->inst->ops[succ_idx];

		sol_succ.start = std::max(succ.start_lb, sol_op.start + op.dur);
		sol_succ.prev = &path[path.size()-1];

		path.push_back(sol_succ);
	}
}

void Solution::build_all_paths()
{
	for (uint train_id = 0; train_id < this->inst->n_train; train_id++) {
		if (this->paths.size() <= train_id) {
			this->paths.push_back(std::vector<Sol_op>());
		}
		this->build_path(train_id);
	}
}

void Solution::make_random_fork_choice()
{
	if (this->fork_choice.size() != this->inst->n_fork) {
		this->fork_choice.resize(this->inst->n_fork);
	}
	if (this->fork_used.size() != this->inst->n_fork) {
		this->fork_used.resize(this->inst->n_fork);
	}

	for(uint i = 0; i < this->inst->n_fork; i++) {
		const Operation& op = this->inst->ops[this->inst->fork_idx_map[i]];
		std::uniform_int_distribution<uint> dist(0, op.n_succ-1);
		this->fork_choice[i] = dist(*this->inst->rng);
	}
	this->build_all_paths();
}

void Solution::make_resource_prio(const std::vector<uint> global_prio)
{
	this->resource_prio.push_back(global_prio);
	for (uint res_idx = 0; res_idx < this->inst->n_res; res_idx++) {
		this->resource_prio.push_back(
			std::vector<uint>(global_prio));
	}
}

void Solution::build_time_plan()
{
	const uint n_train = this->inst->n_train;
	std::vector<uint> path_idx(this->inst->n_train, 0);
	
	uint plan_size = 0;
	for (uint train_idx = 0; train_idx < n_train; train_idx++) {
		plan_size += this->paths[train_idx].size();
	}
	
	this->op_plan.resize(plan_size);

	std::vector<bool> res_locked(this->inst->n_res, false);

	uint plan_idx = 0;
	
	while (true) {
		
	}
	
}