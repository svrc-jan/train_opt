#include "tree.hpp"

#include <queue>
#include <cstdio>

struct Path_step
{
	int op = -1;
	int start = -1;
	int end = -1;
};


Tree::Tree(const Instance& inst) : inst(inst), graph(Graph(inst))
{
	this->n_solve_calls = 0;
}


void print_res_col(const Res_col& res_col, int type)
{
	char buf1[20];
	char buf2[20];
	char buf3[20];
	
	if (res_col.first.lock == res_col.first.unlock) {
		sprintf(buf1, "     %-4d", res_col.first.lock);
	}
	else {
		sprintf(buf1, "%4d:%-4d", res_col.first.lock, res_col.first.unlock);
	}

	if (res_col.second.lock == res_col.second.unlock) {
		sprintf(buf3, "%4d", res_col.second.lock);
	}
	else {
		sprintf(buf3, "%4d:%-4d", res_col.second.lock, res_col.second.unlock);
	}

	switch (type) {
	case 0:
		sprintf(buf2, "-->");
		break;
	
	case 1:
		sprintf(buf2, "<--");
		break;

	default:
		sprintf(buf2, " x ");
		break;
	}

	printf("%s %s %s\n", buf1, buf2, buf3);
}



void Tree::solve(int depth)
{
	if (depth == 0) {
		this->obj_ub = -1;
	}

	this->n_solve_calls += 1;

	printf("depth: %d, calls: %d, ub: %d \r", depth, this->n_solve_calls, this->obj_ub);
	fflush(stdout);
	
	int obj = 0;
	Res_col res_col;
	int res_cons_idx;


	int order_ret = this->graph.make_order(this->order, res_col, obj);

	if (order_ret == TRAIN_UNFINISHED || order_ret == UB_REACHED) {
		return;
	}

	if (obj >= this->obj_ub && this->obj_ub >= 0) {
		return;
	}

	if (order_ret == FEASIBLE) {
		this->obj_ub = obj;
		this->save_solution();
		return;
	}




	auto old_path = this->graph.path;
	auto old_state = this->graph.state;

	if (this->graph.reroute_path(res_col.first.lock, res_col.first.unlock)) {
		this->solve(depth + 1);
	}

	this->graph.path = old_path;
	this->graph.state = old_state;

	if (this->graph.reroute_path(res_col.second.lock, res_col.second.unlock)) {
		this->solve(depth + 1);
	}

	this->graph.path = old_path;
	this->graph.state = old_state;

	assert(this->graph.lock_path(res_col.first.unlock));
	assert(this->graph.lock_path(res_col.second.unlock));


	this->graph.extend_res_col(res_col);

	// print_res_col(res_col, 0);
	res_cons_idx = graph.add_res_cons(res_col, false);
	this->solve(depth+1);
	this->graph.remove_last_res_cons(res_col, false, res_cons_idx);

	// print_res_col(res_col, 1);
	res_cons_idx = graph.add_res_cons(res_col, true);
	this->solve(depth+1);
	this->graph.remove_last_res_cons(res_col, true, res_cons_idx);
	// print_res_col(res_col, -1);

	this->graph.state = old_state;
}


void Tree::save_solution()
{
	this->best_sol.clear();
	this->best_sol.reserve(this->order.size());

	for (int o : this->order) {
		best_sol.push_back({o, this->graph.time[o]});
	}
}




