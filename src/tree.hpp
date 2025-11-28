#pragma once

#include "utils/array.hpp"
#include "instance.hpp"
#include "graph.hpp"

class Tree
{
public:
	int n_solve_calls;

	Tree(const Instance& inst);
	bool solve(int depth);

private:
	const Instance& inst;
	Graph graph;

	vector<int> order;
	vector<int> start_time;
	vector<int> node_prev;
	vector<int> node_succ;

	bool make_node_succ();
	bool find_res_col(Res_col& res_col);

};