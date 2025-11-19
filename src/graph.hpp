#pragma once

#include <vector>
#include <algorithm>
#include <queue>

#include "utils/array.hpp"
#include "instance.hpp"


using namespace std;


struct Node_time
{
	int node = -1;
	int time = 0;

	inline bool operator<(const Node_time& other) const { return this->time > other.time; }
};

struct Event
{
	const Op* in_op = nullptr;
	const Op* out_op = nullptr;
	int time = 0;
};

struct Node
{
	uint32_t viable_out_flag = 0;

	// TODO: improve allocation
	vector<Node_time> res_edges = {};
};


class Graph
{
public:
	Graph(const Instance& inst);
	vector<int> get_order(vector<int>& time, vector<const Op*>& in_op);
	vector<Event> get_events();

	inline int n_nodes() const { return nodes.size(); }

private:
	const Instance& inst;

	vector<Node> nodes;
	vector<int> in_degree;
};


std::ostream& operator<<(std::ostream& stream, const Node_time& node_time);
std::ostream& operator<<(std::ostream& stream, const Event& event);
