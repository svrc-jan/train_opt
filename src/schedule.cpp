#include "schedule.hpp"

using namespace std;

Schedule::Schedule(Graph& graph)
	: inst(graph.inst), prepr(graph.prepr), graph(graph)
{
	this->n_trains = this->inst.n_trains();
	this->n_res = this->inst.n_res();

	this->paths.reserve(this->n_trains);
	this->train_res_uses.reserve(this->n_trains);
	this->train_ru_idx.resize(this->n_trains, 0);

	for (int t = 0; t < this->n_trains; t++) {
		this->paths.push_back({});
		this->train_res_uses.push_back({});
	}
	
	this->res_intervals.resize(this->n_res);
	for (int r = 0; r < this->n_res; r++) {
		this->res_intervals.push_back({});
	}
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
	
#ifndef NO_VLA
	Res_use res_uses[this->n_res];
#else
	vector<Res_use> res_uses(this->n_res);
#endif

	for (int r = 0; r < this->n_res; r++) {
		res_uses[r] = Res_use();
	}

	for (int o : path) {
		auto& op = this->inst.ops[o];
		
		int level_start = this->prepr.op_level[o].first;
		int level_end = this->prepr.op_level[o].second;

		for (auto& res : op.res) {
			auto& ru = res_uses[res.idx];
			if (ru.level_lock >= 0) {
				ru.level_unlock = level_end;
				ru.res_time = (TIME_T)res.idx;
			}
			else {
				ru = {
					.res_idx = res.idx,
					.level_lock = level_start,
					.level_unlock = level_end,
					.res_time = (TIME_T)res.time
				};
			}
		}
	}

	this->train_ru_idx[train_idx] = 0;
	this->train_res_uses[train_idx].clear();
	for (const auto& ru : res_uses) {
		if (ru.res_idx >= 0) {
			this->train_res_uses[train_idx].push_back(ru);
		}
	}

	sort(this->train_res_uses[train_idx].begin(), this->train_res_uses[train_idx].end());
}


bool Schedule::process_from_start(Res_edges& res_edges)
{
	for (auto& ri : this->res_intervals) {
		ri.clear();
	}

	prio_queue.clear();
	for (int t = 0; t < this->n_trains; t++) {
		int ru_idx = this->train_ru_idx[t];
		const auto& ru = this->train_res_uses[t][ru_idx];

		Res_interval ri = {
			.train = t,
			.res_use_idx = ru_idx,
		};
		this->update_res_inteval(ri);
		this->prio_queue.push_back(ri);
	}

	make_heap(this->prio_queue.begin(), this->prio_queue.begin());

	while (!this->prio_queue.empty()) {
		pop_heap(this->prio_queue.begin(), this->prio_queue.begin());
		auto ri_curr = this->prio_queue.back();
		this->prio_queue.pop_back();

		auto& ru_curr = this->get_res_use(ri_curr);

		auto& res_ints = this->res_intervals[ru_curr.res_idx];
		if (res_ints.size() > 0) {
			auto& ri_last = res_ints.back();
			if (ri_curr.time_lock < ri_last.time_unlock) {
				const auto& ru_last = this->get_res_use(ri_last);
				this->make_res_edges(res_edges, ru_last, ru_curr);

				return false;
			}
		}		

		res_ints.push_back(ri_curr);

		if (ri_curr.res_use_idx + 1 < (int)this->train_res_uses[ri_curr.train].size()) {
			Res_interval ri_new = {
				.train = ri_curr.train,
				.res_use_idx = ri_curr.res_use_idx + 1,
			};

			this->update_res_inteval(ri_new);
			this->prio_queue.push_back(ri_new);
			push_heap(this->prio_queue.begin(), this->prio_queue.end());
		}
	}

	return true;
}


void Schedule::update_res_inteval(Res_interval& ri)
{
	const auto& ru = this->train_res_uses[ri.train][ri.res_use_idx];

	ri.time_lock = this->graph.get_time(ru.level_lock);
	ri.time_unlock = this->graph.get_time(ru.level_unlock) + ru.res_time;
}


void Schedule::make_res_edges(Res_edges& res_edges, const Res_use& a, const Res_use& b)
{
	res_edges.first = {
		.vertex_from = a.level_unlock,
		.vertex_to = b.level_lock,
		.time = a.res_time
	};

	res_edges.second = {
		.vertex_from = b.level_unlock,
		.vertex_to = a.level_lock,
		.time = b.res_time
	};
}