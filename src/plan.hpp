#pragma once

#include <vector>

#include "instance.hpp"
#include "solution.hpp"

struct Plan_op
{
	uint train_idx = -1;
	uint op_idx = -1;
	// uint path_idx = -1;
	uint start = -1;

	bool operator<(const Plan_op& rhs)
	{	
		return this->start < rhs.start;
	}
};

struct Plan_res
{
	bool locked = false;
	uint release_time = 0;
};

class Plan
{
public:
	const uint& n_train;
	const Solution *sol = nullptr;

	uint plan_idx = -1;
	uint size = 0;

	std::vector<uint> paths_idx;
	std::vector<Plan_op> next_ops;
	
	std::vector<Plan_op> ops;
	std::vector<Plan_res> res;

	Plan(const Solution *sol);
	
	void resize_to_sol();
	void reset();
	void reset_next_ops();
	void build();

	bool is_train_unlocked(const uint train_idx);
	void lock_res(const uint res_idx);
	void unlock_res(const uint curr_train_idx, const uint res_idx, const uint unlock_time);
	void advance_train(const uint train_idx);
	
	inline const std::vector<std::vector<uint>>& get_paths() 
	{	return this->sol->paths; }

	inline const Operation& get_op(const uint train_idx, const uint op_idx)
	{	return this->sol->inst->ops[train_idx][op_idx]; }

	inline const Operation& get_op(const Plan_op& plan_op)
	{	return this->sol->inst->ops[plan_op.train_idx][plan_op.op_idx]; }

	inline uint get_path_op_idx(const uint train_idx, const uint path_id)
	{	return this->sol->paths[train_idx][path_id]; }

	inline const Operation& get_path_op(const uint train_idx, const uint path_idx)
	{	return this->sol->inst->ops[train_idx][this->sol->paths[train_idx][path_idx]]; }



	inline const Operation& get_op(const Fork fork)
	{	return this->get_op(fork.train_idx, fork.op_idx); }

	inline const Fork& get_fork(const uint fork_id)
	{	return this->sol->inst->forks[fork_id]; }

	inline const Train& get_train(const uint train_idx)
	{	return this->sol->inst->trains[train_idx]; }
};