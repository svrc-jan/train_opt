#include "graph.hpp"

Graph::Graph() {}

Graph::~Graph() {}


void Graph::add_train(const Train& train)
{
	Node_interval node_int = {
		.op_begin = train.op_begin,
		.op_end = train.op_begin + train.n_ops(),
		.node_begin = this->n_nodes(),
	};



	for (const Op& op : train.ops) {
		Node node = {
			.op = op.idx,
			.dur = op.dur,
			.start_lb = op.start_lb
		};

		for (int s : op.succ) {
			node.path_out.size++;
			this->node_path_out.push_back({
				.node = node_int.op_to_node(s),
				.active = true
			});
		}
		this->nodes.push_back(node);
	}

	this->node_intervals.push_back(node_int);
	sort(this->node_intervals.begin(), this->node_intervals.end());

	this->assign_path_out_pointers();
}


void Graph::assign_path_out_pointers()
{
	int idx = 0;
	for (Node& node : this->nodes) {
		if (node.path_out.size > 0) {
			node.path_out.ptr = &(this->node_path_out[idx]);
			idx += node.path_out.size;
		}
	}
	assert(idx == (int)this->node_path_out.size());
}


const Node_interval* Graph::find_node_interval(int op_id)
{
	return binary_search_node_interval(op_id, 0, (int)this->node_intervals.size());
}


const Node_interval* Graph::binary_search_node_interval(int op_id, int left, int right)
{
	if (left >= right - 1) {
		const Node_interval* node_int = &(this->node_intervals[left]);
		if (node_int->is_op_in(op_id)) {
			return node_int;
		}
		else {
			return nullptr;
		}
	}

	int mid = (left + right)/2;

	if (op_id < this->node_intervals[mid - 1].op_end) {
		return this->binary_search_node_interval(op_id, left, mid);
	}
	else if (op_id >= this->node_intervals[mid].op_begin) {
		return this->binary_search_node_interval(op_id, mid, right);
	}
	else {
		return nullptr;
	}
}