#pragma once

#include <vector>
#include <utility>
#include <map>

#include "instance.hpp"

using std::vector;
using std::pair;

class Solution
{
private:
	const Instance& inst;
	Rand_int_gen& rng;

	vector<pair<int, int>> order;
	vector<vector<pair<int, int>>> paths;

public:
	Solution(const Instance& inst, Rand_int_gen& rng);

	vector<int>& make_random_path(int train);
	vector<vector<int>>& make_all_random_paths();

	std::map<int, int> get_earliest_starts();
};
