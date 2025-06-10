#pragma once

#include <vector>
#include <utility>
#include <map>

#include "instance.hpp"

using std::vector;
using std::pair;
using op_order_t = vector<pair<int, int>>;

class Solution
{
private:
	const Instance& inst;
	Rand_int_gen& rng;

	op_order_t order;
	vector<vector<pair<int, int>>> paths;

	std::map<int, int> earliest_start;
	std::map<int, int> latest_start;

public:
	Solution(const Instance& inst, Rand_int_gen& rng);

	op_order_t& make_random_path(size_t train);
	vector<op_order_t>& make_all_random_paths();
	std::map<int, int>& make_earliest_start();
	std::map<int, int>& make_latest_start();
	op_order_t& make_order();

	void print_order(std::ostream& os) const;
};


std::ostream& operator<< (std::ostream& os, const Solution& sol);