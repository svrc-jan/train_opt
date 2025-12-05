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

struct Obj
{
	int threshold = 0;
	int coeff = 0;
	int increment = 0;
};


struct Op
{
	int train = -1;
	
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	Obj* obj = nullptr;

	Array<int> succ = {nullptr, 0};
	Array<int> pred = {nullptr, 0};
	Array<Res> res = {nullptr, 0};
};


struct Train
{
	int op_start = -1;
	Array<Op> ops = {nullptr, 0};

	inline int op_last() const { return this->op_start + this->ops.size - 1; }
	inline int op_end() const { return this->op_start + this->ops.size; }
};


class Instance
{
public:
	int max_n_train_ops = 0;

	vector<Train> trains = {};
	vector<Op> ops = {};
	vector<Obj> objs = {};

	Instance(const string& file_name);

	inline int n_trains() const { return this->trains.size(); }
	inline int n_ops() const { return this->ops.size(); }
	inline int n_res() const { return this->res_name_to_idx.size(); }
	inline int n_op_succ() const { return this->op_succ.size(); }
	inline int n_op_pred() const { return this->op_pred.size(); }
	inline int n_op_res() const { return this->op_res.size(); }

private:
	vector<int> op_succ = {};
	vector<int> op_pred = {};
	vector<Res> op_res = {};
	vector<int> train_topo_order = {};
	map<string, int> res_name_to_idx = {};

	void prepare(json inst_jsn);
	void parse(json inst_jsn);
	void assign_arrays();
	void assign_pred_ops();
	void propagate_bounds();

	void add_res_name(string res_name);
};
