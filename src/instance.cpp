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
}


void Instance::add_ops(const Base_data& data)
{
	this->trains.reserve(data.n_trains());
	this->ops.reserve(data.n_ops());
	this->op_succ.reserve(data.n_op_succ());
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

			for (int base_succ : base_op.succ) {
				op.succ.size += 1;
				this->op_succ.push_back(base_succ + train.op_begin);
			}

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
	int op_succ_idx = 0;
	int op_res_idx = 0;

	for (Train& train : this->trains) {
		train.ops.assign_ptr(this->ops, ops_idx);

		for (Op& op : train.ops) {
			op.succ.assign_ptr(this->op_succ, op_succ_idx);
			op.res.assign_ptr(this->op_res, op_res_idx);
		}
	}
	
}