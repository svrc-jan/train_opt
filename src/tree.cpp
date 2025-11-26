#include "tree.hpp"

#include <queue>

#include "iostream"

struct Path_step
{
	int op = -1;
	int start = -1;
	int end = -1;
};


Tree::Tree(const Instance& inst) : inst(inst), graph(Graph(inst))
{
	
}


bool Tree::solve()
{
	vector<int> order;
	vector<int> start_time;
	vector<int> node_prev;
	vector<int> node_succ;

	vector<pair<int, int>> path_order;

	order.reserve(this->graph.n_nodes);
	start_time.resize(this->graph.n_nodes);
	node_prev.resize(this->graph.n_nodes);
	node_succ.resize(this->graph.n_nodes);

	Res_col res_col;

	int it = 0;
	while (true) {
		it += 1;

		if (!this->graph.make_order(order, start_time, node_prev)) {
			cout << "it " << it << ", make_order failed" << endl;
			return false;
		}

		if (!this->make_node_succ(node_succ, node_prev)) {
			cout << "it " << it << ", make_node_succ failed" << endl;
			return false;
		}

		if (!this->find_res_col(res_col, order, start_time, node_prev, node_succ)) {
			break;
		}

		this->graph.add_path(res_col.first.unlock, node_prev);
		this->graph.add_path(res_col.second.unlock, node_prev);
		
		break;
	}
	

	
	this->make_node_succ(node_succ, node_prev);
	this->find_res_col(res_col, order, start_time, node_prev, node_succ);

	cout << res_col.first.unlock << ", " << res_col.second.lock << ", " << res_col.res << endl;

	return true;
}



bool Tree::make_node_succ(vector<int>& node_succ, const vector<int>& node_prev)
{
	for (int n = 0; n < this->graph.n_nodes; n++) {
		node_succ[n] = -1;
	}

	for (int n : this->graph.train_last_nodes) {
		int succ = n;
		int curr = node_prev[succ];

		node_succ[n] == -2; // signify end of train
		
		while (curr != -2) { // signify start of train
			if (curr == -1) { // disconnected path - invalid
				return false;
			}

			node_succ[curr] = succ;

			succ = curr;
			curr = node_prev[curr];
		}
	}

	return true;
}

struct Res_lock
{
	int first_node = -1;
	int curr_node = -1;
	int time = -1;
};


bool Tree::find_res_col(Res_col& res_col, const vector<int>& order, const vector<int>& start_time,
		const vector<int>& node_prev, const vector<int>& node_succ)
{
	vector<Res_lock> res_lock;
	
	res_lock.resize(this->inst.n_res());
	for (int r = 0; r < this->inst.n_res(); r++) {
		res_lock[r] = {-1, -1, -1};
	}

	res_col.res = -1;

	for (int n_out : order) {
		if (node_succ[n_out] == -1) {
			continue;
		}
		int time = start_time[n_out];

		int n_in = node_prev[n_out];

		if (n_in >= 0) {
			const auto& node_in = this->graph.nodes[n_in];

			for (const auto& res : node_in.res) {
				assert(res_lock[res.idx].curr_node == n_in);
				res_lock[res.idx].time = time + res.time;
			}
		}

		const auto& node_out = this->graph.nodes[n_out];
		for (const auto& res : node_out.res) {
			int release_time = (node_succ[n_out] == -2) ? time + res.time : -1;

			auto& rl = res_lock[res.idx];
			if (rl.curr_node == n_in) {
				rl.curr_node = n_out;
				rl.time = release_time;
			}

			else if (rl.curr_node == -1 || (rl.time <= time && rl.time != -1)) {
				rl = {
					.first_node = n_out,
					.curr_node = n_out,
					.time = release_time
				};
			}

			else {
				res_col.first = {
					.lock = rl.first_node,
					.unlock = rl.curr_node
				};
				res_col.second = {
					.lock = n_out,
					.unlock = n_out
				};

				res_col.res = res.idx;
				break;
			}
		}

		if (res_col.res != -1) {
			break;
		}
	}

	if (res_col.res == -1) {
		return false;
	}

	// find first unlock op
	while (true) {
		int n_succ = node_succ[res_col.first.unlock];
		if (n_succ < 0 || this->graph.nodes[n_succ].res.find_sorted(res_col.res) == -1) {
			break;
		}
		res_col.first.unlock = n_succ;
	}

	// find second unlock op
	while (true) {
		int n_succ = node_succ[res_col.second.unlock];
		if (n_succ < 0 || this->graph.nodes[n_succ].res.find_sorted(res_col.res) == -1) {
			break;
		}
		res_col.second.unlock = n_succ;
	}

	return true;
}

