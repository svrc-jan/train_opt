#pragma once

#include <vector>
#include <algorithm>

#include "utils/array.hpp"
#include "instance.hpp"

using namespace std;


struct Path_edge
{
	int node = -1;
	bool active = true;
};

struct Node
{
	int op = -1;
	int dur = 0;
	int start_lb = 0;

	Array<Path_edge> path_out = {nullptr, 0};
};


struct Node_interval
{
	int op_begin = -1;
	int op_end = -1;
	int node_begin = -1;

	inline bool is_op_in(int op_id) const
	{ return op_id >= this->op_begin && op_id < this->op_end; }

	inline int node_to_op(int node_id) const 
	{ return node_id + this->op_begin - this->node_begin; }

	inline int op_to_node(int op_id) const 
	{ return op_id + this->node_begin - this->op_begin; }

	bool operator<(const Node_interval& other) const
	{ return this->op_begin < other.op_begin; }
};




class Graph
{
public:
	Graph();
	~Graph();
	
	void add_train(const Train& train);

	int n_nodes() const { return this->nodes.size(); }

private:
	vector<Node> nodes = {};
	vector<Path_edge> node_path_out = {};
	vector<Node_interval> node_intervals = {};

	const Node_interval* find_node_interval(int op_id);
	const Node_interval* binary_search_node_interval(int op_id, int left, int right);

	void assign_path_out_pointers();
};
