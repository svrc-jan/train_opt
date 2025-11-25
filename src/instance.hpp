#pragma once

#include <limits>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

#include "utils/array.hpp"

using namespace std;
using json = nlohmann::json;

#define MAX_INT numeric_limits<int>::max()


struct Res
{
	int idx = -1;
	int time = 0;

	bool operator<(const Res& other) const { return this->idx < other.idx; }
	bool operator==(const Res& other) const { return this->idx == other.idx; }

	bool operator<(int other) const { return this->idx < other; }
	bool operator==(int other) const { return this->idx == other; }
};



struct Op
{
	int train = -1;
	
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	Array<int> succ = {nullptr, 0};
	Array<int> prev = {nullptr, 0};
	Array<Res> res = {nullptr, 0};
};

struct Train
{
	int op_begin = -1;
	Array<Op> ops = {nullptr, 0};
};


class Instance
{
public:
	vector<Train> trains = {};
	vector<Op> ops = {};

	Instance(const string& file_name);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_ops() const { return this->ops.size(); }
	inline int n_res() const { return this->res_name_to_idx.size(); }
	inline int n_op_succ() const { return this->op_succ.size(); }
	inline int n_op_prev() const { return this->op_prev.size(); }
	inline int n_op_res() const { return this->op_res.size(); }

private:
	vector<int> op_succ = {};
	vector<int> op_prev = {};
	vector<Res> op_res = {};
	map<string, int> res_name_to_idx = {};

	void prepare(json inst_jsn);
	void parse(json inst_jsn);
	void assign_arrays();
	void assign_prev_ops();

	void add_res_name(string res_name);
};
