#pragma once

#include "utils/array.hpp"
#include "instance.hpp"
#include "graph.hpp"

struct Res_interval
{
	int lock_time = -1;
	int unlock_time = -1;
	int op = -1;
};

struct Res_col
{
	struct {
		int lock = -1;
		int unlock = -1;
	} first;

	struct {
		int lock = -1;
		int unlock = -1;
	} second;

	int res = -1;
};

class Tree
{
public:
	Tree(const Instance& inst);
	bool solve();

private:
	const Instance& inst;
	Graph graph;

	bool make_node_succ(vector<int>& node_succ, const vector<int>& node_prev);
	bool find_res_col(Res_col& res_col, const vector<int>& order, const vector<int>& start_time,
		const vector<int>& node_prev, const vector<int>& node_succ);

};