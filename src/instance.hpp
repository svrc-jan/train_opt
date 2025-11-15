#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <limits>
#include <string>
#include <cstdint>

#include "utils/array.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

#define MAX_INT numeric_limits<int>::max()


struct Res
{
	int idx = -1;
	int time = 0;

	inline bool operator<(const Res& other) const { return this->idx < other.idx; }
	inline bool operator>(const Res& other) const { return this->idx > other.idx; }
	inline bool operator==(const Res& other) const { return this->idx == other.idx; }
};


struct Level;
struct Train;

struct Op
{
	int idx = -1;
	Train* train = nullptr;
	
	int dur = 0;
	int start_lb = 0;
	int start_ub = MAX_INT;

	Level* level_start = nullptr;
	Level* level_end = nullptr;

	Array<Op*> succ = {nullptr, 0};
	Array<Op*> prev = {nullptr, 0};
	Array<Res> res = {nullptr, 0};

	inline int& n_succ() { return this->succ.size; }
	inline int& n_prev() { return this->prev.size; }
	inline int& n_res() { return this->res.size; }

	inline int n_succ() const { return this->succ.size; }
	inline int n_prev() const { return this->prev.size; }
	inline int n_res() const { return this->res.size; }
};


struct Level
{
	int idx = -1;
	Train* train = nullptr;

	int dur_to_end = MAX_INT;

	Array<Op *> ops_in = {nullptr, 0};
	Array<Op *> ops_out = {nullptr, 0};

	uint8_t* allowed = nullptr;

	inline int& n_ops_in() { return this->ops_in.size; }
	inline int& n_ops_out() { return this->ops_out.size; }

	inline int n_ops_in() const { return this->ops_in.size; }
	inline int n_ops_out() const { return this->ops_out.size; }

	bool is_allowed(Op* op_in, Op* op_out) const;
};


struct Train
{
	int idx = -1;
	int op_begin = -1;
	int level_begin = -1;

	Array<Op> ops = {nullptr, 0};
	Array<Level> levels = {nullptr, 0};

	inline int& n_ops() { return this->ops.size; }
	inline int& n_levels() { return this->levels.size; }

	inline int n_ops() const { return this->ops.size; }
	inline int n_levels() const { return this->levels.size; }
};


class Instance
{
public:
	vector<Op> ops = {};
	vector<Level> levels = {};
	vector<Train> trains = {};
	unordered_map<string, int> res_name_to_idx = {};

	Instance(string file_name);
	~Instance();

	inline int n_ops() const { return this->ops.size(); }
	inline int n_trains() const { return this->trains.size(); }
	inline int n_levels() const { return this->levels.size(); }
	inline int n_res() const { return this->res_name_to_idx.size(); }

private:
	vector<Op *> op_succ = {};
	vector<Op *> op_prev = {};
	vector<Res> op_res = {};

	vector<Op *> level_ops_in = {};
	vector<Op *> level_ops_out = {};

	vector<uint8_t> level_allowed = {};

	void parse_json_file(const string file_name);
	void parse_json_train(const json& train_jsn);
	void parse_json_op(const json& op_jsn, Train& train);

	void reserve_mem(const json& inst_jsn);

	void assign_pointers();
	void assign_prev_ops();
	void sort_arrays();
	void assign_levels();

	void calculate_dist_to_end();

	int get_res_idx(string name);
};

std::ostream& operator<<(std::ostream& stream, const Op& op);
std::ostream& operator<<(std::ostream& stream, const Op& op);
std::ostream& operator<<(std::ostream& stream, const Level& level);
