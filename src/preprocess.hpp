#pragma once

#include "utils/array.hpp"
#include "instance.hpp"

class Preprocess
{
public:
	struct Level;
	struct Train;

	const Instance& inst;

	std::vector<Train> trains = {};
	std::vector<Level> levels = {};

	std::vector<std::pair<int, int>> op_level = {};
	
	Preprocess(const Instance& inst);
	void make_levels();
	void make_level_bounds();
	

	bool is_routing_level(int l);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_levels() const { return this->levels.size(); }

	inline int op_level_start(int op) const { return this->op_level[op].first; }
	inline int op_level_end(int op) const { return this->op_level[op].second; }

private:

	std::vector<int> level_ops_out = {};
	std::vector<int> level_ops_in = {};
};


struct Preprocess::Level
{
	int train = -1;
	bool is_req = true;

	int time_lb = 0;
	int time_ub = INT_MAX;

	Array<int> ops_in = {nullptr, 0};
	Array<int> ops_out = {nullptr, 0};
};


struct Preprocess::Train
{
	int level_start = -1;
	Array<Level> levels = {nullptr, 0};

	inline int level_last() const { return this->level_start + this->levels.size - 1; }
	inline int level_end() const { return this->level_start + this->levels.size; }
};
