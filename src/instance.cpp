#include "instance.hpp"

#include <iostream>

Instance::Instance(string file_name)
{
	this->parse_json_file(file_name);
}


Instance::~Instance()
{

}

void Instance::parse_json_file(const string file_name)
{

	json inst_jsn = get_json_file(file_name);
	
	this->v_train.reserve(inst_jsn["trains"].size());
	for (const json& train_jsn : inst_jsn["trains"]) {
		this->parse_json_train(train_jsn);
	}

	this->assign_prev_ops();
}

void Instance::parse_json_train(const json& train_jsn)
{
	Train train = {
		.idx	= (int)this->v_train.size(),
		.begin 	= (int)this->v_op.size(),
		.end 	= (int)this->v_op.size()
	};

	for (const auto& op_jsn : train_jsn) {
		this->parse_json_op(op_jsn, train);
	}

	this->v_train.push_back(train);
}

void Instance::parse_json_op(const json& op_jsn, Train& train)
{
	static vector<int> v_tmp;
	Op op = {
		.idx = (int)this->v_op.size(),
		.train = train.idx,
		.dur = op_jsn["min_duration"]
	};

	if (op_jsn.contains("start_lb")) {
		op.start_lb = op_jsn["start_lb"];
	}

	if (op_jsn.contains("start_ub")) {
		op.start_ub = op_jsn["start_ub"];
	}

	op.n_succ = op_jsn["successors"].size();
	op.v_succ.reserve(op.n_succ);
	
	for (int i_succ : op_jsn["successors"]) {
		op.v_succ.push_back(i_succ + train.begin);
	}

	sort(op.v_succ.begin(), op.v_succ.end());
	
	if (op_jsn.contains("resources")) {

		for (const auto& res_jsn : op_jsn["resources"]) {
			string res_name = res_jsn["resource"];
			int res_idx = this->get_res_idx(res_name);

			int res_time = 0;
			if (res_jsn.contains("release_time")) {
				res_time = res_jsn["release_time"];
			}

			op.v_res.push_back(res_idx);
			this->mp_res_time[{res_idx, op.idx}] = res_time;
		}
		sort(op.v_res.begin(), op.v_res.end());
	}

	this->v_op.push_back(op);
	train.end++;
}


void Instance::assign_prev_ops()
{
	for (const Op& op : this->v_op) {
		for (const int succ : op.v_succ) {
			this->v_op[succ].n_prev += 1;
		}
	}

	for (Op& op : this->v_op) {
		op.v_prev.reserve(op.n_prev);
	}

	for (const Op& op : this->v_op) {
		for (const int succ : op.v_succ) {
			Op& succ_op = this->v_op[succ];
			succ_op.v_prev.push_back(op.idx);
		}
	}	
}


int Instance::get_res_idx(string name)
{
	unordered_map<string, int>& m = this->mp_res_name_idx;

	int idx;
	auto it = m.find(name);

	if (it != m.end()) {
		idx = it->second;
	}
	else {
		idx = m.size();
		m[name] = idx;
	}

	return idx;
}

pair<vector<int>, vector<int>> Instance::_res_diff(int first, int second) const
{
	pair<vector<int>, vector<int>> result = {{}, {}};
	
	size_t i = 0;
	size_t j = 0;

	const vector<int>& first_res = this->v_op[first].v_res;
	const vector<int>& second_res = this->v_op[second].v_res;

	while (i < first_res.size() && j < second_res.size()) {
		if (first_res[i] < second_res[j]) {
			result.first.push_back(first_res[i]);
			i++;
		}

		else if (first_res[i] > second_res[j]) {
			result.second.push_back(second_res[j]);
			j++;
		}

		else {
			i++;
			j++;
		}
	}

	for (; i < first_res.size(); i++) {
		result.first.push_back(first_res[i]);
	}
	
	for (; j < second_res.size(); j++) {
		result.second.push_back(second_res[j]);
	}

	return result;
}


const pair<vector<int>, vector<int>>& Instance::res_diff(int first, int second) const
{
	unordered_map<
		pair<int, int>, 
		pair<vector<int>, vector<int>>,
		Pair_hasher
	>& cache = this->mp_res_diff_cache;

	pair<int, int> key = {first, second};
	auto it = cache.find(key);
	if (it == cache.end()) {
		auto new_it = cache.insert({key, move(this->_res_diff(first, second))});
		return new_it.first->second;
	}

	return it->second;
}

vector<Res_use> Instance::_res_uses(const vector<int>& path) const
{
	vector<Res_use> result = {};

	vector<int> locked(this->n_res(), -1);


	int from = path[0];

	

	for
}