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
	int train = -1;
	
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	int level_start = -1;
	int level_end = -1;

	Array<int> succ = {nullptr, 0};
	Array<Res> res = {nullptr, 0};
};


struct Train
{
	int op_begin = -1;

	Array<Op> ops = {nullptr, 0};

	inline int n_ops() const { return this->ops.size; }
};


class Instance
{
public:
	vector<Train> trains = {};
	vector<Op> ops = {};

	int n_res = 0;

	Instance(string file_name);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_ops() const { return this->ops.size(); }
	

private:
	vector<int> op_succ = {};
	vector<Res> op_res = {};

	void add_ops(const Base_data& data);
};
