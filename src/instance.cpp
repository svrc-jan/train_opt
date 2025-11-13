#include "instance.hpp"

#include <iostream>

Instance::Instance(string file_name)
{
	this->parse_json_file(file_name);
	this->assign_array_pointers();
	this->assign_prev_ops();
}


Instance::~Instance()
{

}

void Instance::parse_json_file(const string file_name)
{

	json inst_jsn = get_json_file(file_name);
	
	for (const json& train_jsn : inst_jsn["trains"]) {
		this->parse_json_train(train_jsn);
	}
}

void Instance::parse_json_train(const json& train_jsn)
{
	Train train = {
		.idx = this->n_trains(),
		.op_begin = this->n_ops()
	};

	for (const auto& op_jsn : train_jsn) {
		this->parse_json_op(op_jsn, train);
	}

	this->trains.push_back(train);
}

void Instance::parse_json_op(const json& op_jsn, Train& train)
{
	static vector<int> v_tmp;
	Op op = {
		.idx = this->n_ops(),
		.train = train.idx,
		.dur = op_jsn["min_duration"]
	};

	if (op_jsn.contains("start_lb")) {
		op.start_lb = op_jsn["start_lb"];
	}

	if (op_jsn.contains("start_ub")) {
		op.start_ub = op_jsn["start_ub"];
	}

	
	for (int i_succ : op_jsn["successors"]) {
		op.n_succ()++;
		this->op_succ.push_back(i_succ + train.op_begin);
	}
	
	if (op_jsn.contains("resources")) {

		for (const auto& res_jsn : op_jsn["resources"]) {
			string res_name = res_jsn["resource"];
			int res_idx = this->get_res_idx(res_name);

			int res_time = 0;
			if (res_jsn.contains("release_time")) {
				res_time = res_jsn["release_time"];
			}
			
			op.res.size++;
			this->op_res.push_back({res_idx, res_time});
		}
	}

	train.n_ops()++;
	this->ops.push_back(op);
}


void Instance::assign_array_pointers()
{
	int idx;
	
	// train -> op
	idx = 0;
	for (Train& train : this->trains) {
		if (train.n_ops() > 0) {
			train.ops.ptr = &(this->ops[idx]);
			idx += train.n_ops();
		}
	}
	assert(idx == this->n_ops());

	// op -> succ
	idx = 0;
	for (Op& op : this->ops) {
		if (op.n_succ() > 0) {
			op.succ.ptr = &(this->op_succ[idx]);
			idx += op.n_succ();
		}
		else {
			op.succ.ptr = nullptr;
		}
	}
	assert(idx == (int)this->op_succ.size());

	// op -> res
	idx = 0;
	for (Op& op : this->ops) {
		if (op.n_res() > 0) {
			op.res.ptr = &(this->op_res[idx]);
			idx += op.n_res();
		}
		else {
			op.res.ptr = nullptr;
		}
	}
	assert(idx == (int)this->op_res.size());
}


void Instance::assign_prev_ops()
{
	// increment the number of prev for each op
	for (const Op& op : this->ops) {
		for (const int s : op.succ) {
			this->ops[s].n_prev() += 1;
		}
	}

	this->op_prev.resize(this->op_succ.size());	

	// op -> prev
	int idx = 0;
	for (Op& op : this->ops) {
		if (op.n_prev() > 0) {
			op.prev.ptr = &(this->op_prev[idx]);
			idx += op.n_prev();
		}
	}
	assert(idx == (int)this->op_prev.size());

	vector<int> op_prev_filled(this->n_ops(), 0);
	for (const Op& op : this->ops) {
		for (const int s : op.succ) {
			Op& succ = this->ops[s];
			succ.prev[op_prev_filled[s]++] = op.idx;
		}
	}	
}


int Instance::get_res_idx(string name)
{
	unordered_map<string, int>& m = this->res_name_to_idx;

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