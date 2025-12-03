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

private:
	const Instance& inst;
	vector<ClpSimplex*> train_lps = {};

	bool make_train_lp(int t);
};
