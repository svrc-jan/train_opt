#include "instance.hpp"

#include <set>
#include "utils/files.hpp"


Instance::Instance(const string& file_name) 
{
	json inst_jsn = get_json_file(file_name);

	this->prepare(inst_jsn);
	this->parse(inst_jsn);
	this->assign_arrays();
	this->assign_prev_ops();
}


void Instance::prepare(json inst_jsn)
{
	int ops_size = 0;
	int trains_size = 0;
	int succ_size = 0;
	int res_size = 0;
	int objs_size = 0;

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

	objs_size = inst_jsn["objective"].size();


	this->ops.reserve(ops_size);
	this->trains.reserve(trains_size);
	this->op_succ.reserve(succ_size);
	this->op_res.reserve(res_size);
	this->objs.reserve(objs_size);
}


void Instance::parse(json inst_jsn)
{
	set<int> res_set;
	int n_res_duplicates = 0;

	for (const json& train_jsn : inst_jsn["trains"]) {
		Train train;
		train.op_start = this->n_ops();

		for (const json& op_jsn : train_jsn) {
			Op op;
			
			op.train = this->n_trains();

			op.dur = op_jsn["min_duration"];
			
			if (op_jsn.contains("start_lb")) {
				op.start_lb = op_jsn["start_lb"];
			}

			if (op_jsn.contains("start_ub")) {
				op.start_ub = op_jsn["start_ub"];
			}

			for (int s : op_jsn["successors"]) {
				op.succ.size += 1;
				this->op_succ.push_back(s + train.op_start);
			}

			if (op_jsn.contains("resources")) {
				res_set.clear();

				for (const auto& res_jsn : op_jsn["resources"]) {
					Res res;

					res.idx = this->res_name_to_idx[res_jsn["resource"]];
					if (res_jsn.contains("release_time")) {
						res.time = res_jsn["release_time"];
					}

					if (res_set.find(res.idx) != res_set.end()) {
						n_res_duplicates += 1;
						continue;
					}
					res_set.insert(res.idx);

					op.res.size += 1;
					this->op_res.push_back(res);
				}
			}

			train.ops.size += 1;
			this->ops.push_back(op);
		}

		this->trains.push_back(train);
	}

	this->op_res.shrink_to_fit();

	vector<int> op_obj_idx(this->n_ops(), -1);

	for (const json& obj_jsn : inst_jsn["objective"]) {
		assert(obj_jsn["type"] == "op_delay");

		Obj obj;
		if (obj_jsn.contains("threshold")) {
			obj.threshold = obj_jsn["threshold"];
		}

		if (obj_jsn.contains("coeff")) {
			obj.coeff = obj_jsn["coeff"];
		}

		if (obj_jsn.contains("increment")) {
			obj.increment = obj_jsn["increment"];
		}

		if (obj.coeff == 0 && obj.increment == 0) {
			continue;
		}


		int train_i = obj_jsn["train"];
		int op_i = obj_jsn["operation"];

		int op_idx = this->trains[train_i].op_start + op_i;

		op_obj_idx[op_idx] = this->objs.size();
		this->objs.push_back(obj);
	}

	for (int o = 0; o < this->n_ops(); o++) {
		if (op_obj_idx[o] >= 0) {
			this->ops[o].obj = &(this->objs[op_obj_idx[o]]);
		}
	}

}

void Instance::assign_arrays()
{
	int ops_idx = 0;
	int op_succ_idx = 0;
	int op_res_idx = 0;

	for (Train& train : this->trains) {
		assert(train.op_start == ops_idx);
		train.ops.assign_ptr(this->ops, ops_idx);

		for (Op& op : train.ops) {
			op.succ.assign_ptr(this->op_succ, op_succ_idx);
			op.res.assign_ptr(this->op_res, op_res_idx);
		}
	}

	assert(ops_idx == this->n_ops());
	assert(op_succ_idx == this->n_op_succ());
	assert(op_res_idx == this->n_op_res());
}


void Instance::assign_prev_ops()
{
	this->op_prev.resize(this->n_op_succ());

	for (const Op& op : this->ops) {
		for (int s : op.succ) {
			this->ops[s].prev.size += 1;
		}
	}

	int idx = 0;
	for (Op& op : this->ops) {
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



void Instance::add_res_name(string res_name)
{
	if (this->res_name_to_idx.find(res_name) == this->res_name_to_idx.end()) {
		this->res_name_to_idx[res_name] = this->n_res();
	}
}