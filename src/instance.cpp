#include "instance.hpp"

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

#include "utils/files.hpp"
#include "utils/disjoint_set.hpp"


Instance::Instance(string file_name)
{
	Base_data data(file_name);

	this->n_res = data.n_res();
	this->add_ops(data);
	this->add_levels(data);
}


void Instance::add_ops(const Base_data& data)
{

	this->trains.reserve(data.n_trains());
	this->ops.reserve(data.n_ops());
	this->op_res.reserve(data.n_op_res());

	for (auto& base_train : data.trains) {
		Train train;

		train.op_begin = this->n_ops();

		for (int i = 0; i < base_train.ops.size; i++) {
			const Base_op& base_op = base_train.ops[i];

			Op op = {
				.dur = base_op.dur,
				.start_lb = base_op.start_lb,
				.start_ub = base_op.start_ub,
			};

			for (auto& base_res : base_op.res) {
				Res res = {
					.idx = base_res.idx,
					.time = base_res.time
				};

				op.res.size += 1;
				this->op_res.push_back(res);
			}

			train.ops.size += 1;
			this->ops.push_back(op);
		}

		this->trains.push_back(train);
	}

	int ops_idx = 0;
	int op_res_idx = 0;

	for (Train& train : this->trains) {
		train.ops.assign_ptr(this->ops, ops_idx);

		for (Op& op : train.ops) {
			op.res.assign_ptr(this->op_res, op_res_idx);
		}
	}
	
}


void Instance::add_levels(const Base_data& data)
{
	vector<vector<int>> set_idx(this->n_trains());
	
	for (int t = 0; t < this->n_trains(); t++) {
		auto& base_train = data.trains[t];
		Train& train = this->trains[t];

		Disjoint_set disj_set(train.n_ops());

		for (auto& base_op : base_train.ops) {
			for (int i = 0; i < base_op.succ.size; i++) {
				for (int j = i + 1; j < base_op.succ.size; j++) {
					disj_set.union_set(base_op.succ[i], base_op.succ[j]);
				}
			}
		}

		train.levels.size = disj_set.n_sets + 1;

		set_idx[t] = disj_set.get_result();
	}

	int level_idx = 0;

	for (int t = 0; t < this->n_trains(); t++) {
		auto& base_train = data.trains[t];
		Train& train = this->trains[t];
	
		train.level_begin = level_idx;
		level_idx += train.levels.size;

		for (int i = 0; i < train.n_ops(); i++) {
			train.ops[i].level_start = set_idx[t][i] + train.level_begin;
		}
	}

	for (int t = 0; t < this->n_trains(); t++) {
		auto& base_train = data.trains[t];
		Train& train = this->trains[t];

		for (int i = 0; i < train.n_ops(); i++) {
			auto& base_op = base_train.ops[i];
			Op& op = train.ops[i];

			if (base_op.succ.size > 0) {
				Op& succ = train.ops[base_op.succ[0]];

				op.level_end = succ.level_start;
			}
			else {
				op.level_end = train.level_last();
			}
		}
	}

	this->levels.resize(level_idx, Level());

	for (int t = 0; t < this->n_trains(); t++) {
		Train& train = this->trains[t];

		train.levels.ptr = &(this->levels[train.level_begin]);

		for (Level& level : train.levels) {
			level.train = t;
		}
	}

	for (int t = 0; t < this->n_trains(); t++) {
		auto& base_train = data.trains[t];
		const Train& train = this->trains[t];
	
		for (int i = 0; i < train.n_ops(); i++) {
			auto& base_op = base_train.ops[i];
			const Op& op = train.ops[i];

			assert(op.level_start != -1 && op.level_end != -1);

			for (int j : base_op.succ) {
				const Op& succ = train.ops[j];
				assert(op.level_end == succ.level_start);
			}	
		}

		assert(train.ops[0].level_start == train.level_begin);
		assert(train.ops.back().level_end == train.level_last());
	}
	
	this->level_ops_in.resize(this->n_ops());
	this->level_ops_out.resize(this->n_ops());

	for (const Op& op : this->ops) {
		this->levels[op.level_end].ops_in.size += 1;
		this->levels[op.level_start].ops_out.size += 1;
	}

	int ops_in_idx = 0;
	int ops_out_idx = 0;

	for (Level& level : this->levels) {
		level.ops_in.assign_ptr(this->level_ops_in, ops_in_idx);
		level.ops_out.assign_ptr(this->level_ops_out, ops_out_idx);
	}

	vector<int> ops_in_filled(this->n_levels(), 0);
	vector<int> ops_out_filled(this->n_levels(), 0);

	for (int o = 0; o < this->n_ops(); o++) {
		const Op& op = this->ops[o];
		this->levels[op.level_end].ops_in[ops_in_filled[op.level_end]++] = o;
		this->levels[op.level_start].ops_out[ops_out_filled[op.level_start]++] = o;
	}
}
