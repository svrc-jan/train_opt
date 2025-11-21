#include "graph.hpp"

#include <algorithm>
#include <queue>




Graph::Graph(const Instance& inst) : inst(inst)
{
	this->path.resize(this->inst.n_ops(), 0);
	this->res_cons.resize(this->inst.n_ops(), vector<Op_time>());
	this->n_in_res_cons.resize(this->inst.n_ops(), 0);
}


struct Transition
{
	int op_in;
	int op_out;
	int time;
};

class Transition_comparator
{
public:
	bool operator()(const Transition& a, const Transition& b) {
		return a.time > b.time;
	}
};


struct Res_lock
{
	int train = -1;
	int op = -1;
	int time = -1;
};


enum Op_state {
	OP_WAITING,
	OP_RUNNING,
	OP_ENDED
};


bool Graph::get_col(Res_col& col)
{
	priority_queue<Transition, vector<Transition>, Transition_comparator> pq;

	vector<int> state(this->inst.n_ops(), OP_WAITING);
	vector<int> res_cons_done(this->inst.n_ops(), 0);

	vector<Res_lock> res_lock(this->inst.n_res, {-1, -1, -1});
	
	for (auto& train : this->inst.trains) {
		pq.push({
			.op_in = -1,
			.op_out = train.op_begin,
			.time = train.ops[0].start_lb
		});
	}

	while (!pq.empty()) {
		Transition curr = pq.top(); pq.pop();
	}

}
