#pragma once

#include <vector>

#include <limits.h>
#include <random>

#include "utils.hpp"


struct Operation
{
	uint dur = 0;
	uint start_lb = 0;
	uint start_ub = -1;

	std::vector<uint> succ;
	uint n_succ = 0;
	uint fork_idx = -1;

	// std::vector<uint> pred;
	uint n_res = 0;
	std::vector<uint> res_idx;
	std::vector<uint> res_release_time;

	uint obj_id = -1;
};


struct Train
{
	uint n_ops = 0;
	uint fork_begin = -1;
	uint fork_end = -1;
};

struct Fork
{
	uint train_idx = -1;
	uint op_idx = -1;
};

struct Objective
{
	uint train_idx = -1;
	uint op_idx = -1;

	uint threshold = 0;
	uint increment = 0;
	uint coeff = 0;
};


class Instance
{
public:
	std::string name = "";

	Instance(const std::string& name, const std::string& json_file, int seed=0);
	~Instance();

	std::vector<std::vector<Operation>> ops;
	std::vector<Train> trains;
	std::vector<Fork> forks;
	std::vector<Objective> objectives;

	std::map<std::string, uint> res_idx_map;

	uint n_res = 0;
	uint n_train = 0;
	uint n_fork = 0;
	
	std::mt19937* rng;
	void init_rng(uint seed);

	const std::vector<uint>& get_fork_succ(const uint fork_idx);

	const Operation& get_op(const Fork& fork)
	{	return this->ops[fork.train_idx][fork.op_idx]; }

private:
	void parse_json(const std::string& json_file);
};
