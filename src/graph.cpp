#include "graph.hpp"

#include <print>
#include <queue>

using namespace std;

Graph::Graph(const Preprocess& prepr)
	: inst(prepr.inst), prepr(prepr)
{
	this->set_num_vertices(prepr.n_levels());
	this->clear_constrains();
}


void Graph::set_num_vertices(const int n_ver)
{
	this->n_ver = n_ver;

	this->time.resize(n_ver, 0);
	this->forward.resize(n_ver, Vertex_forward());
	this->backward.resize(n_ver, Vertex_backward());
	this->time_bounds.resize(n_ver, Vertex_time_bounds());
}


void Graph::clear_constrains()
{
	for (auto& vtx_forward : this->forward) {
		vtx_forward.constrains.clear();
	}

	for (auto& vtx_backward : this->backward) {
		vtx_backward.constrains.clear();
	}
}


void Graph::set_all_paths(const std::vector<std::vector<int>>& paths)
{
	for (auto& path : paths) {
		this->set_path(path);
	}
}


void Graph::set_path(const vector<int>& path)
{
	// assume path is valid, no checking is done
	for (int o : path) {
		this->set_path_op(o);
	}

	int v_path_start = this->prepr.op_level[path[0]].first;
	int v_path_end = this->prepr.op_level[path.back()].second;

	this->backward[v_path_start].path = {.vertex = -1, .time = 0};
	this->forward[v_path_start].path = -1;
}


void Graph::set_path_op(const int op)
{
	auto& inst_op = this->inst.ops[op];

	int v_start = this->prepr.op_level[op].first;
	int v_end = this->prepr.op_level[op].second;

	auto& vtx_forward = this->forward[v_start];
	auto& vtx_backward = this->backward[v_end];
	auto& vtx_time_bounds = this->time_bounds[v_start];

	vtx_forward.path = v_end;
	vtx_backward.path = {.vertex = v_start, .time = inst_op.dur};
	vtx_time_bounds = {.lower = inst_op.start_lb, .upper = inst_op.start_ub};
}


bool Graph::add_edge(const Edge& edge, const bool check_ub)
{
	if (!this->find_updates(edge.vertex_from,  edge.vertex_to)) {
		return false;
	}

	if (!this->update_time(check_ub)) {
		this->restore_time_changes(this->time_changes);

		return false;
	}

	this->forward[edge.vertex_from].constrains.push_back(edge.vertex_to);
	this->backward[edge.vertex_from].constrains.push_back({
		.vertex = edge.vertex_to,
		.time = edge.time
	});

	return true;
}

bool Graph::remove_last_edge(const Edge& edge)
{
	auto& forward_cons = this->forward[edge.vertex_from].constrains;
	auto& backward_cons = this->backward[edge.vertex_from].constrains;

	if (forward_cons.back() != edge.vertex_to 
		|| backward_cons.back().vertex != edge.vertex_from) {

		return false;
	}

	forward_cons.pop_back();
	backward_cons.pop_back();

	return true;
};



bool Graph::find_updates(const int v_from, const int v_cycle)
{
	this->to_update.clear();
	this->clear_update_flag();

	dq.push_back(v_from);
	
	while (!dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_back();

		const auto& vtx_forward = this->forward[v_curr];

		this->to_update.push_back(v_curr);
		this->set_update_flag(v_curr);

		int v_path = vtx_forward.path;
		if (v_path >= 0) {
			if (v_path == v_cycle) {
				return false;
			}

			if (!this->get_update_flag(vtx_forward.path)) {
				this->set_update_flag(vtx_forward.path);
				dq.push_back(vtx_forward.path);
			}
		}

		for (int v_cons : vtx_forward.constrains) {
			if (v_cons == v_cycle) {
				return false;
			}

			this->set_update_flag(v_cons);
			dq.push_front(v_cons);
		}
	}

	return true;
}

bool Graph::update_time(const bool check_ub)
{
	sort(this->to_update.begin(), this->to_update.end());
	int n_upd = this->to_update.size();
	
#ifndef NO_VLA
	char in_deg[this->n_ver];
#else
	vector<char> in_deg(this->n_ver);
#endif

	for (int v : this->to_update) {
		const auto& vtx_backward = this->backward[v];

		char d = 0;
		int v_path = vtx_backward.path.vertex;
		if (v_path >= 0 && this->get_update_flag(v_path)) {

			d += 1;
		};

		for (auto& cons : vtx_backward.constrains) {
			if (this->get_update_flag(cons.vertex)) {
				d += 1;
			}
		}

		if (d > 0) {
			in_deg[v] = d;
		}
		else {
			this->dq.push_back(v);
		}
	}

	this->time_changes.clear();

	while (!this->dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_front();

		const auto& vtx_forward = this->forward[v_curr];
		const auto& vtx_backward = this->backward[v_curr];
		const auto& vtx_time_bounds = this->time_bounds[v_curr];

		TIME_T new_time = vtx_time_bounds.lower;
		if (vtx_backward.path.vertex >= 0) {
			new_time = max(new_time, this->time[vtx_backward.path.vertex]); 
		};

		for (auto& cons : vtx_backward.constrains) {
			new_time = max(new_time, this->time[cons.vertex]);
		}
		
		if (check_ub && TIME_GREATER(new_time, vtx_time_bounds.upper)) {
			return false;
		}

		if (!TIME_EQUAL(new_time, this->time[v_curr])) {
			this->time_changes.push_back({v_curr, this->time[v_curr]});
			this->time[v_curr] = new_time;
		}

		int v_path = vtx_backward.path.vertex;
		if (v_path >= 0 && this->get_update_flag(v_path)) {
			in_deg[v_path] -= 1;
			if (in_deg[v_path] == 0) {
				this->dq.push_front(v_path); // prioritize same train for locality
			}
		};

		for (auto& cons : vtx_backward.constrains) {
			if (this->get_update_flag(cons.vertex)) {
				in_deg[cons.vertex] -= 1;
				if (in_deg[cons.vertex] == 0) {
					this->dq.push_back(cons.vertex);
				}
			}
		}
	}

	return true;
}

