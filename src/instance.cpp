#include "instance.hpp"

Instance::Instance(const std::string& name, const std::string& json_file, int seed)
	: name(name)
{
	this->parse_json(json_file);
	this->init_rng(seed);

	int n = 0;
	int k = 0;
}

Instance::~Instance()
{
	delete this->rng;
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
					if (this->res_idx_map.find(res_name) == this->res_idx_map.end()) {
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
	
	// add succesors and resources, index trains
	for (auto train_json : td_json["trains"]) {
		Train train;
		train.inst = this;

		train.begin_idx = op_idx;
		for (auto op_json : train_json) {
			Operation& op = this->ops[op_idx];

			for (int succ_id : op_json["successors"]) {
				op.succ.push_back(train.begin_idx + succ_id);
				op.n_succ++;
			}

			// add resource to op
			op.n_res = 0;
			op.res_idx.clear();
			op.res_release_time.clear();
			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];

					int res_idx = this->res_idx_map.at(res_name);
					
					op.res_idx.push_back(res_idx);
					op.res_mask |= 1U << res_idx;

					int release_time = 0;
					if (res_json.contains("release_time"))
						release_time = res_json["release_time"];
					op.res_release_time.push_back(release_time);

					op.n_res++;
				}
			}
			op_idx++;
		}

		train.end_idx = op_idx;
		this->trains.push_back(train);
	}

	this->n_train = this->trains.size();

	this->objectives.clear();
	for (auto objective_json : td_json["objective"]) {
		if (objective_json["type"] != "op_delay") {
			continue;
		}

		Objective obj;

		int obj_train = objective_json["train"];
		int obj_op = objective_json["operation"];

		if (objective_json.contains("threshold")) {
			obj.threshold = objective_json["threshold"];
		}
		
		if (objective_json.contains("increment")) {
			obj.increment = objective_json["increment"];
		}

		if (objective_json.contains("coeff")) {
			obj.coeff = objective_json["coeff"];
		}
		assert(obj_train < this->n_train);

		this->objectives.push_back(obj);
	}

}

void Instance::init_rng(uint seed)
{
	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	this->rng = new std::mt19937(seed);
}


