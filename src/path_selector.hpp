#pragma once

#include <vector>
#include "coin/ClpSimplex.hpp"
#include "instance.hpp"

using namespace std;

class Path_selector
{
public:
	Path_selector(const Instance& inst);
	~Path_selector();

	bool make_train_lp(int t);
	bool set_train_lp_obj(int t, const vector<double>& op_costs);

private:
	const Instance& inst;
	vector<ClpSimplex*> train_lps = {};

	
};
