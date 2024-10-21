#include "plan.hpp"

Plan::Plan(const Solution *sol) : sol(sol), n_train(sol->n_train)
{
	this->resize_to_sol();
}

void Plan::resize_to_sol()
{
	this->paths_idx.resize(this->n_train);
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

	Plan_op op = {-1, -1, -1, -1};
	std::fill(this->ops.begin(), this->ops.end(), op);

	Plan_res r = {false, 0};
	std::fill(this->res.begin(), this->res.end(), r);	
}

void Plan::build()
{

	while (this->plan_idx < this->size) {
		Plan_op next_op = {-1, -1, -1, -1};
		for (uint train_idx = 0; train_idx < this->n_train; train_idx++) {
			auto pos_op = this->get_path_op(train_idx, this->paths_idx[train_idx]);
		}
		
	}
	
}

