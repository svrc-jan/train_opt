#pragma once

#include "instance.hpp"

class Solution
{
public:
	const Instance* inst = nullptr;
	std::vector<uint> branching_choice;
	std::vector<std::vector<uint>> paths;

	Solution(const Instance *inst);

	void build_path(const uint train_id);
	void build_all_paths();

	std::vector<uint> get_random_branching_choice();
};
