#include "tree.hpp"

#include <queue>
#include <cstdio>

Tree::Tree(const Instance& inst) : inst(inst), graph(Graph(inst))
{
	this->n_solve_calls = 0;
}

void Tree::solve(int depth)
{
	
}