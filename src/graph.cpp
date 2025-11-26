#include "graph.hpp"

#include <iostream>
#include <algorithm>
#include <queue>


struct Order_item
{
	int node_in = -1;
	int node_out = -1;
	int time = 0;
};


class Order_comparator
{
public:
	bool operator()(const Order_item& a, const Order_item& b) {
		return a.time > b.time;
	}
};

using Order_prio_queue = priority_queue<Order_item, vector<Order_item>, Order_comparator>;


Graph::Graph(const Instance& inst) : inst(inst)
{
	this->make_chrono_order();
	this->make_op_data();
}


void Graph::make_chrono_order()
{	
	Order_prio_queue pq;

	this->op_chrono.set_size(this->inst.n_ops());
	this->res_chrono.set_size(this->inst.n_res());

	vector<int> visited(this->inst.n_ops(), false);

	for (auto& train : this->inst.trains) {
		int o = train.op_start;

		auto& start_op = inst.ops[o];

		pq.push({
			.node_in = -1,
			.node_out = o,
			.time = start_op.start_lb
		});
	}
	
	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();

		int o = curr.node_out;
		
		if (visited[o]) {
			continue;
		}

		visited[o] = true;
		this->op_chrono.add(o);

		int t = curr.time;
		auto& op = this->inst.ops[o];

		for (auto& res : op.res) {
			this->res_chrono.add(res.idx);
		}

		for (int s : op.succ) {
			if (visited[s]) {
				continue;
			}

			auto& succ = this->inst.ops[s];
			
			pq.push({
				.node_in = o,
				.node_out = s,
				.time = max(succ.start_lb, t + op.dur) 
			});
		}
	}

	assert(this->op_chrono.is_done() && this->res_chrono.is_done());
}


void Graph::make_op_data()
{
	this->n_nodes = this->inst.n_ops();

	this->nodes.reserve(this->n_nodes);
	this->node_train.reserve(this->n_nodes);
	
	this->_node_succ.reserve(this->inst.n_op_succ());
	this->_node_res.reserve(this->inst.n_op_res());

	for (int o : this->op_chrono.order) {
		const Op& op = this->inst.ops[o];

		Node node = {
			.dur = op.dur,
			.start_lb = op.start_lb,
			.start_ub = op.start_ub
		};

		for (int s : op.succ) {
			node.succ.size += 1;
			this->_node_succ.push_back(this->op_chrono.idx[s]);
		}

		for (const Res& res : op.res) {
			node.res.size += 1;
			this->_node_res.push_back({
				.idx = this->res_chrono.idx[res.idx],
				.time = res.time
			});
		}

		this->nodes.push_back(node);
		this->node_train.push_back(op.train);
	}

	int node_succ_idx = 0;
	int node_res_idx = 0;

	for (Node& node : this->nodes) {
		node.succ.assign_ptr(this->_node_succ, node_succ_idx);
		node.res.assign_ptr(this->_node_res, node_res_idx);

		node.succ.sort();
		node.res.sort();
	}

	this->node_valid.resize(this->n_nodes);
	for (int n = 0; n < this->n_nodes; n++) {
		this->node_valid[n] = true;
	}

	this->node_path.resize(this->n_nodes);
	for (int n = 0; n < this->n_nodes; n++) {
		this->node_path[n] = -1;
	}

	this->node_res_cons_idx.resize(this->n_nodes);
	for (int n = 0; n < this->n_nodes; n++) {
		this->node_res_cons_idx[n] = -1;
	}

	this->train_start_nodes.reserve(this->inst.n_trains());
	this->train_last_nodes.reserve(this->inst.n_trains());

	for (const Train& train : this->inst.trains) {
		train_start_nodes.push_back(this->op_chrono.idx[train.op_start]);
		train_last_nodes.push_back(this->op_chrono.idx[train.op_last()]);
	}
}

enum Node_state_enum {
	PREV_WAIT, // wait for any previous node
	PATH_WAIT, // wait for path node
	RES_WAIT,  // wait for res
	STARTED,   // op is started
	ENDED,     // op has ended
};

enum Node_prev_enum {
	DEFAULT_PREV = -1,
	START_PREV = -2,
	RES_PREV = -3
};

