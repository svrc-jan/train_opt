#include "instance.hpp"

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

#include "utils/files.hpp"
#include "utils/disjoint_set.hpp"


Instance::Instance(string file_name)
{
	this->parse_json_file(file_name);
	this->assign_pointers();
	this->assign_prev_ops();
	this->sort_arrays();
	this->assign_levels();
	this->calculate_dist_to_end();
	this->adjust_start_ub();
	this->find_unlock_paths();
}


Instance::~Instance()
{

}

void Instance::parse_json_file(const string file_name)
{

	json inst_jsn = get_json_file(file_name);
	this->reserve_mem(inst_jsn);
	
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

	for (const json& op_jsn : train_jsn) {
		this->parse_json_op(op_jsn, train);
	}

	this->trains.push_back(train);
}

void Instance::parse_json_op(const json& op_jsn, Train& train)
{
	Op op = {
		.idx = this->n_ops(),
		.train = &(this->trains[train.idx]),
		.dur = op_jsn["min_duration"]
	};

	if (op_jsn.contains("start_lb")) {
		op.start_lb = op_jsn["start_lb"];
	}

	if (op_jsn.contains("start_ub")) {
		op.start_ub = op_jsn["start_ub"];
	}

	
	for (int s : op_jsn["successors"]) {
		int succ_idx = s + train.op_begin;
		
		assert(succ_idx > op.idx);

		op.n_succ()++;
		this->op_succ.push_back(&(this->ops[succ_idx]));
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
			this->op_res.push_back(res_idx);
			this->op_res_info.push_back({.time = res_time});
		}
	}

	train.n_ops()++;
	this->ops.push_back(op);
}


void Instance::reserve_mem(const json& inst_jsn)
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
			}
		}
	}

	this->ops.reserve(ops_size);
	this->trains.reserve(trains_size);
	this->op_succ.reserve(succ_size);
	this->op_res.reserve(res_size);
	this->op_res_info.reserve(res_size);
}


