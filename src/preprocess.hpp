#pragma once

#include "utils/array.hpp"
#include "instance.hpp"

struct Level
{
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

	vector<int> op_level_start = {};
	vector<int> op_level_end = {};
	
	Preprocess(const Instance& inst);
	void make_levels();

	inline int n_trains() const { return this->trains.size(); }
	inline int n_levels() const { return this->levels.size(); }

private:
	const Instance& inst;

	vector<int> level_ops_out = {};
	vector<int> level_ops_in = {};
	
};
