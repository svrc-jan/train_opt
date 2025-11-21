#include "graph.hpp"

#include <algorithm>
#include <queue>




Graph::Graph(const Instance& inst) : inst(inst)
{
	this->res_cons.resize(this->inst.n_levels(), vector<Res_cons>());
	this->n_in_res_cons.resize(this->inst.n_levels(), 0);
}


struct Order_item
{
	int time;
	int node;
	int op;
};

class Order_comparator
{
public:
	bool operator()(const Order_item& a, const Order_item& b) {
		return a.time > b.time;
	}
};

bool Graph::get_order(vector<int>& order, vector<int>& time, vector<int>& op_in)
{	

	order.clear();
	order.reserve(this->inst.n_levels());

	time.resize(this->inst.n_levels());
	
	op_in.resize(this->inst.n_levels());
	memset(op_in.data(), -1, sizeof(int)*this->inst.n_levels());

	vector<int> res_cons_done(this->inst.n_levels(), 0);
	
	priority_queue<Order_item, vector<Order_item>, Order_comparator> pq;

	for (auto& train : this->inst.trains) {
		pq.push({
			.time = train.ops[0].start_lb,
			.node = train.level_begin,
			.op = -2
		});
	}

	while (!pq.empty()) {
		Order_item curr = pq.top(); pq.pop();

		if (curr.op != -1) { // path update
			if (op_in[curr.node] == -1) {
				op_in[curr.node] = curr.op; 
			}
			else {
				continue;
			}
		}

		if (op_in[curr.node] == -1 || res_cons_done[curr.node] < this->n_in_res_cons[curr.node]) {
			continue;
		}

		order.push_back(curr.node);
		time[curr.node] = curr.time;
		
		
		for (int o : this->inst.levels[curr.node].ops_out) {
			auto& op = this->inst.ops[o];

			if (op_in[op.level_end] != -1) {
				continue;
			}

			pq.push({
				.time = max(curr.time, op.start_lb) + op.dur,
				.node = op.level_end,
				.op = o
			});
		}

		for (auto& rc : this->res_cons[curr.node]) {
			res_cons_done[rc.node] += 1;
			pq.push({
				.time = curr.time + rc.time,
				.node = rc.node,
				.op = -1
			});
		}
	}
	
	return (int)order.size() == this->inst.n_levels();
}


void Graph::add_res_cons(int from, int to, int time)
{
	this->res_cons[from].push_back({
		.node = to,
		.time = time
	});

	this->n_in_res_cons[to] += 1;
}
