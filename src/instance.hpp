#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <limits>
#include <string>

#include "utils/array.hpp"
#include "utils/files.hpp"


using namespace std;

#define MAX_INT numeric_limits<int>::max()


struct Res
{
	int idx = -1;
	int time = 0;
};

struct Op
{
	int idx = -1;
	int train = -1;
	
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	Array<int> succ = {nullptr, 0};
	Array<int> prev = {nullptr, 0};
	Array<Res> res = {nullptr, 0};

	inline int& n_succ() { return this->succ.size; }
	inline int& n_prev() { return this->prev.size; }
	inline int& n_res() { return this->res.size; }

	inline int n_succ() const { return this->succ.size; }
	inline int n_prev() const { return this->prev.size; }
	inline int n_res() const { return this->res.size; }
};


struct Train
{
	int idx = -1;
	int op_begin = -1;

	Array<Op> ops = {nullptr, 0};

	inline int& n_ops() { return this->ops.size; }
	inline int n_ops() const { return this->ops.size; }
};


class Instance
{
public:
	vector<Op> ops = {};
	vector<Train> trains = {};
	unordered_map<string, int> res_name_to_idx = {};
	
	Instance(string file_name);
	~Instance();

	inline int n_ops() const { return this->ops.size(); }
	inline int n_trains() const { return this->trains.size(); }
	inline int n_res() const { return this->res_name_to_idx.size(); }

private:
	vector<int> op_succ = {};
	vector<int> op_prev = {};
	vector<Res> op_res = {};

	void parse_json_file(const string file_name);
	void parse_json_train(const json& train_jsn);
	void parse_json_op(const json& op_jsn, Train& train);

	void assign_array_pointers();
	void assign_prev_ops();

	int get_res_idx(string name);
};
