#include "preprocess.hpp"

#include <cstdio>
#include <iostream>
#include "utils/disjoint_set.hpp"
#include "utils/stl_print.hpp"

Preprocess::Preprocess(const Instance& inst) : inst(inst)
{
	this->trains.resize(this->inst.n_trains(), PP_train());
	this->make_levels();
}


void Preprocess::make_levels()
{
	Disjoint_set disj_set(this->inst.n_ops());
	
	for (auto& op : this->inst.ops) {
		for (int i = 0; i < op.succ.size; i++) {
			for (int j = i+1; j < op.succ.size; j++) {
				disj_set.union_set(op.succ[i], op.succ[j]);
			}
		}
	}

	auto succ_set = disj_set.get_result();
	vector<vector<int>> succ_groups(disj_set.n_sets, vector<int>());

	for (int o = 0; o < this->inst.n_ops(); o++) {
		succ_groups[succ_set[o]].push_back(o);
	}

#ifndef NO_VLA
	int op_succ_group[this->inst.n_ops()];
	memset(op_succ_group, -1, sizeof(op_succ_group));
#else
	vector<int> op_succ_group(this->inst.n_ops(), -1);
#endif
	
	for (int sg = 0; sg < disj_set.n_sets; sg++) {
		for (int o : succ_groups[sg]) {
			for (int p : this->inst.ops[o].pred) {
				assert(op_succ_group[p] == -1 || op_succ_group[p] == sg);
				op_succ_group[p] = sg;
			}
		}
	}

#ifndef NO_VLA
	int n_in[this->inst.n_ops()];
	memset(n_in, 0, sizeof(n_in));
#else
	vector<int> n_in(this->inst.n_ops(), 0);
#endif

	for (int o = 0; o < this->inst.n_ops(); o++) {
		int sg = op_succ_group[o];
		if (sg < 0) {
			continue;
		}
		for (int s : succ_groups[sg]) {
			n_in[s] += 1;
		}
	}

	vector<int> zero_in;
	vector<int> topo_gen;

	this->op_level_start.resize(this->inst.n_ops(), -1);
	this->op_level_end.resize(this->inst.n_ops(), -1);

	this->level_ops_out.reserve(this->inst.n_ops());
	this->level_ops_in.reserve(this->inst.n_ops());
	
	for (int t = 0; t < this->n_trains(); t++) {
		auto& inst_train = this->inst.trains[t];
		auto& train = this->trains[t];

		for (int i = 0; i < inst_train.ops.size; i++) {
			// n_in[i] = inst_train.ops[i].pred.size;
		}

		train.level_start = this->n_levels();

		zero_in.push_back(inst_train.op_start);

		while (!zero_in.empty()) {
			topo_gen = zero_in;
			zero_in.clear();

			for (int o : topo_gen) {
				int sg = op_succ_group[o];
				if (sg < 0) {
					continue;
				}
				for (int s : succ_groups[sg]) {	
					n_in[s] -= 1;
					if (n_in[s] == 0) {
						zero_in.push_back(s);
					}
				}
			}
 
			// cout << topo_gen << " ";

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
			this->op_level_start[o] = l;
		}
	}

	for (int t = 0; t < this->n_trains(); t++) {
		auto& inst_train = this->inst.trains[t];
		auto& train = this->trains[t];

		train.levels.ptr = &(this->levels[train.level_start]);

		for (int o = inst_train.op_start; o < inst_train.op_end(); o++) {
			auto& op = this->inst.ops[o];

			if (op.succ.size > 0) {
				int succ_level = this->op_level_start[op.succ[0]];
				for (int s : op.succ) {
					assert(this->op_level_start[s] == succ_level);
				}
				this->op_level_end[o] = succ_level;
				this->levels[succ_level].ops_in.size += 1;
			}
			else {
				this->op_level_end[o] = train.level_last();
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
		int l = this->op_level_end[o];
		auto& level = this->levels[l];

		level.ops_in[level_ops_in_filled[l]++] = o;
	}

	for (int l = 0; l < this->n_levels(); l++) {
		auto& level = this->levels[l];
		assert(level_ops_in_filled[l] == level.ops_in.size);
	}


	cout << "n ops: " << this->inst.n_ops() << ", n levels: " << this->n_levels() << endl;
}