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

	this->trains.clear();
	this->ops.clear();
	this->objectives.clear();
	this->resources.clear();

	this->n_res = 0;
	this->n_train = 0;
	this->n_fork = 0;
	
	uint train_id = 0;
	uint op_id = 0;
	uint idx = 0;

	// build basic operation info and resource map
	for (auto train_json : td_json["trains"]) {
		op_id = 0;
		Train train;
		this->n_train++;
		train.op_begin = idx;

		this->op_idx_map.push_back(std::vector<uint>());

		for (auto op_json : train_json) {
			Operation op;
			op.train_id = train_id;
			op.op_id = op_id;

			op.dur = op_json["min_duration"];

			if (op_json.contains("start_lb"))
				op.start_lb = op_json["start_lb"];

			if (op_json.contains("start_ub"))
				op.start_ub = op_json["start_ub"];

			this->ops.push_back(op);
			this->op_idx_map.back().push_back(idx);

			// add resource to map
			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];
					if (this->res_idx_map.find(res_name) == this->res_idx_map.end()) {
						Resource res;
						res.trains.clear();
						res.n_trains = 0;
						this->resources.push_back(res);
						this->res_idx_map[res_name] = this->n_res++;
					}
				}
			}


			op_id++;
			idx++;
		}

		train.op_end = idx;

		this->trains.push_back(train);
		train_id++;
	}

	assert(train_id == this->trains.size());
	assert(idx == this->ops.size());

	// add succesors and resources
	train_id = 0;
	idx = 0;
	for (auto train_json : td_json["trains"]) {
		op_id = 0;
		Train& train = this->trains[train_id];

		train.fork_begin = this->n_fork;

		for (auto op_json : train_json) {
			Operation& op = this->ops[idx];

			op.succ.clear();
			op.n_succ = 0;
			for (uint succ_id : op_json["successors"]) {
				op.succ.push_back(op_idx_map[train_id][succ_id]);
				op.n_succ++;
				// this->ops[op_idx_map[train_id][succ_id]].pred.push_back(idx);
			}

			// set forks
			if (op.n_succ > 1) {
				op.fork_idx = this->n_fork++;
				this->fork_idx_map.push_back(idx);
			}

			// add resource to op
			op.res.clear();
			op.res_release_time.clear();
			if (op_json.contains("resources")) {
				for (auto res_json : op_json["resources"]) {
					std::string res_name = res_json["resource"];
					uint res_idx = this->res_idx_map.at(res_name);
					Resource& res = this->resources[res_idx];
					
					op.res.push_back(res_idx);
					res.trains.push_back(train_id);
					res.n_trains++;

					uint release_time = 0;
					if (res_json.contains("release_time"))
						release_time = res_json["release_time"];
					op.res_release_time.push_back(release_time);
				}
			}

			op_id++;
			idx++;
		}

		train.fork_end = this->n_fork;
		train_id++;
	}

	for (auto objective_json : td_json["objective"]) {
		if (objective_json["type"] != "op_delay")
			continue;

		train_id = objective_json["train"];
		op_id = objective_json["operation"];
		idx = this->op_idx_map[train_id][op_id];

		Objective obj;

		obj.op_idx = idx;

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
		this->ops[idx].objective = &this->objectives[this->objectives.size()-1];
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


