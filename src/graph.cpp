#include "graph.hpp"

#include <iostream>
#include <queue>

Graph::Graph(const Instance& inst) : inst(inst), n_ops(inst.n_ops()), n_res(inst.n_res())
{
	this->nodes.resize(this->n_ops);
	this->n_pred.resize(this->n_ops);
}

void Graph::make_order()
{
	
}