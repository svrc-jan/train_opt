#include "base_data.hpp"

#include "utils/files.hpp"


Base_data::Base_data(const string& file_name) 
{
	json inst_jsn = get_json_file(file_name);

	this->prepare(inst_jsn);
	this->parse(inst_jsn);
	this->assign_arrays();
	this->assign_prev_ops();
}


void Base_data::prepare(json inst_jsn)
{
	int ops_size = 0;
	int trains_size = 0;
	int succ_size = 0;
	int res_size = 0;

	for (const json& train_jsn : inst_jsn["trains"]) {
		trains_size++;

		for (const json& op_jsn : train_jsn) {
			ops_size++;

			succ_size += op_jsn["successors"].size();
			if (op_jsn.contains("resources")) {
				res_size += op_jsn["resources"].size();
				for (const auto& res_jsn : op_jsn["resources"]) {
					this->add_res_name(res_jsn["resource"]);
				}
			}
		}
	}

	this->ops.reserve(ops_size);
	this->trains.reserve(trains_size);
	this->op_succ.reserve(succ_size);
	this->op_res.reserve(res_size);
}


void Base_data::parse(json inst_jsn)
{
	for (const json& train_jsn : inst_jsn["trains"]) {
		Base_train train;

		for (const json& op_jsn : train_jsn) {
			Base_op op;

			op.dur = op_jsn["min_duration"];
			
			if (op_jsn.contains("start_lb")) {
				op.start_lb = op_jsn["start_lb"];
			}

			if (op_jsn.contains("start_ub")) {
				op.start_ub = op_jsn["start_ub"];
			}

			for (int s : op_jsn["successors"]) {
				op.succ.size += 1;
				this->op_succ.push_back(s);
			}

			if (op_jsn.contains("resources")) {
				for (const auto& res_jsn : op_jsn["resources"]) {
					Base_res res;

					res.idx = this->res_name_to_idx[res_jsn["resource"]];
					if (res_jsn.contains("release_time")) {
						res.time = res_jsn["release_time"];
					}

					op.res.size += 1;
					this->op_res.push_back(res);
				}
			}

			train.ops.size += 1;
			this->ops.push_back(op);
		}

		this->trains.push_back(train);
	}
}

void Base_data::assign_arrays()
{
	int ops_idx = 0;
	int op_succ_idx = 0;
	int op_res_idx = 0;

	for (Base_train& train : this->trains) {
		train.ops.assign_ptr(this->ops, ops_idx);

		for (Base_op& op : train.ops) {
			op.succ.assign_ptr(this->op_succ, op_succ_idx);
			op.res.assign_ptr(this->op_res, op_res_idx);
		}
	}

	assert(ops_idx == this->n_ops());
	assert(op_succ_idx == this->n_op_succ());
	assert(op_res_idx == this->n_op_res());
}


void Base_data::assign_prev_ops()
{
	this->op_prev.resize(this->n_op_succ());

	for (const Base_op& op : this->ops) {
		for (int s : op.succ) {
			this->ops[s].prev.size += 1;
		}
	}

	int idx = 0;
	for (Base_op& op : this->ops) {
		op.prev.assign_ptr(this->op_prev, idx);
	}
	assert(idx == this->n_op_prev());


	vector<int> prev_filled(this->n_ops(), 0);
	for (int o = 0; o < this->n_ops(); o++) {
		for (int s : this->ops[o].succ) {
			this->ops[s].prev[prev_filled[s]++] = o;
		}
	}
}


void Base_data::add_res_name(string res_name)
{
	if (this->res_name_to_idx.find(res_name) != this->res_name_to_idx.end()) {
		this->res_name_to_idx[res_name] = this->n_res();
	}
}