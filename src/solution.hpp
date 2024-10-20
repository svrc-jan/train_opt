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

	void build_path(const uint train_id);
	void build_all_paths();

	void make_random_fork_choice();


	inline const Operation& get_op(const uint train_id, const uint op_id)
	{	return this->inst->ops[train_id][op_id]; }

	// inline const Operation& get_op(const Sol_op sol_op)
	// {	return this->get_op(sol_op.train_id, sol_op.op_id); }

	inline const Operation& get_op(const Fork fork)
	{	return this->get_op(fork.train_id, fork.op_id); }

	inline const Fork& get_fork(const uint fork_id)
	{	return this->inst->forks[fork_id]; }

	// inline const Fork& get_fork(const Sol_op sol_op)
	// {	return this->inst->forks[this->get_op(sol_op).fork_id]; }

	inline const Train& get_train(const uint train_id)
	{	return this->inst->trains[train_id]; }
	

};
