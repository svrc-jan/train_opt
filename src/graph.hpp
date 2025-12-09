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
	struct Time_change;

	const Instance& inst;
	const Preprocess& prepr;

	Graph(const Preprocess& prepr);

	void set_num_vertices(const int n_vtx);
	void clear_constrains();

	void set_all_paths(const std::vector<std::vector<int>>& paths);
	void set_path(const std::vector<int>& path);

	void set_path_op(const int op);
	bool add_edge(const Edge& edge, const bool check_ub=true);
	bool remove_last_edge(const Edge& edge);
	inline void restore_time_changes(const std::vector<Time_change> &changes);

	inline const std::vector<TIME_T>& get_time() const
	{ return this->time; }

	inline const std::vector<Time_change>& get_time_changes() const
	{ return this->time_changes; }


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

	std::vector<Time_change> time_changes = {};

	bool find_updates(const int v_from, const int v_cycle);
	bool update_time(const bool check_ub);

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

struct Graph::Time_change
{
	int vertex = -1;
	TIME_T old_value = 0;
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


inline void Graph::restore_time_changes(const std::vector<Time_change> &changes)
{
	for (auto& change : this->time_changes) {
		this->time[change.vertex] = change.old_value;
	}
}