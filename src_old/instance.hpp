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

	Operation& get_op(const int idx);
	Operation& operator[](const int idx) { return this->get_op(idx); }
	
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
	bin_vec::block_t* start_res_vec;

	int n_op = 0; 
	int n_res = 0;
	int n_train = 0;

	int get_res_overlap(const int a, const int b) const;

private:
	void parse_json(const std::string& json_file);
	void make_res_bin_vec();

	mutable std::map<std::pair<int, int>, int> res_overlap_cache;
};
