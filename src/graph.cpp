#include "graph.hpp"

Graph::Graph(const Instance& inst) : inst(inst)
{
	this->nodes.reserve(this->inst.n_levels());
	this->in_degree.reserve(this->inst.n_levels());

	for (const Level& level : this->inst.levels) {
		Node node;

		assert(level.n_ops_out() <= 32);
		for (int i = 0; i < level.n_ops_out(); i++) {
			node.viable_out_flag |= (1U << i);
		}

		this->nodes.push_back(node);
		this->in_degree.push_back(level.n_ops_in());
	}
}



vector<int> Graph::get_order(vector<int>& time, vector<const Op *>& in_op)
{
	vector<int> in_count = this->in_degree;
	vector<int> res_time(this->n_nodes(), 0);

	priority_queue<Node_time> q;

	time.resize(this->n_nodes());
	in_op.resize(this->n_nodes());

	for (int i = 0; i < this->n_nodes(); i++) {
		time[i] = MAX_INT;
		in_op[i] = nullptr;

		if (in_count[i] == 0) {
			const Level& level = this->inst.levels[i];
			
			time[i] = level.ops_out[0]->start_lb;
			q.push({
				.node = level.idx,
				.time = time[i]
			});
		}
	}

	vector<int> order;
	order.reserve(this->n_nodes());

	while (!q.empty()) {
		Node_time curr = q.top(); q.pop();

		order.push_back(curr.node);

		const Node& node = this->nodes[curr.node];
		const Level& level = this->inst.levels[curr.node];
		
		for (int i = 0; i < level.n_ops_out(); i++) {
			if ((node.viable_out_flag & (1U << i)) == 0U) {
				continue;
			}

			const Op* op = level.ops_out[i];
			int succ_idx = op->level_end->idx;

			int op_end = max(curr.time, op->start_lb) + op->dur;			
			if (op_end < time[succ_idx]) {
				time[succ_idx] = max(curr.time, op->start_lb) + op->dur;
				in_op[succ_idx] = op;
			}

			in_count[succ_idx] -= 1;
			if (in_count[succ_idx] == 0) {
				time[succ_idx] = max(time[succ_idx], res_time[succ_idx]);

				q.push({
					.node = succ_idx,
					.time = time[succ_idx]
				});
			}
		}

		for (const Node_time& res_edge : node.res_edges) {
			res_time[res_edge.node] = max(res_time[res_edge.node], curr.time + res_edge.time);
			
			in_count[res_edge.node] -= 1;
			if (in_count[res_edge.node] == 0) {
				time[res_edge.node] = max(time[res_edge.node], res_time[res_edge.node]);

				q.push({
					.node = res_edge.node,
					.time = time[res_edge.node]
				});
			}
		}
	}

	assert((int)order.size() == this->n_nodes());

	return order;
}



vector<Event> Graph::get_events()
{
	vector<int> time(this->n_nodes());
	vector<const Op*> in_op(this->n_nodes());
	vector<const Op*> out_op(this->n_nodes(), nullptr);

	vector<int> order = this->get_order(time, in_op);

	int events_size = 0;
	for (const Train& train: this->inst.trains) {
		int curr_l = train.level_begin + train.n_levels() - 1;
		while (curr_l != train.level_begin) {
			const Op* op = in_op[curr_l];
			assert(op != nullptr);

			int prev_l = op->level_start->idx;
			out_op[prev_l] = op;

			curr_l = prev_l;

			events_size += 1;
		}
		events_size += 1;
	}

	vector<Event> events;
	events.reserve(events_size);

	for (int l : order) {
		if (out_op[l] != nullptr || this->inst.levels[l].n_ops_out() == 0) {
			if (in_op[l] == nullptr) {
				assert(out_op[l]->n_prev() == 0);
			}

			if (out_op[l] == nullptr) {
				assert(in_op[l]->n_succ() == 0);
			}

			if (!events.empty()) {
				assert(events.back().time <= time[l]);
			}

			events.push_back({
				.in_op = in_op[l],
				.out_op = out_op[l],
				.time = time[l],
			});
		}
	}

	return events;
}



std::ostream& operator<<(std::ostream& stream, const Node_time& node_time)
{
	stream << "(" << node_time.node << ", " << node_time.time << ")";
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Event& event)
{
	stream << "Event("; 

	if (event.in_op != nullptr) {
		stream << "in=" << event.in_op->idx << ", ";
	}

	if (event.out_op != nullptr) {
		stream << "out=" << event.out_op->idx << ", ";
	}

	stream << "time=" << event.time << ")";

	return stream;
}

