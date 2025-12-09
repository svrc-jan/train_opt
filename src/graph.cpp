#include "graph.hpp"


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

	this->dirty.resize_for_items(this->n_ver);
	this->visited.resize_for_items(this->n_ver);
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

	this->dirty.set_true(v_end);
}


bool Graph::add_edge(const Edge& edge, const bool check_ub)
{
	if (!this->find_visited(edge.vertex_from,  edge.vertex_to)) {
		return false;
	}

	this->dirty.update(this->visited);

	this->forward[edge.vertex_from].constrains.push_back(edge.vertex_to);
	this->backward[edge.vertex_from].constrains.push_back({
		.vertex = edge.vertex_to,
		.time = edge.time
	});

	return true;
}

bool Graph::remove_edge(const Edge& edge)
{
	auto& forward_cons = this->forward[edge.vertex_from].constrains;
	auto& backward_cons = this->backward[edge.vertex_from].constrains;

	int forward_idx = -1;
	for (int i = (int)forward.size() - 1; i >= 0; i--) {
		if (forward_cons[i] == edge.vertex_to) {
			forward_idx = i;
			break;
		}
	}

	int backward_idx = -1;
	for (int i = (int)backward.size() - 1; i >= 0; i--) {
		if (backward_cons[i].vertex == edge.vertex_from 
			&& backward_cons[i].time == edge.time) {
			
			backward_idx = i;
			break;
		}
	}

	if (forward_idx < 0 || backward_idx < 0) {
		return false;
	}

	this->mark_dirty(edge.vertex_from);

	return true;
};


bool Graph::find_visited(const int v_from, const int v_cycle)
{
	this->visited.clear();

	this->visited.set_true(v_from);
	this->dq.push_back(v_from);

	while (!this->dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_front();

		const auto& vtx_forward = this->forward[v_curr];

		int v_path = vtx_forward.path;
		if (v_path >= 0) {
			if (v_path == v_cycle) {
				return false;
			}

			if (!this->visited.get(v_path)) {
				this->visited.set_true(v_path);
				this->dq.push_back(v_path);
			}
		}

		for (int v_cons : vtx_forward.constrains) {
			if (v_cons == v_cycle) {
				return true;
			}

			if (!this->visited.get(v_path)) {
				this->visited.set_true(v_path);
				this->dq.push_front(v_path); // prioritize resource branching to find cycles
			}
		}
	}
}


void Graph::mark_dirty(const int v_from)
{
	this->dirty.set_true(v_from);
	this->dq.push_back(v_from);

	while (!this->dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_front();

		const auto& vtx_forward = this->forward[v_curr];

		int v_path = vtx_forward.path;
		if (v_path >= 0) {
			if (!this->dirty.get(v_path)) {
				this->dirty.set_true(v_path);
				this->dq.push_front(v_path);
			}
		}

		for (int v_cons : vtx_forward.constrains) {
			if (!this->dirty.get(v_cons)) {
				this->dirty.set_true(v_cons);
				this->dq.push_back(v_cons);
			}
		}
	}
}


void Graph::update_time(const int v_from)
{
	// backward pass to find all dirty predecessors
	this->visited.clear();
	this->need_update.clear();

	this->visited.set_true(v_from);
	this->need_update.push_back(v_from);
	this->dq.push_back(v_from);

	while (!this->dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_front();

		const auto& vtx_backward = this->backward[v_curr];

		int v_path = vtx_backward.path.vertex;
		if (v_path >= 0) {
			if (!this->visited.get(v_path) && this->dirty.get(v_path)) {
				this->visited.set_true(v_path);
				this->need_update.push_back(v_path);
				this->dq.push_front(v_path);
			}
		}

		for (const auto& cons : vtx_backward.constrains) {
			int v_cons = cons.vertex;
			if (!this->visited.get(v_cons) && this->dirty.get(v_cons)) {
				this->visited.set_true(v_cons);
				this->need_update.push_back(v_cons);
				this->dq.push_back(v_cons);
			}
		}
	}

	sort(this->need_update.begin(), this->need_update.end());	

#ifndef NO_VLA
	uint8_t n_dep[this->n_ver];
#else
	vector<uint8_t> n_dep(this->n_ver);
#endif

	// calculate number of dependencies
	for (int v : this->need_update) {
		const auto& vtx_backward = this->backward[v];

		int vtx_n_dep = 0;

		int v_path = vtx_backward.path.vertex;
		if (v_path >= 0) {
			if (this->visited.get(v_path)) {
				vtx_n_dep += 1;
			}
		}

		for (const auto& cons : vtx_backward.constrains) {
			int v_cons = cons.vertex;
			if (this->visited.get(v_cons)) {
				vtx_n_dep += 1;
			}
		}

		assert(vtx_n_dep <= UINT8_MAX);
		if (n_dep == 0) {
			this->dq.push_back(v);
		}
		else {
			n_dep[v] = vtx_n_dep;
		}
	}

	// forward pass to update times from previous
	while (!this->dq.empty()) {
		int v_curr = this->dq.front();
		this->dq.pop_front();

		const auto& vtx_forward = this->forward[v_curr];
		const auto& vtx_backward = this->backward[v_curr];
		const auto& vtx_time_bounds = this->time_bounds[v_curr];

		// backward time updates
		this->time[v_curr] = vtx_time_bounds.lower;

		auto& path = vtx_backward.path;
		if (path.vertex >= 0) {
			this->time[v_curr] = max(this->time[v_curr], 
				this->time[path.vertex] + path.time);
		}

		for (const auto& cons : vtx_backward.constrains) {
			this->time[v_curr] = max(this->time[v_curr], 
				this->time[cons.vertex] + cons.time);
		}

		this->dirty.set_false(v_curr);

		// forward enque necessary
		int v_path = vtx_forward.path;
		if (v_path >= 0) {
			if (this->visited.get(v_path)) {
				n_dep[v_path] -= 1;
				if (n_dep[v_path]) {
					this->dq.push_front(v_path); // prioritize path for locallity
				} 
			}
		}

		for (int v_cons: vtx_forward.constrains) {
			if (this->visited.get(v_cons)) {
				n_dep[v_cons] -= 1;
				if (n_dep[v_cons]) {
					this->dq.push_back(v_cons);
				} 
			}
		}
	}
	
	// check if the target node is updated
	assert(!this->dirty.get(v_from));
}
