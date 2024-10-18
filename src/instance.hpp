#pragma once

#include <vector>

#include <limits.h>
#include <random>

#include "utils.hpp"

struct Train
{
	uint op_begin = -1;
	uint op_end = -1;

	uint fork_begin = -1;
	uint fork_end = -1;
};

struct Objective {
	uint op_idx = -1;
	uint threshold = 0;
	uint increment = 0;
	uint coeff = 0;
};

struct Operation
{
	uint train_id = -1;
	uint op_id = -1;

	uint dur = 0;
	uint start_lb = 0;
	uint start_ub = 0;

	std::vector<uint> succ;
	uint n_succ = 0;
	uint fork_idx = -1;

	// std::vector<uint> pred;
	std::vector<uint> res;
	std::vector<uint> res_release_time;

	Objective* objective = nullptr;
};

struct Resource
{
	std::vector<uint> trains;
	uint n_trains = 0;
};

class Instance
{
public:
	std::string name = "";

	Instance(const std::string& name, const std::string& json_file, int seed=0);
	~Instance();

	std::vector<Train> trains;
	std::vector<Operation> ops;
	std::vector<Objective> objectives;
	std::vector<Resource> resources;

	std::vector<std::vector<uint>> op_idx_map;
	std::map<std::string, uint> res_idx_map;
	std::vector<uint> fork_idx_map;

	uint n_res = 0;
	uint n_train = 0;
	uint n_fork = 0;
	
	std::mt19937* rng;
	void init_rng(uint seed);

private:
	void parse_json(const std::string& json_file);
};
