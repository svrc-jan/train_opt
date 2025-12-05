#include "graph.hpp"

#include <iostream>
#include <queue>

Graph::Graph(const Instance& inst) : inst(inst)
{
	for (int t = 0; t < this->inst.n_trains(); t++) {
		this->paths.push_back({});
		this->res_uses.push_back({});
		this->res_cons_out.push_back({});
		this->res_cons_in.push_back({});
	}
	
	for (int r = 0; r < this->inst.n_res(); r++) {
		res_uses.push_back({});
	}
}

void Graph::add_path(const int train_idx, const vector<int>& path)
{
	this->paths[train_idx] = path;
	
	int path_len = path.size();
	for (int i = 0; i < path_len; i++) {
		auto& op = this->inst.ops[path[i]];
		for (auto& res : op.res) {
			auto& ru = this->res_uses[res.idx];
			if (ru.size() > 0 && ru.back().train == train_idx) {
				ru.back().node_unlock = (i < path_len - 1) ? i + 1 : -1;
				ru.back().res_time = res.time;
			}
			else {
				ru.push_back({
					.train = train_idx,
					.node_lock = i,
					.node_unlock = (i < path_len - 1) ? i + 1 : -1,
					.res_time = res.time
				});
			}
		}
	}
}


void Graph::make_order()
{
	this->order.clear();

}