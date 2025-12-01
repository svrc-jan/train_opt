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

struct Res_cons
{
	int op;
	int time;
};


struct Node
{
	int time = 0;
	int old_time = 0;
	int next = -1;
	vector<Res_cons> res_cons_out = {};
};

class Graph
{
public:
	Graph(const Instance& inst);

	const int n_ops;
	const int n_res;

	void make_order();

private:
	const Instance& inst;

	// data
	vector<int> topo_order;

	vector<Node> nodes;

	// aux
	deque<int> dq;
	priority_queue<Prio_queue_item> pq;
	vector<int> n_pred;
};






