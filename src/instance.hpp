#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <limits>
#include <string>

#include "utils.hpp"

using namespace std;

#define MAX_INT numeric_limits<int>::max()

struct Res
{
	int idx = -1;
	int time = 0;
};

inline bool operator<(const Res& a, const Res& b)
{ return a.idx < b.idx; }

struct Op
{
	int idx = -1;
	int train = -1;
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	int n_succ = 0;
	int n_prev = 0;

	vector<int> v_prev = {};
	vector<int> v_succ = {};

	vector<int> v_res = {};
};


struct Train
{
	int idx = -1;
	int begin = -1;
	int end = -1;
};

struct Res_use
{
	int lock = -1;
	int unlock = -1;
	int time = 0;
};

class Instance
{
public:
	vector<Train> v_train = {};
	vector<Op> v_op = {};

	Instance(string file_name);
	~Instance();

	inline int res_time(int res, int op) const
	{ return this->mp_res_time.find({res, op})->second; }

	inline int n_res() const
	{ return this->mp_res_name_idx.size(); }

	const pair<vector<int>, vector<int>>& res_diff(int first, int second) const;
	const vector<Res_use>& res_uses(const vector<int>& path) const;


private:
	unordered_map<string, int> mp_res_name_idx;
	unordered_map<
		pair<int, int>,
		int,
		Pair_hasher
	> mp_res_time = {};
	
	mutable unordered_map<
		pair<int, int>, 
		pair<vector<int>, vector<int>>,
		Pair_hasher
	> mp_res_diff_cache;

	mutable unordered_map<
		vector<int>, 
		vector<Res_use>,
		Vector_hasher
	> mp_res_uses_cache;
	
	void parse_json_file(const string file_name);
	void parse_json_train(const json& train_jsn);
	void parse_json_op(const json& op_jsn, Train& train);

	void assign_prev_ops();
	pair<vector<int>, vector<int>> _res_diff(int first, int second) const;
	vector<Res_use> _res_uses(const vector<int>& path) const;

	int get_res_idx(string name);
};
