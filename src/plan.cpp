#include "plan.hpp"

Plan::Plan(const Solution *sol) : sol(sol), n_train(sol->n_train)
{
	this->resize_to_sol();
	this->reset();
	this->build();
}

void Plan::resize_to_sol()
{
	this->paths_idx.resize(this->n_train);
	this->next_ops.resize(this->n_train);
	
	this->res.resize(this->sol->inst->n_res);
	this->size = 0;
	for (uint train_idx = 0; train_idx < n_train; train_idx++) {
		this->size += this->sol->paths[train_idx].size();
	}

	this->ops.resize(this->size);
}

void Plan::reset()
{
	this->plan_idx = 0;
	std::fill(this->paths_idx.begin(), this->paths_idx.end(), 0);

	Plan_op op;
	std::fill(this->ops.begin(), this->ops.end(), op);

	Plan_res r;
	std::fill(this->res.begin(), this->res.end(), r);

	this->reset_next_ops();
}

void Plan::reset_next_ops()
{
	for (uint train_idx = 0; train_idx < n_train; train_idx++) {
		auto op = this->get_op(train_idx, 0);
		Plan_op& next_op = this->next_ops[train_idx];

		next_op.train_idx = train_idx;
		next_op.op_idx = 0;
		// next_op.path_idx = 0;
		next_op.start = op.start_lb;
	}
}

bool Plan::is_train_unlocked(const uint train_idx)
{
	Plan_op& plan_op = this->next_ops[train_idx];
	auto op = this->get_op(plan_op);

	for (auto res_idx : op.res_idx) {
		if (this->res[res_idx].locked)
			return false;
	}
	return true;
}

void Plan::unlock_res(const uint curr_train_idx, const uint res_idx, const uint release_time)
{
	this->res[res_idx].locked = false;
	this->res[res_idx].release_time = release_time;
	for (uint train_idx = 0; train_idx < n_train; train_idx++) {
		if (train_idx == curr_train_idx)
			continue;

		Plan_op& plan_op = this->next_ops[train_idx];

		auto op = this->get_op(plan_op);

		for (uint r = 0; r < op.n_res; r++) {
			if (op.res_idx[r] = res_idx) {
				plan_op.start = std::max(plan_op.start, release_time);
				break;
			}
		}
	}
}

void Plan::lock_res(const uint res_idx)
{
	this->res[res_idx].locked = true;
}

void Plan::advance_train(const uint train_idx)
{
	uint& path_idx = this->paths_idx[train_idx];
	Plan_op& plan_op = this->next_ops[train_idx];
	auto op = this->get_op(plan_op);

	if (path_idx > 0) {
		auto prev_op = get_path_op(train_idx, path_idx - 1);
		for (uint i = 0; i < prev_op.n_res; i++) {
			this->unlock_res(train_idx, prev_op.res_idx[i], 
				plan_op.start + prev_op.res_release_time[i]);
		}
	}

	for (auto res_idx : op.res_idx) {
		this->lock_res(res_idx);
	}

	this->ops[plan_idx++] = plan_op;
	path_idx++;

	if (path_idx >= this->get_paths()[train_idx].size()) {
		plan_op.op_idx = -1;
		plan_op.start = -1;
		return;
	}

	plan_op.op_idx = this->get_path_op_idx(train_idx, path_idx);
	auto next_op = this->get_op(plan_op);
	plan_op.start = std::max(plan_op.start + op.dur, next_op.start_lb);

	for (auto res_idx : next_op.res_idx) {
		plan_op.start = std::max(plan_op.start, this->res[res_idx].release_time);
	}
}

void Plan::build()
{
	while (this->plan_idx < this->size) {
		Plan_op next_op;
		for (uint train_idx = 0; train_idx < n_train; train_idx++) {
			if (is_train_unlocked(train_idx) && (this->next_ops[train_idx] < next_op)) {
				next_op = this->next_ops[train_idx];
			}
		}
		if (next_op.op_idx == (uint)-1) {
			break;
		}
		this->advance_train(next_op.train_idx);
	}
}

