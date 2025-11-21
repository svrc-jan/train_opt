#pragma once

#include <vector>
#include <tuple>

#include "utils/array.hpp"
#include "instance.hpp"

using namespace std;

struct Op_time
{
	int op = -1;
	int time = -1;
};


struct Res_col
{
	Op_time first = {-1, -1};
	Op_time second = {-1, -1};
};

class Graph
{
public:
	Graph(const Instance& inst);

	bool get_col(Res_col& col);

private:
	const Instance& inst;

	vector<int> path = {};
	vector<vector<Op_time>> res_cons = {};
	vector<int> n_in_res_cons = {};
};

