#pragma once

#include <vector>
#include <tuple>

#include "utils/array.hpp"
#include "utils/chrono_tracker.hpp"
#include "instance.hpp"

using namespace std;

struct Res_interval
{
	int train = -1;
	int op = -1;
	int lock = -1;
	int unlock = -1;
};


struct Res_tracker
{
	int locks_start = 0;
	int unlocks_start = 0;
	Array<Res_interval> intervals;
};


struct Node
{
	int train = -1;
	int path = -1;
	bool valid = true;

	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	Array<int> succ = {nullptr, 0};
	Array<Res> res = {nullptr, 0};
};


class Graph
{
public:
	Graph(const Instance& inst);

private:
	const Instance& inst;

	Chrono_tracker op_chrono;
	Chrono_tracker res_chrono;
	
	vector<Node> nodes;

	void make_chrono_order();
	void make_op_data();
};

