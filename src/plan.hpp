#pragma once

#include <vector>

#include "instance.hpp"
#include "solution.hpp"

struct Plan_op
{
	uint train_idx = -1;
	uint op_idx = -1;
	uint path_idx = -1;
	uint start = -1;
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
	
	std::vector<Plan_op> ops;
	std::vector<Plan_res> res;

	Plan(const Solution *sol);
	
	void resize_to_sol();
	void reset();
	void build();	
	
	inline const std::vector<std::vector<uint>>& get_paths() 
	{	return this->sol->paths; }

	inline const Operation& get_op(const uint train_idx, const uint op_idx)
	{	return this->sol->inst->ops[train_idx][op_idx]; }

	inline const Operation& get_path_op(const uint train_idx, const uint path_idx)
	{	return this->sol->inst->ops[train_idx][this->sol->paths[train_idx][path_idx]]; }

	inline const Operation& get_op(const Fork fork)
	{	return this->get_op(fork.train_idx, fork.op_idx); }

	inline const Fork& get_fork(const uint fork_id)
	{	return this->sol->inst->forks[fork_id]; }

	inline const Train& get_train(const uint train_idx)
	{	return this->sol->inst->trains[train_idx]; }
};