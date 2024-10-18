#pragma once

#include "instance.hpp"

struct Sol_op
{
	uint idx;
	uint start;
	Sol_op* prev;
};


struct Sol_res
{
	bool locked = false;
};

class Solution
{
public:
	const Instance* inst = nullptr;
	std::vector<uint> fork_choice;
	std::vector<bool> fork_used;

	std::vector<std::vector<uint>> resource_prio;
	std::vector<std::vector<Sol_op>> paths;

	std::vector<Sol_op *> op_plan;

	Solution(const Instance *inst);

	void build_path(const uint train_id);
	void build_all_paths();

	void make_random_fork_choice();
	void make_resource_prio(const std::vector<uint> global_prio);

	void build_time_plan();

};
