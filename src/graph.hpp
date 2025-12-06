#pragma once

#include <vector>
#include <deque>
#include <queue>

#include "preprocess.hpp"
#include "instance.hpp"

using namespace std;

struct Prio_queue_item
{
	int op;
	int time;

	bool operator<(const Prio_queue_item& other) { return this->time < other.time; }
};


struct Res_use
{
	int train;
	int node_lock;
	int node_unlock;
	int res_time;
};

using Res_col = pair<Res_use, Res_use>;

struct Res_cons
{
	int node;
	int time;
};


struct Node_forward
{
	int succ = -1;
	vector<int> res_cons_out = {};
};


struct Node_backward
{
	int pred = -1;
	int pred_dur = 0;
	int time_lb = 0;
	int time_ub = 0;

	vector<Res_cons> res_cons_in = {};
};

class Graph
{
public:
	Graph(const Instance& inst, const Preprocess& prepr);

	void add_all_paths(const vector<vector<int>>& paths);
	void add_path(const int train_idx, const vector<int>& path);
	bool make_order();
	bool make_time();
	bool get_res_col(Res_col& res_col);

private:
	const Instance& inst;
	const Preprocess& prepr;

	int n_nodes = 0;
	vector<int> order;
	vector<int> time;

	vector<int> start_nodes = {};

	vector<vector<int>> paths = {};
	vector<vector<Res_use>> res_uses = {};

	vector<Node_forward> nodes_forw = {};
	vector<Node_backward> nodes_backw = {};

	// aux
	deque<int> dq;
};






