#pragma once

#include "utils/array.hpp"
#include "instance.hpp"
#include "graph.hpp"

class Tree
{
public:
	int n_solve_calls;
	int obj_ub;

	vector<pair<int, int>> best_sol = {};

	Tree(const Instance& inst);
	void solve(int depth);

private:
	const Instance& inst;
	Graph graph;

	vector<int> order = {};

	void save_solution();
};
