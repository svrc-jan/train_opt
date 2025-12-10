#pragma once

#include <vector>
#include <queue>
#include <algorithm>

#include "time_type.hpp"
#include "graph.hpp"


class Schedule
{
public:
	using Res_edges = std::pair<Graph::Edge, Graph::Edge>;
	struct Res_use;
	struct Res_interval;

	Schedule(Graph& graph);

	void set_all_paths(const std::vector<std::vector<int>>& paths);
	void set_path(const int train_idx, const std::vector<int>& path);
	bool process_from_start(Res_edges& res_edges);


private:
	struct Event;

	const Instance& inst;
	const Preprocess& prepr;
	Graph& graph;

	int n_trains = 0;
	int n_res = 0;

	std::vector<std::vector<int>> paths = {};
	std::vector<std::vector<Res_use>> train_res_uses = {};
	std::vector<int> train_ru_idx = {};
	std::vector<std::vector<Res_interval>> res_intervals = {};

	// aux
	std::vector<Res_interval> prio_queue;

	void update_res_inteval(Res_interval& ri);
	void make_res_edges(Res_edges& res_edges, const Res_use& a, const Res_use& b);
	inline const Res_use& get_res_use(const Res_interval& ri) const;
};


struct Schedule::Res_use
{
	int res_idx = -1;
	int level_lock = -1;
	int level_unlock = -1;
	TIME_T res_time = 0;

	inline bool operator<(const Res_use& other) const;
};


struct Schedule::Res_interval
{
	int train = -1;
	int res_use_idx = -1;
	TIME_T time_lock = 0;
	TIME_T time_unlock = 0;

	inline bool operator<(const Res_interval& other) const;
};


inline const Schedule::Res_use& Schedule::get_res_use(const Res_interval& ri) const
{
	return this->train_res_uses[ri.train][ri.res_use_idx];
}


bool Schedule::Res_use::operator<(const Res_use& other) const
{
	if (this->level_lock == other.level_lock) {
		return this->level_unlock > this->level_unlock;
	}
	return this->level_unlock < this->level_unlock;
}


bool Schedule::Res_interval::operator<(const Res_interval& other) const
{
	if (this->time_lock == other.time_lock) {
		return this->time_unlock < other.time_unlock;
	}
	return this->time_lock > other.time_lock;
}


