#include "instance.hpp"

Instance::Instance(const std::string& name, const std::string& json_file, int seed)
	: name(name)
{
	this->parse_json(json_file);
	this->init_rng(seed);

	uint n = 0;
	uint k = 0;
	// for (uint i = 0; i < this->n_res; i++) {
	// 	Resource& res = this->resources[i];
	// 	std::cout << "res " << i << " : trains " << res.trains << std::endl;
	// }

}

Instance::~Instance()
{
	delete this->rng;
}

void Instance::parse_json(const std::string& json_file)
{
	json td_json = get_json_file(json_file);

	this->ops.clear();
	this->trains.clear();

	this->n_res = 0;
	this->n_train = 0;
	
	uint train_id = 0;

	// build basic operation info and resource map
	for (auto train_json : td_json["trains"]) {
		Train train;

		this->ops.push_back(std::vector<Operation>());

		for (auto op_json : train_json) {
			Operation op;

			op.dur = op_json["min_duration"];

			if (op_json.contains("start_lb"))
				op.start_lb = op_json["start_lb"];

			if (op_json.contains("start_ub"))
				op.start_ub = op_json["start_ub"];

			this->ops[train_id].push_back(op);
			train.n_ops++;

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

		this->trains.push_back(train);
		train_id++;
	}
	this->n_train = train_id;

	this->n_fork = 0;

	// add succesors and resources
	uint op_id;
	train_id = 0;
	for (auto train_json : td_json["trains"]) {
		Train& train = this->trains[train_id];
		train.fork_begin = this->n_fork;
		op_id = 0;
		for (auto op_json : train_json) {
			Operation& op = this->ops[train_id][op_id];

			for (uint succ_id : op_json["successors"]) {
				op.succ.push_back(succ_id);
				op.n_succ++;
				// this->ops[op_idx_map[train_id][succ_id]].pred.push_back(idx);
			}

			// set forks
			if (op.n_succ > 1) {
				Fork fork;
				fork.train_id = train_id;
				fork.op_id = op_id;

				this->forks.push_back(fork);

				op.fork_id = this->n_fork++;
			}

			// add resource to op
			op.n_res = 0;
			op.res_id.clear();
			op.res_release_time.clear();
			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];

					uint res_idx = this->res_idx_map.at(res_name);
					
					op.res_id.push_back(res_idx);
					
					uint release_time = 0;
					if (res_json.contains("release_time"))
						release_time = res_json["release_time"];
					op.res_release_time.push_back(release_time);

					op.n_res++;
				}
			}

			op_id++;
		}
		train.fork_end = this->n_fork;
		train_id++;
	}


	this->objectives.clear();
	for (auto objective_json : td_json["objective"]) {
		if (objective_json["type"] != "op_delay")
			continue;

		Objective obj;

		obj.train_id = objective_json["train"];
		obj.op_id = objective_json["operation"];

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

	for (uint obj_id = 0; obj_id < this->objectives.size(); obj_id++) {
		Objective& obj = this->objectives[obj_id];
		this->ops[obj.train_id][obj.op_id].obj_id = obj_id;
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


