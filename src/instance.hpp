#pragma once

#include <vector>
#include <limits>

#include "utils.hpp"

using std::vector;

struct Objective
{
	int threshold = 0;
	int increment = 0;
	int coeff = 0;
};

struct Res
{
	int id = -1;
	int time = 0;
};

struct Operation
{
	int train_id = -1;
	int op_id = -1;

	int dur = 0;
	int start_lb = 0;
	int start_ub = std::numeric_limits<typeof(start_ub)>::max()/4;

	int n_succ = 0;
	vector<int> succ;

	int n_prev = 0;
	vector<int> prev;

	int n_res = 0;
	vector<Res> res;
	// vector<int> res_vec;

	Objective *obj = nullptr;

	inline bool operator==(const Operation& other);
	inline bool operator!=(const Operation& other);
};

class Instance;

struct Train
{
	Instance *inst = nullptr;
	int begin_idx = -1;
	int end_idx = -1;

	vector<Operation>::iterator begin();
	vector<Operation>::iterator end();
	inline Operation& operator[](int idx);
};

class Instance
{
public:
	std::string name = "";

	Instance(const std::string& json_file);

	~Instance();

	vector<Operation> ops;
	vector<Train> trains;
	vector<Objective> objectives;
	std::unordered_map<std::string, uint> res_idx_map;

	int n_op = 0; 
	int n_res = 0;
	int n_train = 0;

private:
	void parse_json(const std::string& json_file);
};
