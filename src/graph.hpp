#pragma once

#include <vector>
#include <deque>
#include <queue>

#include "instance.hpp"

using namespace std;

struct Prio_queue_item
{
	int op;
	int time;

	bool operator<(const Prio_queue_item& other) { return this->time < other.time; }
};


struct Res_uses
{
	int train;
	int node_lock;
	int node_unlock;
	int res_time;
};


struct Res_ints
{
	int train;
};

struct Node_idx
{
	int train;
	int node;
};


struct Res_cons
{
	Node_idx nidx;
	int time;
};


class Graph
{
public:
	Graph(const Instance& inst);

	void add_path(const int train_idx, const vector<int>& path);
	void make_order();

private:
	const Instance& inst;

	int n_nodes = 0;
	vector<int> node_idx;
	vector<Node_idx> order;

	vector<vector<int>> paths = {};

	vector<vector<Node_idx>> res_cons_out = {};
	vector<vector<Res_cons>> res_cons_in = {};

	vector<vector<Res_uses>> res_uses = {};

	// aux
	deque<int> dq;
};






