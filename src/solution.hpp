#pragma once

#include "instance.hpp"

class Solution
{
public:
	const uint& n_train;
	const uint& n_fork;
	const Instance* inst = nullptr;

	std::vector<uint> fork_choice;
	std::vector<bool> fork_used;

	std::vector<std::vector<uint>> paths;

	Solution(const Instance *inst);

	void build_path(const uint train_idx);
	void build_all_paths();

	void make_random_fork_choice();


	inline const Operation& get_op(const uint train_idx, const uint op_idx)
	{	return this->inst->ops[train_idx][op_idx]; }

	inline const Operation& get_path_op(const uint train_idx, const uint path_id)
	{	return this->inst->ops[train_idx][this->paths[train_idx][path_id]]; }

	// inline const Operation& get_op(const Sol_op sol_op)
	// {	return this->get_op(sol_op.train_idx, sol_op.op_idx); }

	inline const Operation& get_op(const Fork fork)
	{	return this->inst->ops[fork.train_idx][fork.op_idx]; }

	inline const Fork& get_fork(const uint fork_id)
	{	return this->inst->forks[fork_id]; }

	// inline const Fork& get_fork(const Sol_op sol_op)
	// {	return this->inst->forks[this->get_op(sol_op).fork_id]; }

	inline const Train& get_train(const uint train_idx)
	{	return this->inst->trains[train_idx]; }
	

};
