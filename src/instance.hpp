#pragma once

#include <vector>
#include <limits>

#include "utils.hpp"
#include "operation.hpp"
#include "bin_vec.hpp"

using std::vector;

class Instance;

struct Train
{
	Instance *inst = nullptr;
	int begin_idx = -1;
	int end_idx = -1;

	vector<Operation>::iterator begin();
	vector<Operation>::iterator end();
	inline Operation& operator[](int idx);
};

class Instance
{
public:
	std::string name = "";

	Instance(const std::string& json_file);

	~Instance();

	vector<Operation> ops;
	vector<Train> trains;
	vector<Objective> objectives;
	std::unordered_map<std::string, uint> res_idx_map;
	vector<bin_vec::block_t> res_bin_vec;

	int n_op = 0; 
	int n_res = 0;
	int n_train = 0;

private:
	void parse_json(const std::string& json_file);
	void make_res_bin_vec();
};
