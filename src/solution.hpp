#pragma once

#include "utils.hpp"
#include "instance.hpp"

struct Sol_graph
{
	unordered_map<pair<int, int>, int, Pair_hasher> dur = {};
	unordered_map<int, vector<int>> prev = {};
	unordered_map<int, vector<int>> succ = {};

	void add(int from, int to, int dur);
	void remove(int from, int to);

	inline int in_deg(int node)
	{ return this->prev[node].size(); }

	inline int out_deg(int node)
	{ return this->succ[node].size(); }
};

struct Res_use
{
	int lock = -1;
	int unlock = -1;
	int time = 0;
};


class Solution
{
public:
	const Instance& inst;
	Rand_int_gen& rng;

	vector<int> v_op;
	unordered_map<int, vector<Res_use>> mp_res_use;
	Sol_graph graph;

	Solution(const Instance& inst, Rand_int_gen& rng);
	void make_random_paths();
	void make_res_uses();

private:
	inline const Op& get_op(int idx) const
	{ return this->inst.v_op[idx]; }

		
};