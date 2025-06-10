#include "instance.hpp"

bool Operation::operator==(const Operation& other)
{
	return (this->train_id == other.train_id) && 
		(this->op_id == other.op_id);
}

bool Operation::operator!=(const Operation& other)
{
	return !this->operator==(other);
}

vector<Operation>::iterator Train::begin()
{
	return this->inst->ops.begin() + this->begin_idx;
}

vector<Operation>::iterator Train::end()
{
	return this->inst->ops.begin() + this->end_idx;
}

Operation& Train::operator[](int idx)
{
	return this->inst->ops[this->begin_idx + idx];
}

Instance::Instance(const std::string& json_file)
	: name(json_file)
{
	this->parse_json(json_file);
}

Instance::~Instance()
{

}

void Instance::parse_json(const std::string& json_file)
{
	json td_json = get_json_file(json_file);

	this->ops.clear();
	this->res_idx_map.clear();

	// build basic operation info and resource map
	for (auto train_json : td_json["trains"]) {
		for (auto op_json : train_json) {
			Operation op;

			op.dur = op_json["min_duration"];

			if (op_json.contains("start_lb"))
				op.start_lb = op_json["start_lb"];

			if (op_json.contains("start_ub"))
				op.start_ub = op_json["start_ub"];

			this->ops.push_back(op);

			// add resource to map
			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];

					if (this->res_idx_map.count(res_name) == 0) {
						this->res_idx_map[res_name] = this->n_res++;
					}
				}
			}
		}
	}

	this->n_op = this->ops.size();
	this->n_res = this->res_idx_map.size();

	this->trains.clear();
	int op_idx = 0;
	int train_id = 0;
	
	// add succesors and resources, index trains
	for (auto train_json : td_json["trains"]) {
		Train train;
		train.inst = this;

		train.begin_idx = op_idx;
		for (auto op_json : train_json) {
			Operation& op = this->ops[op_idx];
			op.train_id = train_id;
			op.op_id = op_idx - train.begin_idx;

			for (int succ_id : op_json["successors"]) {
				op.succ.push_back(train.begin_idx + succ_id);
				op.n_succ++;

				Operation& succ = this->ops[train.begin_idx + succ_id];
				succ.prev.push_back(op_idx);
				succ.n_prev++;
			}

			// add resource to op
			op.n_res = 0;
			op.res.clear();
			// op.res_vec.resize(this->n_res);
			// std::fill(op.res_vec.begin(), op.res_vec.end(), 0);

			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];

					int res_idx = this->res_idx_map.at(res_name);
					
					int release_time = 0;
					if (res_json.contains("release_time"))
						release_time = res_json["release_time"];
					op.res.push_back({res_idx, release_time});
					// op.res_vec[res_idx] = 1;

					op.n_res++;
				}
			}
			op_idx++;
		}

		train.end_idx = op_idx;
		this->trains.push_back(train);
		train_id++;
	}

	this->n_train = this->trains.size();

	this->objectives.clear();
	for (auto objective_json : td_json["objective"]) {
		if (objective_json["type"] != "op_delay") {
			continue;
		}

		Objective obj;

		if (objective_json.contains("threshold")) {
			obj.threshold = objective_json["threshold"];
		}
		
		if (objective_json.contains("increment")) {
			obj.increment = objective_json["increment"];
		}

		if (objective_json.contains("coeff")) {
			obj.coeff = objective_json["coeff"];
		}

		this->objectives.push_back(obj);
	}
	
	int obj_idx = 0; 
	for (auto objective_json : td_json["objective"]) {
		if (objective_json["type"] != "op_delay") {
			continue;
		}

		Objective obj;

		int obj_train = objective_json["train"];
		int obj_op = objective_json["operation"];

		this->trains[obj_train][obj_op].obj = &(this->objectives[obj_idx]);

		obj_idx++;
	}	
}


