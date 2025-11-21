#pragma once

#include "instance.hpp"

struct Res_col
{
	struct {
		int train = -1;
		int start = -1;
		int end = -1;
		int res_time = -1;
	} first;

	struct {
		int train = -1;
		int start = -1;
		int end = -1;
		int res_time = -1;
	} second;
};

class Tree
{
public:
	Tree(const Instance& inst);

	bool solve();
	void make_op_out(vector<int>& op_out, const vector<int>& op_in);
	bool get_res_col(Res_col& result, const vector<int>& order, const vector<int>& time,
	const vector<int>& op_in, const vector<int>& op_out);


private:
	const Instance& inst;
};