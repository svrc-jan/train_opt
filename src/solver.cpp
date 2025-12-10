#include "solver.hpp"


using namespace std;

Solver::Solver(Graph& graph)
	:  inst(graph.inst), prepr(graph.prepr), graph(graph)
{
	node_train.resize(this->prepr.n_levels());
	for (int t = 0; t < this->prepr.n_trains(); t++) {
		auto& train = this->prepr.trains[t];

		for (int l = train.level_start; l < train.level_end(); l++) {
			this->node_train[l] = t;
		}
	}
}


bool Solver::solve_with_train_prio(const vector<double>& prio)
{
	int iter = 0;
	println("solved in {} iter", iter);

	return true;
}


