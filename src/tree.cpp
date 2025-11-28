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



bool Tree::solve(int depth)
{
	this->n_solve_calls += 1;

	printf("depth: %04d, calls: %05d\r", depth, this->n_solve_calls);
	fflush(stdout);
	
	Res_col res_col;
	int res_cons_idx;

	if (this->graph.make_order(res_col)) {
		return true;
	}

	this->graph.extend_res_col(res_col);

	// print_res_col(res_col, 0);
	res_cons_idx = graph.add_res_cons(res_col, false);
	if (this->solve(depth+1)) {
		return true;
	}
	this->graph.remove_last_res_cons(res_col, false, res_cons_idx);

	// print_res_col(res_col, 1);
	res_cons_idx = graph.add_res_cons(res_col, true);
	if (this->solve(depth+1)) {
		return true;
	}
	this->graph.remove_last_res_cons(res_col, true, res_cons_idx);
	// print_res_col(res_col, -1);
	
	return false;
}





