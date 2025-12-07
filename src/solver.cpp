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
	while (true) {
		iter += 1;
		if (!graph.make_order()) {
			println("make order failed");
			return false;
		}

		if (!graph.make_time()) {
			// println("make time failed");
			// return false;
		}

		pair<Graph::Node_ordering, Graph::Node_ordering> node_ords;
		if (!graph.find_res_col(node_ords)) {
			break;
		}

		Graph::Node_ordering ord;
		int train1 = this->node_train[node_ords.first.node_from];
		int train2 = this->node_train[node_ords.second.node_from];
		if (prio[train1] > prio[train2]) {
			ord = node_ords.first;
		}
		else {
			ord = node_ords.second;
		}

		// this->graph.extend_node_ordering(ord);

#if defined STATIC_LOG_LEVEL && STATIC_LOG_LEVEL >= 100
		println("add {} -({})> {} (t{} -> t{})",
			ord.node_from,
			ord.res_time,
			ord.node_to,
			this->node_train[ord.node_from],
			this->node_train[ord.node_to]
		);
#endif

		graph.add_res_cons(ord);
	}

	println("solved in {} iter", iter);

	return true;
}


