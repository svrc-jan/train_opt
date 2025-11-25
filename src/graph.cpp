#include "graph.hpp"

#include <algorithm>
#include <queue>


struct Order_item
{
	int op;
	int time;
};


class Order_comparator
{
public:
	bool operator()(const Order_item& a, const Order_item& b) {
		return a.time > b.time;
	}
};

using Order_prio_queue = priority_queue<Order_item, vector<Order_item>, Order_comparator>;


Graph::Graph(const Instance& inst) : inst(inst)
{
	this->make_chrono_order();
}


void Graph::make_chrono_order()
{	
	Order_prio_queue pq;

	this->op_chrono.set_size(this->inst.n_ops());
	this->res_chrono.set_size(this->inst.n_res());

	for (auto& train : this->inst.trains) {
		auto& start_op = train.ops[0];

		pq.push({
			.op = train.op_begin,
			.time = start_op.start_lb + start_op.dur
		});
	}
	
	while (!pq.empty()) {
		auto curr = pq.top(); pq.pop();
		if (this->op_chrono.add(curr.op)) {
			for (int s : this->inst.ops[curr.op].succ) {
				auto& succ_op = this->inst.ops[s];

				for (auto& res : succ_op.res) {
					this->res_chrono.add(res.idx);
				}

				pq.push({
					.op = s,
					.time = max(curr.time, succ_op.start_lb) + succ_op.dur
				});
			}
		}
	}

	assert(this->op_chrono.is_done() && this->res_chrono.is_done());
}


void Graph::make_op_data()
{
	for (int o : this->op_chrono.order) {

	}
}
