#pragma once

#include <cstdint>
#include <vector>
#include <deque>

#include "time_type.hpp"
#include "preprocess.hpp"
#include "instance.hpp"

class Graph
{
public:
	struct Edge;

	const Instance& inst;
	const Preprocess& prepr;

	Graph(const Preprocess& prepr);

	void set_num_vertices(const int n_vtx);
	void clear_constrains();

	void set_path(const std::vector<int>& path);
	void set_path_op(const int op);

	bool add_edge(const Edge& edge, const bool check_ub=true);
	bool find_updates(const int v_from, const int v_cycle);
	bool update_time(const bool check_ub);
	inline void restore_old_time();


private:
	struct Constraint;
	struct Vertex_forward;
	struct Vertex_backward;
	struct Vertex_time_bounds;

	int n_ver = 0;

	std::vector<TIME_T> time = {};
	std::vector<Vertex_forward> forward = {};
	std::vector<Vertex_backward> backward = {};
	std::vector<Vertex_time_bounds> time_bounds = {};

	// aux
	std::deque<int> dq;

	std::vector<int> to_update = {};
	std::vector<int> update_order = {};

	std::vector<uint64_t> update_flag;
	inline void set_update_flag(int vertex);
	inline bool get_update_flag(int vertex);
	inline void clear_update_flag();

	std::vector<std::pair<int, TIME_T>> time_changes = {};
};


struct Graph::Edge
{
	int vertex_from = -1;
	int vertex_to = -1;
	TIME_T time = 0;
};


struct Graph::Constraint
{
	int vertex = -1;
	TIME_T time = 0;
};


struct Graph::Vertex_forward
{
	int path = -1;
	std::vector<int> constrains = {};
};


struct Graph::Vertex_backward
{
	Constraint path = {-1, 0};
	std::vector<Constraint> constrains = {};
};

struct Graph::Vertex_time_bounds
{
	int lower = 0;
	int upper = INT_MAX;
};


inline void Graph::set_update_flag(int vertex)
{
	this->update_flag[vertex/64] |= (1 << (vertex % 64));
}


inline bool Graph::get_update_flag(int vertex)
{
	return (this->update_flag[vertex/64] & (1 << (vertex % 64))) != 0;
}


inline void Graph::clear_update_flag()
{
	memset(this->update_flag.data(), 0, 
		sizeof(uint64_t)*this->update_flag.size());
}


inline void Graph::restore_old_time()
{
	for (auto& change : this->time_changes) {
		this->time[change.first] = change.second;
	}
}