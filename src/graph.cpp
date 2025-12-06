#include "graph.hpp"

#include <iostream>
#include <queue>

Graph::Graph(const Instance& inst, const Preprocess& prepr)
	: inst(inst), prepr(prepr)
{
	for (int t = 0; t < this->inst.n_trains(); t++) {
		this->paths.push_back({});
	}
	
	for (int r = 0; r < this->inst.n_res(); r++) {
		res_uses.push_back({});
	}

	this->n_nodes = this->prepr.n_levels();
	this->nodes_forw.resize(this->n_nodes, Node_forward());
	this->nodes_backw.resize(this->n_nodes, Node_backward());
}


void Graph::add_all_paths(const vector<vector<int>>& paths)
{
	assert((int)paths.size() == this->inst.n_trains());

	for (int t = 0; t < this->inst.n_trains(); t++) {
		this->add_path(t, paths[t]);
	}
}


void Graph::add_path(const int train_idx, const vector<int>& path)
{
	this->paths[train_idx] = path;

	this->start_nodes.push_back(this->prepr.trains[train_idx].level_start);
	
	for (int o : path) {
		auto& op = this->inst.ops[o];

		int nd_start = this->prepr.op_level[o].first;
		int nd_end = this->prepr.op_level[o].second;

		auto& node_fw = this->nodes_forw[nd_start];
		node_fw.succ = nd_end;
		node_fw.res_cons_out.clear();

		auto& node_bnds = this->nodes_backw[nd_start];
		node_bnds.time_lb = op.start_lb;
		node_bnds.time_ub = op.start_ub;

		auto& node_bw = this->nodes_backw[nd_end];
		node_bw.pred = nd_start;
		node_bw.pred_dur = op.dur;
		node_bw.res_cons_in.clear();
	}

	for (int o : path) {
		auto& op = this->inst.ops[o];
		for (auto& res : op.res) {
			auto& ru = this->res_uses[res.idx];
			if (ru.size() > 0 && ru.back().train == train_idx) {
				ru.back().node_unlock = this->prepr.op_level[o].second;
				ru.back().res_time = max(res.time, 1);
			}
			else {
				ru.push_back({
					.train = train_idx,
					.node_lock = this->prepr.op_level[o].first,
					.node_unlock = this->prepr.op_level[o].second,
					.res_time = max(res.time, 1)
				});
			}
		}
	}
}


bool Graph::make_order()
{
#ifndef NO_VLA
	int n_pred[this->n_nodes];
#else
	vector<int> n_pred(this->n_nodes);
#endif

	for (int i = 0; i < this->n_nodes; i++) {
		const auto& node_bw = this->nodes_backw[i];
		n_pred[i] = (node_bw.pred >= 0) + node_bw.res_cons_in.size();
	}

	this->order.clear();
	for (int i : this->start_nodes) {
		dq.push_back(i);
	}

	int paths_done = 0;
	
	while (!dq.empty())	{
		int i = dq.front();
		dq.pop_front();

		const auto& node_fw = this->nodes_forw[i];
		if (node_fw.succ >= 0) {
			int j = node_fw.succ;
			n_pred[j] -= 1;
			if (n_pred[j] == 0) {
				dq.push_front(j); // push front to help locality - same train
			}
		}
		else {
			paths_done += 1;
		}

		for (int j : node_fw.res_cons_out) {
			n_pred[j] -= 1;
			if (n_pred[j] == 0) {
				dq.push_back(j);
			}
		}
	}

	return paths_done == (int)this->start_nodes.size();
}

bool Graph::make_time()
{
	auto& t = this->time;
	for (int i : this->order) {
		const auto& node_bw = this->nodes_backw[i];
		
		t[i] = node_bw.time_lb;
		if (node_bw.pred >= 0) {
			t[i] = max(t[i], t[node_bw.pred] + node_bw.pred_dur);
		}

		for (const auto& rc : node_bw.res_cons_in) {
			t[i] = max(t[i], t[rc.node] + rc.time);
		}

		if (t[i] > node_bw.time_ub) {
			return false;
		}
	}

	return true;
}

struct Interval
{
	int idx;
	int start;
	int end;

	inline bool operator<(const Interval& other) 
	{
		return (this->start < other.start) ? true 
			: ((this->start > other.start) ? false : (this->end < other.end));
	}
};


bool Graph::get_res_col(Res_col& res_col)
{
	int res_col_time = INT_MAX;
	int res_col_overlap = 0;

	for (auto& ru : this->res_uses) {
		int n_intervals = ru.size();
#ifndef NO_VLA
		Interval intervals[n_intervals];
#else
		vector<Interval> intervals(n_intervals);
#endif	

		for (int i = 0; i < n_intervals; i++) {
			intervals[i] = {
				.idx = i,
				.start = this->time[ru[i].node_lock],
				.end = this->time[ru[i].node_unlock] + ru[i].res_time
			};
		}

#ifndef NO_VLA
		sort(intervals, intervals + n_intervals);
#else
		sort(intervals.begin(), intervals.end());
#endif	
		for (int i = 0; i < n_intervals; i++) {
			const auto& a = intervals[i];
			if (a.end > res_col_time) {
				break;
			}

			for (int j = i + 1; i < n_intervals; i++) {
				const auto& b = intervals[j];

				if (a.end <= b.start) {
					break; // all further intervals start later
				}
				else {
					int overlap = a.end - b.start;
					if (b.start < res_col_time 
						|| (b.start < res_col_time && overlap > res_col_overlap)) {
						
						res_col_time = b.start;
						res_col_overlap = overlap;
						
						res_col.first = ru[a.idx];
						res_col.second = ru[b.idx];
					}
				}
			}
		}
	}

	return res_col_time == INT_MAX;
}
