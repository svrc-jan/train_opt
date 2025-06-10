#include "solution.hpp"

Solution::Solution(const Instance& inst, Rand_int_gen& rng)
	: inst(inst), rng(rng)
{
	this->paths = vector<op_order_t>(inst.n_train);
}


op_order_t& Solution::make_random_path(size_t train)
{
	op_order_t& path = this->paths[train];
	path.clear();

	path.push_back({-1, inst.trains[train].begin_idx});

	while (true) {
		int op_idx = path.back().second;
		const Operation* op = &inst.ops[op_idx];

		if (op->n_succ == 0) {
			// path.push_back({op_idx, -1});
			break;
		}
		
		else if (op->n_succ == 1) {
			path.push_back({op_idx, op->succ[0]});
		}

		else {
			path.push_back({op_idx, op->succ[rng(op->n_succ)]});
		}
	}

	return path;
}


vector<op_order_t>& Solution::make_all_random_paths()
{
	for (int train = 0; train < inst.n_train; train++) {
		this->make_random_path(train);
	}

	return this->paths;
}

std::map<int, int>& Solution::make_earliest_start()
{
	this->earliest_start.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const op_order_t& path = this->paths[train];

		for (const auto& [prev, op] : path) {
			if (op == -1) {
				break;
			}
			
			else if (prev == -1) {
				this->earliest_start[op] = inst.ops[op].start_lb;
			}

			else {
				this->earliest_start[op] = std::max(
					inst.ops[op].start_lb,
					this->earliest_start[prev] + inst.ops[prev].dur
				);
			}
		}

	}

	return this->earliest_start;
}


std::map<int, int>& Solution::make_latest_start()
{
	this->latest_start.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const op_order_t& path = this->paths[train];
		
		for (size_t i = 0; i < path.size(); i++) {
			int op = path[i].second;
			this->latest_start[op] = inst.ops[op].start_ub;

			if (inst.ops[op].obj == nullptr) {
				continue;
			}

			if (inst.ops[op].obj->threshold >= this->latest_start[op]) {
				continue;
			}

			this->latest_start[op] = inst.ops[op].obj->threshold;

			for (size_t j = i; path[j].first >= 0; j--) {
				int start_push = 
					this->latest_start[path[j].second] -
					inst.ops[path[j].first].dur;

				if (start_push >= this->latest_start[path[j].first]) {
					break;
				}

				this->latest_start[path[j].first] = start_push;
			}
		}
	}

	return this->latest_start;
}


op_order_t& Solution::make_order()
{
	this->order.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const op_order_t& path = this->paths[train];

		order.insert(order.begin(), path.begin(), path.end());
	}
	
	std::sort(this->order.begin(), this->order.end(), 
		[this](const pair<int, int>& a, const pair<int, int>& b) {
			int prio_a = 
				this->earliest_start[a.second] + 
				this->latest_start[a.second];
			
			int prio_b = 
				this->earliest_start[b.second] + 
				this->latest_start[b.second];

			if (prio_a == prio_b) {
				return (a.second < b.second);
			}

			return (prio_a < prio_b);
		}
	);

	return this->order;
}





void Solution::print_order(std::ostream& os) const
{
	vector<std::string> vs;

	char buffer[256];

	for (const auto& [prev, op] : this->order) {
		
		if (prev == -1) {
			sprintf(buffer, "(%d: %d)", 
				inst.ops[op].train_id, 
				inst.ops[op].op_id
			);
		}
		
		else {
			sprintf(buffer, "(%d: %d -> %d)", 
				inst.ops[op].train_id, 
				inst.ops[prev].op_id,
				inst.ops[op].op_id
			);
		}

		vs.push_back(buffer);
	}

	os << vs;
}

std::ostream& operator<< (std::ostream& os, const Solution& sol)
{
	sol.print_order(os);
	return os;
}