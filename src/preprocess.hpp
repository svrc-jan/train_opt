#pragma once

#include "utils/array.hpp"
#include "instance.hpp"

struct Level
{
	bool is_req = true;

	int time_lb = 0;
	int time_ub = INT_MAX;

	Array<int> ops_in = {nullptr, 0};
	Array<int> ops_out = {nullptr, 0};
};

struct PP_train
{
	int level_start = -1;
	Array<Level> levels = {nullptr, 0};

	inline int level_last() const { return this->level_start + this->levels.size - 1; }
	inline int level_end() const { return this->level_start + this->levels.size; }
};


class Preprocess
{
public:
	vector<PP_train> trains = {};
	vector<Level> levels = {};

	vector<pair<int, int>> op_level = {};
	
	Preprocess(const Instance& inst);
	void make_levels();
	void make_level_bounds();
	

	bool is_routing_level(int l);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_levels() const { return this->levels.size(); }

private:
	const Instance& inst;

	vector<int> level_ops_out = {};
	vector<int> level_ops_in = {};
};
