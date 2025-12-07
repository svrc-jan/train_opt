#pragma once

#include <vector>
#include <deque>
#include <unordered_map>

#include "utils/hasher.hpp"
#include "preprocess.hpp"
#include "instance.hpp"



class Graph
{
public:
	struct Res_use;
	struct Node_ordering;
	
	std::vector<int> time;

	const Instance& inst;
	const Preprocess& prepr;
	
	Graph(const Preprocess& prepr);

	void add_all_paths(const std::vector<std::vector<int>>& paths);
	void add_path(const int train_idx, const std::vector<int>& path);

	bool make_order();
	bool make_time();

	bool find_res_col(std::pair<Node_ordering, Node_ordering>& node_ords);
	void extend_node_ordering(Node_ordering& node_ord);

	void add_res_cons(const Node_ordering& node_ord);
	void remove_last_res_cons(const Node_ordering& node_ord);

	int get_node_ordering_delay(const Node_ordering& node_ord);

	void clear_res_cons();

private:
	struct Res_cons;
	struct Node_forward;
	struct Node_backward;

	int n_nodes = 0;
	std::vector<int> order;

	std::vector<int> start_nodes = {};

	std::vector<std::vector<int>> paths = {};
	std::vector<std::vector<Res_use>> res_uses = {};

	std::vector<Node_forward> nodes_forw = {};
	std::vector<Node_backward> nodes_backw = {};
	std::vector<std::pair<int, int>> node_ops = {};

	// aux
	std::deque<int> dq;
};


struct Graph::Res_use
{
	int train = -1;
	int node_lock = -1;
	int node_unlock = -1;
	int res_time = -1;
};


struct Graph::Res_cons
{
	int node;
	int time;
};


struct Graph::Node_ordering
{
	int node_from = -1;
	int node_to = -1;

	int res_time = -1;
};


struct Graph::Node_forward
{
	int succ = -1;
	std::vector<int> res_cons_out = {};
};


struct Graph::Node_backward
{
	int pred = -1;
	int dur_in = 0;
	int time_lb = 0;
	int time_ub = INT_MAX;

	std::vector<Res_cons> res_cons_in = {};
};




