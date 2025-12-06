#include "preprocess.hpp"

#include <cstdio>
#include <iostream>
#include "utils/disjoint_set.hpp"
#include "utils/stl_print.hpp"

Preprocess::Preprocess(const Instance& inst) : inst(inst)
{
	this->trains.resize(this->inst.n_trains(), PP_train());
	this->make_levels();
	this->make_level_bounds();
}


void Preprocess::make_levels()
{
	this->op_level.resize(this->inst.n_ops(), {-1, -1});

	this->level_ops_out.reserve(this->inst.n_ops());
	this->level_ops_in.reserve(this->inst.n_ops());

	vector<int> zero_in;
	vector<int> topo_gen;		

	for (int t = 0; t < this->n_trains(); t++) {
		auto& inst_train = this->inst.trains[t];
		auto& train = this->trains[t];

		int op_offset = inst_train.op_start;
		int n_train_ops = inst_train.ops.size;

		Disjoint_set disj_set(n_train_ops);
	
		for (auto& op : inst_train.ops) {
			for (int i = 0; i < op.succ.size; i++) {
				for (int j = i+1; j < op.succ.size; j++) {
					disj_set.union_set(op.succ[i] - op_offset, op.succ[j] - op_offset);
				}
			}
		}

		auto succ_set = disj_set.get_result();
		vector<vector<int>> succ_groups(disj_set.n_sets, vector<int>());

		for (int i = 0; i < n_train_ops; i++) {
			succ_groups[succ_set[i]].push_back(i + op_offset);
		}

#ifndef NO_VLA
		int op_succ_group[n_train_ops];
		memset(op_succ_group, -1, sizeof(op_succ_group));
#else
		vector<int> op_succ_group(n_train_ops, -1);
#endif
	
		for (int sg = 0; sg < disj_set.n_sets; sg++) {
			for (int o : succ_groups[sg]) {
				for (int p : this->inst.ops[o].pred) {
					int curr_sg = op_succ_group[p - op_offset];
					assert(curr_sg == -1 || curr_sg == sg);
					op_succ_group[p - op_offset] = sg;
				}
			}
		}

#ifndef NO_VLA
		int n_in[n_train_ops];
		memset(n_in, 0, sizeof(n_in));
#else
		vector<int> n_in(n_train_ops, 0);
#endif

		for (int i = 0; i < n_train_ops; i++) {
			int sg = op_succ_group[i];
			if (sg < 0) {
				continue;
			}
			for (int s : succ_groups[sg]) {
				n_in[s - op_offset] += 1;
			}
		}

		train.level_start = this->n_levels();

		zero_in.push_back(inst_train.op_start);

		while (!zero_in.empty()) {
			topo_gen = zero_in;
			zero_in.clear();

			for (int o : topo_gen) {
				int sg = op_succ_group[o - op_offset];
				if (sg < 0) {
					continue;
				}
				for (int s : succ_groups[sg]) {	
					n_in[s - op_offset] -= 1;
					if (n_in[s - op_offset] == 0) {
						zero_in.push_back(s);
					}
				}
			}

			train.levels.size += 1;
			this->levels.push_back(Level());

			this->levels.back().ops_out.size = topo_gen.size();

			this->level_ops_out.insert(
				this->level_ops_out.end(),
				topo_gen.begin(),
				topo_gen.end()
			);	
		}

		train.levels.size += 1;
		this->levels.push_back(Level());
	}


	int level_ops_out_idx = 0;
	for (int l = 0; l < this->n_levels(); l++) {
		auto& level = this->levels[l];
		level.ops_out.assign_ptr(this->level_ops_out, level_ops_out_idx);

		for (int o : level.ops_out) {
			this->op_level[o].first = l;
		}
	}

	for (int t = 0; t < this->n_trains(); t++) {
		auto& inst_train = this->inst.trains[t];
		auto& train = this->trains[t];

		train.levels.ptr = &(this->levels[train.level_start]);

		for (int o = inst_train.op_start; o < inst_train.op_end(); o++) {
			auto& op = this->inst.ops[o];

			if (op.succ.size > 0) {
				int succ_level = this->op_level[op.succ[0]].first;
				for (int s : op.succ) {
					assert(this->op_level[s].first == succ_level);
				}
				this->op_level[o].second = succ_level;
				this->levels[succ_level].ops_in.size += 1;
			}
			else {
				this->op_level[o].second = train.level_last();
				train.levels.back().ops_in.size += 1;
			}
		}
	}

	int level_ops_in_idx = 0;
	for (auto& level : this->levels) {
		level.ops_in.assign_ptr(this->level_ops_in, level_ops_in_idx);
	}

	vector<int> level_ops_in_filled(this->n_levels(), 0);

	for (int o = 0; o < this->inst.n_ops(); o++) {
		assert(this->op_level[o].first >= 0);
		assert(this->op_level[o].first < this->op_level[o].second);
		assert(this->op_level[o].second < this->n_levels());

		int l = this->op_level[o].second;
		auto& level = this->levels[l];

		level.ops_in[level_ops_in_filled[l]++] = o;
	}

	for (int l = 0; l < this->n_levels(); l++) {
		auto& level = this->levels[l];
		assert(level_ops_in_filled[l] == level.ops_in.size);
	}


	for (int o = 0; o < this->inst.n_ops(); o++) {
		for (int l = this->op_level[o].first + 1; l < this->op_level[o].second; l++) {
			this->levels[l].is_req = false;
		}
	}


	int not_req = 0;
	for (auto& level : this->levels) {
		if (!level.is_req) {
			not_req += 1;
		}
	}


	cout << "n ops: " << this->inst.n_ops() 
		<< ", n levels: " << this->n_levels()
		<< " (not req: " << not_req << ")" << endl;
}


void Preprocess::make_level_bounds()
{
	for (auto& level : this->levels) {
		level.time_lb = INT_MAX;
		level.time_ub = 0;

		if (level.ops_out.size > 0) {
			for (int o_out : level.ops_out) {
				auto& op_out = this->inst.ops[o_out];
				level.time_lb = min(level.time_lb, op_out.start_lb);
				level.time_ub = max(level.time_ub, op_out.start_ub);
			}
		}
		else {
			for (int o_in : level.ops_in) {
				auto& op_in = this->inst.ops[o_in];
				level.time_lb = min(level.time_lb, op_in.start_lb + op_in.dur);
				level.time_ub = max(level.time_ub, (op_in.start_ub == INT_MAX)
					? INT_MAX : (op_in.start_ub + op_in.dur));
			}
		}
	}
}



bool Preprocess::is_routing_level(int l)
{
	auto& level = this->levels[l];
	if (!level.is_req) {
		return false;
	}

	for (int o_in : level.ops_in) {
		if (this->inst.ops[o_in].succ.size != level.ops_out.size) {
			return false;
		}
	}

	for (int o_out : level.ops_out) {
		if (this->inst.ops[o_out].pred.size != level.ops_in.size) {
			return false;
		}
	}

	return true;
}


