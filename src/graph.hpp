#pragma once

#include <vector>
#include <tuple>

#include "utils/array.hpp"
#include "utils/chrono_tracker.hpp"
#include "instance.hpp"

using namespace std;


struct Res_cons
{
	int node = -1;
	int time = 0;
	int next = -1;
};

struct Node
{
	int path = -1;
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	int res_cons = -1;
	int n_in_res_cons = 0;

	Array<int> succ = {nullptr, 0};
	Array<Res> res = {nullptr, 0};
};


class Graph
{
public:
	int n_nodes = 0;
	vector<Node> nodes = {};

	vector<int> node_train = {};
	vector<int> node_valid = {};

	vector<int> train_start_nodes = {};
	vector<int> train_last_nodes = {};

	vector<int> res_cons = {};

	Graph(const Instance& inst);

	bool make_order(vector<int>& order, vector<int>& start_time, vector<int>& node_prev);
	void add_path(int node, const vector<int>& node_prev);

private:
	const Instance& inst;

	Chrono_tracker op_chrono;
	Chrono_tracker res_chrono;

	vector<int> _node_succ = {};
	vector<Res> _node_res = {};
	
	void make_chrono_order();
	void make_op_data();
};

