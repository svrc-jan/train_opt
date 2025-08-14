#include "instance.hpp"


Operation& Train::get_op(const int idx)
{
	return this->inst->ops[this->begin_idx + idx];
}

Instance::Instance(const std::string& json_file)
	: name(json_file)
{
	this->parse_json(json_file);
	this->make_res_bin_vec();
	this->res_overlap_cache.clear();
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


void Instance::make_res_bin_vec()
{
	bin_vec::set_req_n_blocks(this->n_res);
	this->res_bin_vec = vector<bin_vec::block_t>(
		bin_vec::get_n_blocks()*(this->n_op + 1), 0);
	
	for (int i = 0; i < this->n_op; i++) {
		this->ops[i].res_vec = &(this->res_bin_vec[i*bin_vec::get_n_blocks()]);
		bin_vec::clear(this->ops[i].res_vec);

		vector<int> bit_idx = {};
		bit_idx.reserve(this->ops[i].n_res);

		for (const auto& res : this->ops[i].res) {
			bit_idx.push_back(res.id);
		}

		bin_vec::fill(this->ops[i].res_vec, bit_idx);
	}

	this->start_res_vec = &(this->res_bin_vec[this->n_op*bin_vec::get_n_blocks()]);
	bin_vec::clear(this->start_res_vec);

	for (auto& tr : this->trains) {
		bin_vec::or_(
			this->start_res_vec,
			this->start_res_vec,
			tr[0].res_vec);
	}
}


int Instance::get_res_overlap(const int a, const int b) const
{
	std::pair<int, int> idx = (a < b) ? 
		std::pair<int, int>(a, b) : 
		std::pair<int, int>(b, a);

	auto it = this->res_overlap_cache.find(idx);
	if (it != this->res_overlap_cache.end()) {
		return it->second;
	}

	int rv = bin_vec::count_overlap(
		this->ops[a].res_vec,
		this->ops[b].res_vec
	);

	this->res_overlap_cache[idx] = rv;

	return rv;
}
