#include "schedule.hpp"

using namespace std;

Schedule::Schedule(Graph& graph)
	: inst(graph.inst), prepr(graph.prepr), graph(graph)
{
	this->n_trains = this->inst.n_trains();
	this->n_res = this->inst.n_res();

	this->paths.resize(this->n_trains);
	// this->path_idx.resize(this->n_trains);
}


void Schedule::set_all_paths(const vector<vector<int>>& paths)
{
	for (int t = 0; t < this->n_trains; t++) {
		this->set_path(t, paths[t]);
	}
}


void Schedule::set_path(const int train_idx, const vector<int>& path)
{
	this->paths[train_idx] = path;
}


bool Schedule::process_from_start(Res_edges& res_edges)
{
#ifndef NO_VLA
	Res_use res_uses[this->n_res];
#else
	vector<Res_use> res_uses(this->n_res);
#endif
	for (int r = 0; r < this->n_res; r++) {
		res_uses[r] = Res_use();
	}

	// clean up queue
	while (!prio_queue.empty()) {
		prio_queue.pop();
	}

	for (int t = 0; t < this->n_trains; t++) {
		Event e;
		e.idx = {.train = t, .path = 0};
		e.time = this->get_time_start(e.idx);

		prio_queue.push(e);
	}

	while (!prio_queue.empty()) {
		Event curr = prio_queue.top();
		prio_queue.pop();
		
		TIME_T t = this->get_time_start(curr.idx);

		if (TIME_GREATER(t, prio_queue.top().time)) {
			curr.time = t;
			prio_queue.push(curr);

			continue;
		}

		auto& op = this->inst.ops[this->get_op(curr.idx)];
		for (auto& res : op.res) {
			auto& ru = res_uses[res.idx];

			if (ru.idx.train == curr.idx.train || ru.idx.train < 0) {
				ru.idx = curr.idx;
				ru.res_time = res.time;
				
				continue;
			}

			TIME_T unlock_time = this->get_time_unlock(ru);

			if (TIME_GREATER(unlock_time, t)) {
				this->make_res_edges(res_edges, res.idx, ru, {curr.idx, res.time});
				return false;
			}

			ru.idx = curr.idx;
			ru.res_time = res.time;
		}
		
		Event succ;
		succ.idx = curr.idx.succ();
		if (succ.idx.path < this->path_size(succ.idx.train)) {
			succ.time = this->get_time_start(succ.idx);

			prio_queue.push(succ);
		}
	}

	return true;
}


void Schedule::make_res_edges(Res_edges& res_edges, const int res_idx, 
	const Res_use& res_use1, const Res_use& res_use2)
{
	
}


int Schedule::get_level_unlock(const int res_idx, const Idx& idx)
{
	
}
