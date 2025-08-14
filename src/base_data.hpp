#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "utils.hpp"
#include "array_component.hpp"

using namespace std;


struct Base_op
{
	int dur = 0;
	int start_lb = 0;
	int start_ub = 0;

	Array_entry<int> succ;
	Array_entry<int> prev;
	Array_entry<int> res;
	Array_entry<int> res_time;
};

#define S sizeof(Array_entry<int>)
struct Base_train
{
	Array_entry<Base_op> ops;
};



class Base_data
{
public:
	Base_data(const string& file_name);
	~Base_data();

	vector<Base_train> trains;

private:
	Array_component<Base_op> ac_ops;
	Array_component<int> ac_succ;
	Array_component<int> ac_prev;
	Array_component<int> ac_res;
	Array_component<int> ac_res_time;

	unordered_map<string, int> res_name_idx_map;

	string file_name;
	void parse_json(string file_name);
	void make_prev();

	int get_res_idx(string res_name);
};