#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "utils/array.hpp"
#include "base_data.hpp"

using namespace std;

struct Res
{
	int idx = -1;
	int time = 0;
};


struct Op
{
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	int level_start = -1;
	int level_end = -1;

	Array<Res> res = {nullptr, 0};
};


struct Level
{
	int train = -1;
	
	Array<int> ops_in = {nullptr, 0};
	Array<int> ops_out = {nullptr, 0};
};


struct Train
{
	int op_begin = -1;
	int level_begin = -1;

	Array<Op> ops = {nullptr, 0};
	Array<Level> levels = {nullptr, 0};

	inline int n_ops() const { return this->ops.size; }
	inline int n_levels() const { return this->levels.size; }

	inline int level_last() const { return this->level_begin + this->n_levels() - 1; }
};


class Instance
{
public:
	vector<Train> trains = {};
	vector<Op> ops = {};
	vector<Level> levels = {};

	int n_res = 0;

	Instance(string file_name);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_levels() const { return this->levels.size(); }
	inline int n_ops() const { return this->ops.size(); }
	

private:
	vector<Res> op_res = {};
	vector<int> level_ops_in = {};
	vector<int> level_ops_out = {};

	void add_ops(const Base_data& data);
	void add_levels(const Base_data& data);
};


std::ostream& operator<<(std::ostream& stream, const Op& op);
std::ostream& operator<<(std::ostream& stream, const Level& level);