void Instance::assign_pointers()
{
	int idx;
	
	// train -> op
	idx = 0;
	for (Train& train : this->trains) {
		if (train.n_ops() > 0) {
			assert(train.op_begin == idx);

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
	}
	assert(idx == (int)this->op_succ.size());

	// op -> res
	idx = 0;
	for (Op& op : this->ops) {
		if (op.n_res() > 0) {
			op.res.ptr = &(this->op_res[idx]);
			op.res_info = &(this->op_res_info[idx]);

			idx += op.n_res();
		}
	}
	assert(idx == (int)this->op_res.size());
}


void Instance::assign_prev_ops()
{
	// increment the number of prev for each op
	for (const Op& op : this->ops) {
		for (const Op* succ : op.succ) {
			this->ops[succ->idx].n_prev() += 1;
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
	for (Op& op : this->ops) {
		for (Op* succ : op.succ) {
			succ->prev[op_prev_filled[succ->idx]++] = &op;
		}
	}	
}


void Instance::sort_arrays()
{
	for (Op& op : this->ops) {
		if (!op.succ.is_asc()) {
			op.succ.sort();
		}

		if (!op.prev.is_asc()) {
			op.prev.sort();
		}

		if (!op.res.is_asc()) {
			op.res.sort();
		}
	}
}


void Instance::assign_levels()
{
	this->level_ops_out.reserve(this->n_ops());

	vector<int> op_level_start_idx(this->n_ops());

	for (Train& train : this->trains) {
		train.level_begin = this->n_levels();

		Disjoint_set ds(train.n_ops());
		
		for (const Op& op : train.ops) {
			for (int i = 0; i < op.n_succ(); i++) {
				int a = op.succ[i]->idx - train.op_begin;
				for (int j = i + 1; j < op.n_succ(); j++) {
					int b = op.succ[j]->idx - train.op_begin;

					ds.union_set(a, b);
				}
			}
		}

		auto sets = ds.get_sets();

		this->levels.reserve(this->levels.size() + sets.first.size() + 1);

		int sets_idx = 0;
		for (auto set_size : sets.first) {
			Level level = {
				.idx = this->n_levels(),
				.train = &(this->trains[train.idx])
			};

			level.n_ops_out() = set_size;

			for (int i = 0; i < set_size; i++) {
				int o = sets.second[i + sets_idx] + train.op_begin;
				op_level_start_idx[o] = level.idx;	
			}
			sets_idx += set_size;


			train.n_levels() += 1;
			this->levels.push_back(level);
		}
		assert(sets_idx == train.n_ops());

		Level last_level = {
			.idx = this->n_levels(),
			.train = &(this->trains[train.idx])
		};

		train.n_levels() += 1;
		this->levels.push_back(last_level);

		for (int i : sets.second) {
			this->level_ops_out.push_back(&(train.ops[i]));
		}
	}

	this->levels.shrink_to_fit();
	
	// assign levels to trains
	int idx_level = 0;
	for (Train& train : this->trains) {
		train.levels.ptr = &(this->levels[train.level_begin]);
		assert(idx_level == train.level_begin);
		idx_level += train.n_levels();
	}
	assert(idx_level == this->n_levels());

	// assign start levels to ops
	for (Op& op : this->ops) {
		op.level_start = &(this->levels[op_level_start_idx[op.idx]]);
	}

	// assign level ends to ops
	for (Op& op : this->ops) {
		for (Op* prev : op.prev) {
			if (prev->level_end == nullptr) {
				prev->level_end = op.level_start;
			}
			else {
				assert(prev->level_end == op.level_start);
			}
		}
	}

	// assign last op end to last level
	for (Train& train : this->trains) {
		train.ops.back().level_end = &(this->levels[train.levels.back().idx]);
	}

	// count ops in for levels
	for (Op& op : this->ops) {
		assert(op.level_start != nullptr && op.level_end != nullptr);
		op.level_end->n_ops_in() += 1;
	}

	// assign ops out and ops in array pointers
	this->level_ops_in.resize(this->n_ops());

	int idx_out = 0;
	int idx_in = 0;

	for (Level& level : this->levels) {
		if (level.n_ops_out() > 0) {
			level.ops_out.ptr = &(this->level_ops_out[idx_out]);
			idx_out += level.n_ops_out();
		}
		
		if (level.n_ops_in() > 0) {
			level.ops_in.ptr = &(this->level_ops_in[idx_in]);
			idx_in += level.n_ops_in();
		}
	}
	
	assert(idx_out == this->n_ops());
	assert(idx_in == this->n_ops());

	// fill ops in
	vector<int> level_ops_in_filled(this->n_levels(), 0);
	for (Op& op : this->ops) {
		op.level_end->ops_in[level_ops_in_filled[op.level_end->idx]++] = &(this->ops[op.idx]);
	}

	for (const Level& level : this->levels) {
		assert(level_ops_in_filled[level.idx] == level.n_ops_in());
	}
	level_ops_in_filled.clear();


	// check if all op_in -> op_out transitions are allowed

	vector<int> idx_allowed(this->n_levels(), -1);
	for (Level& level : this->levels) {
		assert(level.n_ops_out() <= 32);

		bool allowed_req = false;
		vector<uint32_t> allowed(level.n_ops_in(), 0);
		
		for (int i = 0; i < level.n_ops_in(); i++) {
			Op* op_in = level.ops_in[i];

			for (int j = 0; j < level.n_ops_out(); j++) {
				Op* op_out = level.ops_out[j];

				if (op_in->succ.find_sorted(op_out) == -1) {
					allowed_req = true;
				}
				else {
					allowed[i] |= (1U << j);
				}
			}
		}

		if (allowed_req) {
			idx_allowed[level.idx] = this->level_allowed.size();
		
			this->level_allowed.reserve(this->level_allowed.size() + allowed.size());
			this->level_allowed.insert(this->level_allowed.end(),
				allowed.begin(), allowed.end());
		}		
	}

	// assign array pointers
	for (Level& level : this->levels) {
		if (idx_allowed[level.idx] > -1) {
			level.allowed = &(this->level_allowed[idx_allowed[level.idx]]);
		}
	}

	// set level start and end i
}


void Instance::calculate_dist_to_end()
{
	vector<int> out_remaining(this->n_levels());
	for (Level& level : this->levels) {
		out_remaining[level.idx] = level.n_ops_out();
	}

	this->max_dur = 0;
	for (Train& train : this->trains) {
		Level* last_level = &(train.levels.back());

		last_level->dur_to_end = 0;

		queue<Level *> q({last_level});

		while (!q.empty()) {
			Level* curr_level = q.front();
			q.pop();

			for (Op* op : curr_level->ops_in) {
				Level* prev_level = op->level_start;

				prev_level->dur_to_end = min(
					prev_level->dur_to_end, 
					curr_level->dur_to_end + op->dur);
				
				out_remaining[prev_level->idx] -= 1;
				if (out_remaining[prev_level->idx] == 0) {
					q.push(prev_level);
				}
			}
		}

		train.max_dur = train.levels[0].dur_to_end;
		this->max_dur += train.max_dur;
	}
}


struct Op_order
{
	Op* op = nullptr;
	int time = 0;

	inline bool operator<(const Op_order& other) { return this->time > other.time; }
};


void Instance::adjust_start_ub()
{

}


void Instance::find_unlock_paths()
{
	// vector<int> n_succ(this->n_ops());
	// queue<Op *> q;

	// for (Op& op : this->ops) {
	// 	n_succ[op.idx] = op.n_succ();
	// 	if (op.n_succ() == 0) {
	// 		q.push(&(this->ops[op.idx]));
	// 	}
	// }

	// while(!q.empty()) {
	// 	Op* op = q.front(); q.pop();

	// 	for (Op* prev : op->prev) {
	// 		n_succ[prev->idx] -= 1;
	// 		if (n_succ[prev->idx] == 0) {
	// 			q.push(prev);				
	// 		}
	// 	}

	for (int o = this->n_ops() - 1; o >= 0; o--) {
		Op* op = &(this->ops[o]);

		for (int i = 0; i < op->n_res(); i++) {
			int res_idx = op->res[i];


			Res_info& res_info = op->res_info[i];
			int orig_time = res_info.time;

			bool unlockable = false;
			
			for (Op* succ : op->succ) {
				int succ_i = succ->res.find(res_idx);
				
				if (succ_i == -1) {
					unlockable = true;
					continue;
				}

				Res_info& succ_info = succ->res_info[succ_i];

				bool succ_better = false;
				if (succ_info.unlock_dur < res_info.unlock_dur) {
					succ_better = true;
				}
				else if (succ_info.unlock_dur == res_info.unlock_dur) {
					if (succ_info.unlock_jumps == res_info.unlock_jumps) {
						succ_better = true;
					}
				}

				if (succ_better) {
					res_info = {
						.last_op = succ_info.last_op,
						.next_op = succ,
						.time = succ_info.time,
						.next_res_i = succ_i,
						.unlock_dur = succ_info.unlock_dur + op->dur,
						.unlock_jumps = succ_info.unlock_jumps + 1
					};
				}

				assert(succ_info.last_op != nullptr);
			}

			if (unlockable || res_info.last_op == nullptr) {
				res_info = {
					.last_op = op,
					.next_op = nullptr,
					.time = orig_time,
					.next_res_i = -1,
					.unlock_dur = op->dur + orig_time,
					.unlock_jumps = 1
				};
			}
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


vector<Op*> Instance::find_path(Op* start, Op* end)
{
	if (start->train != end->train || start->idx > end->idx) {
		return {};
	}

	if (start == end) {
		return {start};
	}

	
	unordered_map<Op *, Op *> prev_op;

	queue<Op*> q;
	q.push(start);

	while (!q.empty()) {
		Op* curr = q.front(); q.pop();
		if (curr == end) {
			break;
		}

		for (Op* succ : curr->succ) {
			if (prev_op.find(succ) == prev_op.end()) {
				q.push(succ);
				prev_op[succ] == curr;
			}
		}
	}

	vector<Op*> path = {end};

	auto it = prev_op.find(end);
	if (it == prev_op.end()) {
		return {};
	}

	while (true) {
		Op* op = it->second;
		path.push_back(op);
		if (op == start) {
			break;
		}
		it = prev_op.find(it->second);
	}
	
	reverse(path.begin(), path.end());
	
	return path;
}


bool Level::is_allowed(Op* op_in, Op* op_out) const
{
	if (this->allowed == nullptr) {
		return true;
	}

	int i = this->ops_in.find_sorted(op_in);
	int j = this->ops_in.find_sorted(op_out);

	assert(i != -1 && j != -1);

	return (this->allowed[i] & (1U << j) != 0);
}


std::ostream& operator<<(std::ostream& stream, const Op& op)
{
	stream << "Op(idx=" << op.idx << ", train=" << op.train <<")";
	return stream;
}


std::ostream& operator<<(std::ostream& stream, const Level& level)
{
	stream << "Level(idx=" << level.idx << ", train=" << level.train <<")";
	return stream;
}
