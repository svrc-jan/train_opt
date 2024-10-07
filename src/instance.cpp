#include "instance.hpp"

Instance::Instance(const std::string& name, const std::string& json_file)
	: name(name)
{
	this->parse_json(json_file);
 
	for (uint i = 0; i < this->ops.size(); i++) {
		Operation& op = this->ops[i];
		if (op.pred.size() == 1 && op.succ.size() == 1) {
			std::cout << "train " << op.train_id
				<< ", op " << op.op_id
				<< std::endl;
		}
	}
}


void Instance::build_paths()
{
	for (uint train_id = 0; train_id < this->trains.size(); train_id++) {
		std::vector<uint> start_vec({this->trains[train_id].begin_idx});
		this->build_path_rec(start_vec, train_id);
		std::cout << train_id << ": "<< this->trains[train_id].paths.size() << std::endl;
	}
}

void Instance::build_path_rec(const std::vector<uint>& path, uint train_id)
{
	const Operation& last_op = this->ops[path.back()];
	if (last_op.succ.size() == 0) {
		std::vector<std::vector<uint>>& train_paths = this->trains[train_id].paths;
		if (std::find(train_paths.begin(), train_paths.end(), path) == train_paths.end()) {
			train_paths.push_back(path);
		}
		// train_paths.push_back(path);
		// std::cout << train_paths.size() << std::endl;
		// print_vec(std::cout, path);
		// std::cout << std::endl;
	}
	else {
		for (uint succ_idx : last_op.succ) {
			std::vector<uint> new_path = path;
			new_path.push_back(succ_idx);
			this->build_path_rec(new_path, train_id);
		}
	}
}

void Instance::parse_json(const std::string& json_file)
{
	json td_json = get_json_file(json_file);

	this->trains.clear();
	this->ops.clear();
	
	uint train_id = 0;
	uint op_id = 0;
	uint idx = 0;

	// build basic operation info and resource map
	for (auto train_json : td_json["trains"]) {
		op_id = 0;
		Train train;
		train.begin_idx = idx;

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
				for (auto res : op_json["resources"]) {
					std::string res_name = res["resource"];
					if (this->res_idx_map.find(res_name) == this->res_idx_map.end())
						this->res_idx_map[res_name] = this->n_res++;
				}
			}


			op_id++;
			idx++;
		}

		train.end_idx = idx;

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
		// Train& train = this->trains[train_id];

		for (auto op_json : train_json) {
			Operation& op = this->ops[idx];

			op.succ.clear();
			for (uint succ_id : op_json["successors"]) {
				op.succ.push_back(op_idx_map[train_id][succ_id]);
				this->ops[op_idx_map[train_id][succ_id]].pred.push_back(idx);
			}

			// add resource to op
			op.res.clear();
			op.res_release_time.clear();
			if (op_json.contains("resources")) {
				for (auto res : op_json["resources"]) {
					std::string res_name = res["resource"];
					op.res.push_back(this->res_idx_map.at(res_name));

					uint release_time = 0;
					if (res.contains("release_time"))
						release_time = res["release_time"];
					op.res_release_time.push_back(release_time);
				}
			}

			op_id++;
			idx++;
		}
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
	}
}

