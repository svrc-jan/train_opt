#pragma once

#include <vector>
#include <list>
#include <utility>
#include <map>

#include "instance.hpp"

using std::vector;
using std::pair;
using std::list;


class Solution
{
private:
	const Instance& inst;
	Rand_int_gen& rng;
	
	vector<vector<pair<int, int>>> paths;
	vector<bool> path_inserted;

	std::map<int, int> earliest_start;
	std::map<int, int> latest_start;


public:
	list<pair<int, int>> order;

	Solution(const Instance& inst, Rand_int_gen& rng);

	const vector<pair<int, int>>& make_random_path(size_t train);
	const vector<vector<pair<int, int>>>& make_all_random_paths();
	const std::map<int, int>& make_earliest_start();
	const std::map<int, int>& make_latest_start();
	const list<pair<int, int>>& make_order();

	int count_collisions(const list<pair<int, int>>& ord) const;
	int get_res_overlap_diff(
		const pair<int, int>& a,
		const pair<int, int>& b) const;

	bool forward_reorder();

	void print_order(std::ostream& os) const;
};


std::ostream& operator<<(std::ostream& os, const Solution& sol);

vector<int>& operator-=(vector<int>& vec, const Operation& op);
vector<int>& operator+=(vector<int>& vec, const Operation& op);