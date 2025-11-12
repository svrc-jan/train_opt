#include "base_data.hpp"

Base_data::Base_data(const string& file_name)
{
	this->parse_json(file_name);
	this->make_prev();
}


Base_data::~Base_data()
{
	
}


void Base_data::parse_json(string file_name)
{
	this->trains.clear();
	this->ac_ops.clear();
	this->ac_succ.clear();
	this->ac_res.clear();

	json inst_jsn = get_json_file(file_name);

	this->trains.reserve(inst_jsn["trains"].size());
	

	for (const auto& train_jsn : inst_jsn["trains"]) {
		Base_train train;

		size_t n_ops = train_jsn.size();
		train.ops = this->ac_ops.add_entry(n_ops);

		for (size_t i_op = 0; const auto& op_jsn : train_jsn) {
			Base_op op;

			op.dur = op_jsn["min_duration"];

			if (op_jsn.contains("start_lb"))
				op.start_lb = op_jsn["start_lb"];

			if (op_jsn.contains("start_ub"))
				op.start_ub = op_jsn["start_ub"];
			
			size_t n_succ = op_jsn["successors"].size();

			op.succ = this->ac_succ.add_entry(n_succ);
			for (size_t i_succ = 0; int succ : op_jsn["successors"]) {
				op.succ[i_succ] = succ;

				++i_succ;
			}

			assert(is_ascending(op.succ));
			
			if (op_jsn.contains("resources")) {
				size_t n_res = op_jsn["resources"].size();
			
				op.res = this->ac_res.add_entry(n_res);
				
				for (size_t i_res = 0; const auto& res_jsn : op_jsn["resources"]) {
					std::string res_name = res_jsn["resource"];
					
					size_t res_idx = this->get_res_idx(res_name);
					op.res[i_res] = res_idx;
					
					int time = 0;
					if (res_jsn.contains("release_time")) {
						time = res_jsn["release_time"];
					}
					
					if (res_idx < this->res_time.size()) {
						assert(this->res_time[res_idx] == time);
					}
					else {
						this->res_time.push_back(time);
						assert(this->res_time.size() == res_idx + 1);
					}

					++i_res;
				}

				if (!is_ascending(op.res)) {
					sort(op.res.begin(), op.res.end());
				}
			}
		
			train.ops[i_op] = op;
			i_op++;

			
		}
		this->trains.push_back(train);
	}
}


void Base_data::make_prev()
{
	this->ac_prev.clear();
	this->ac_prev.reserve(this->ac_succ.size());

	for (auto& train : this->trains) {
		vector<int> prev_count(train.ops.size, 0);
		
		// count all prevs
		for (auto& op : train.ops) {
			for (int i_succ : op.succ) {
				prev_count[i_succ]++;
			}
		}

		// allocate
		for (int i_op = 0; auto& op : train.ops) {
			op.prev = this->ac_prev.add_entry(prev_count[i_op]);
			i_op++;
		}


		// fill values
		for (int i_op = 0; auto& op : train.ops) {
			for (int i_succ : op.succ) {
				Base_op& succ = train.ops[i_succ];
				succ.prev[succ.prev.size - (size_t)prev_count[i_succ]] = i_op;
				prev_count[i_succ]--;

				// if (prev_count[i_succ] == 0) {
				// 	assert(is_ascending(succ.prev));
				// }
			}

			i_op++;
		}
	}
}


int Base_data::get_res_idx(string res_name)
{
	auto it = this->res_name_idx_map.find(res_name);

	int res_idx;
	if (it != this->res_name_idx_map.end()) {
		res_idx = it->second;
	}
	else {
		res_idx = this->res_name_idx_map.size();
		this->res_name_idx_map[res_name] = res_idx;
	}

	return res_idx;
}