bool Graph::make_order(vector<int>& order, vector<int>& start_time, vector<int>& node_prev)
{
	order.clear();
	for (int n = 0; n < this->n_nodes; n++) {
		node_prev[n] = DEFAULT_PREV;
	}

	vector<int> state(this->inst.n_ops());
	for (int n = 0; n < this->n_nodes; n++) {
		state[n] = PREV_WAIT;
	}

	for (int n = 0; n < this->n_nodes; n++) {
		int p = this->node_path[n];
		if (p >= 0) {
			state[p] = PATH_WAIT;
		}
	}

	vector<int> n_res_cons_done(this->inst.n_ops());
	for (int n = 0; n < this->n_nodes; n++) {
		n_res_cons_done[n] = 0;
	}

	Order_prio_queue pq;

	for (int n : this->train_start_nodes) {

		state[n] = PATH_WAIT;
		const Node& node = this->nodes[n];

		pq.push({
			.node_in = START_PREV,
			.node_out = n,
			.time = node.start_lb
		});
	}

	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();

		int n = curr.node_out;
		const auto& node = this->nodes[n];

		if (state[n] < RES_WAIT && (curr.node_in >= 0 || curr.node_in == START_PREV)) {
			node_prev[n] = curr.node_in;
			state[n] = RES_WAIT;
		}

		if (curr.node_in == RES_PREV) {
			n_res_cons_done[n] += 1;
		}

		if (state[n] != RES_WAIT || n_res_cons_done[n] < node.n_in_res_cons) {
			continue;
		}

		int t = curr.time;

		int n_prev = node_prev[n];
		if (n_prev >= 0 && state[n_prev] == STARTED) {
			state[n_prev] = ENDED;

			int res_cons_idx = this->node_res_cons_idx[n_prev];
			while (res_cons_idx >= 0) {
				Res_cons& rc = this->res_cons[res_cons_idx];
				pq.push({
					.node_in = RES_PREV,
					.node_out = rc.node,
					.time = t + rc.time
				});

				res_cons_idx = rc.next_idx;
			}
		}

		state[n] = STARTED;

		if (t > node.start_ub) {
			return false;
		}
		
		order.push_back(n);
		start_time[n] = t;

		int p = this->node_path[n];
		if (p >= 0) {
			if (!this->node_valid[p]) {
				return false;
			}

			node_prev[p] = n;
			
			auto& path_node = this->nodes[p];
			if (path_node.start_ub < t + node.dur) {
				return false;
			}

			pq.push({
				.node_in = n,
				.node_out = p,
				.time = max(path_node.start_lb, t + node.dur) 
			});
		}

		else {
			for (int s : this->nodes[n].succ) {
				if (!this->node_valid[s] || state[s] > PREV_WAIT) {
					continue;
				}

				auto& succ_node = this->nodes[s];

				pq.push({
					.node_in = n,
					.node_out = s,
					.time = max(succ_node.start_lb, t + node.dur) 
				});
			}
		}		
	}

	return true;
}


pair<int, int> Graph::add_path(int node, const vector<int>& node_prev)
{
	int curr = node;
	int prev = node_prev[curr];
	while (prev >= 0 && this->node_path[prev] == -1) {
		this->node_path[prev] = curr;

		curr = prev;
		prev = node_prev[curr];
	}
	
	assert(prev < 0 || this->node_path[prev] == curr);
	// this->print_train_path(this->node_train[node]);

	return {curr, node};
}


int Graph::add_res_cons(int node_from, int node_to, int res)
{
	int res_cons_idx = this->node_res_cons_idx[node_from];
	if (res_cons_idx == -1) {
		this->node_res_cons_idx[node_from] = this->res_cons.size();
	}
	else {
		while (this->res_cons[res_cons_idx].next_idx >= 0) {
			res_cons_idx = this->res_cons[res_cons_idx].next_idx;
		}	
		this->res_cons[res_cons_idx].next_idx = this->res_cons.size();
	}

	const auto& res_arr = this->nodes[node_from].res;
	int time = res_arr[res_arr.find_sorted(res)].time;

	this->res_cons.push_back({
		.node = node_to,
		.time = time,
		.next_idx = -1
	});
	
	this->nodes[node_to].n_in_res_cons += 1;
	
	return res_cons_idx;
}


void Graph::remove_last_res_cons(int node_from, int node_to, int res_cons_idx)
{
	this->res_cons.pop_back();
	if (res_cons_idx == -1) {
		this->node_res_cons_idx[node_from] = -1;
	}
	else {
		this->res_cons[res_cons_idx].next_idx = -1;
	}

	this->nodes[node_to].n_in_res_cons -= 1;
}


void Graph::print_train_path(int train)
{
	cout << "train " << train << " path:";
	int curr = this->train_start_nodes[train];

	while (curr >= 0) {
		cout << " " << curr;
		curr = this->node_path[curr];
	}

	cout << endl;
}