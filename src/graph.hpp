#pragma once

#include <vector>
#include <tuple>

#include "utils/array.hpp"
#include "instance.hpp"

using namespace std;

struct Res_cons
{
	int node = -1;
	int time = -1;
};

class Graph
{
public:
	Graph(const Instance& inst);

	bool get_order(vector<int>& order, vector<int>& time, vector<int>& op_in);

	void add_res_cons(int from, int to, int time);

private:
	const Instance& inst;

	vector<vector<Res_cons>> res_cons = {};
	vector<int> n_in_res_cons = {};
};

