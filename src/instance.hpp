#pragma once

#include <vector>

#include "limits.h"
#include "utils.hpp"

struct Train
{
	uint begin_idx = -1;
	uint end_idx = -1;

	std::vector<std::vector<uint>> paths;
};


struct Operation
{
	uint train_id = -1;
	uint op_id = -1;

	uint dur = 0;
	uint start_lb = 0;
	uint start_ub = 0;

	std::vector<uint> succ;
	std::vector<uint> pred;
	std::vector<uint> res;
	std::vector<uint> res_release_time;
};


struct Objective {
	uint op_idx = -1;
	uint threshold = 0;
	uint increment = 0;
	uint coeff = 0;
};

class Instance
{
public:
	std::string name = "";

	Instance(const std::string& name, const std::string& json_file);

private:
	std::vector<Train> trains;
	std::vector<Operation> ops;
	std::vector<Objective> objectives;

	std::vector<std::vector<uint>> op_idx_map;
	std::map<std::string, uint> res_idx_map;
	uint n_res = 0;


	void parse_json(const std::string& json_file);
	void build_paths();
	void build_path_rec(const std::vector<uint>& path, uint train_id);
};
