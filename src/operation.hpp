#pragma onces

#include <vector>
#include <limits>

#include "binary_vector.hpp"

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
	Binary_vector res_vec;

	Objective *obj = nullptr;

	inline bool operator==(const Operation& other)
	{
		return (this->train_id == other.train_id) && 
			(this->op_id == other.op_id);
	}
	
	inline bool operator!=(const Operation& other)
	{
		return !this->operator==(other);
	}
};