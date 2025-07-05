#include "solution.hpp"

#define first_op(order, idx) this->inst.ops[order[idx].first]
#define second_op(order, idx) this->inst.ops[order[idx].second]


Solution::Solution(const Instance& inst, Rand_int_gen& rng)
	: inst(inst), rng(rng)
{
	this->paths = vector<vector<pair<int, int>>>(inst.n_train);
}


const vector<pair<int, int>>& Solution::make_random_path(size_t train)
{
	vector<pair<int, int>>& path = this->paths[train];
	path.clear();

	while (true) {
		int op_idx = path.empty() ? 
			inst.trains[train].begin_idx : 
			path.back().second;

		const Operation& op = inst.ops[op_idx];

		if (op.n_succ == 0) {
			// path.push_back({op_idx, -1});
			break;
		}
		
		else if (op.n_succ == 1) {
			path.push_back({op_idx, op.succ[0]});
		}

		else {
			path.push_back({op_idx, op.succ[rng(op.n_succ)]});
		}
	}

	return path;
}


const vector<vector<pair<int, int>>>& Solution::make_all_random_paths()
{
	for (int train = 0; train < inst.n_train; train++) {
		this->make_random_path(train);
	}

	this->path_inserted = vector<bool>(inst.n_train, false);

	return this->paths;
}

const std::map<int, int>& Solution::make_earliest_start()
{
	this->earliest_start.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const vector<pair<int, int>>& path = this->paths[train];

		int start_op = inst.trains[train].begin_idx;
		this->earliest_start[start_op] = inst.ops[start_op].start_lb;

		for (const auto& [prev, op] : path) {
			this->earliest_start[op] = std::max(
				inst.ops[op].start_lb,
				this->earliest_start[prev] + inst.ops[prev].dur);
		}

	}

	return this->earliest_start;
}


const std::map<int, int>& Solution::make_latest_start()
{
	this->latest_start.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const vector<pair<int, int>>& path = this->paths[train];
		
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

			for (int j = i; j >= 0; j--) {
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


const list<pair<int, int>>& Solution::make_order()
{
	this->order.clear();

	for (int train = 0; train < inst.n_train; train++) {
		const vector<pair<int, int>>& path = this->paths[train];

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

			if (prio_a != prio_b) {
				return (prio_a < prio_b);
			}

			prio_a = 
				this->earliest_start[a.first] + 
				this->latest_start[a.first];
			
			prio_b = 
				this->earliest_start[b.first] + 
				this->latest_start[b.first];

			if (prio_a != prio_b) {
				return (prio_a < prio_b);
			}

			return (a.second < b.second);
		}
	);

	return this->order;
}


int Solution::count_collisions(const list<pair<int, int>>& ord) const
{
	vector<int> res_vec(inst.n_res, 1);

	for (const auto& train : inst.trains) {
		res_vec -= train[0];
	}

	int count = 0;

	for (const auto& [first, second] : ord) {
		res_vec += inst.ops[first];

		for (const auto& res : inst.ops[second].res) {
			count += (res_vec[res.id] <= 0);
			
		}

		res_vec -= inst.ops[second];
	}

	return count;
}


int Solution::get_res_overlap_diff(
	const std::pair<int, int>& a,
	const std::pair<int, int>& b) const
{
	return
		inst.get_res_overlap(a.first, b.second) - 
		inst.get_res_overlap(b.first, a.second);
}


bool Solution::forward_reorder()
{
	
	int changed = false;

	list<pair<int, int>>& ord = this->order;
	std::cout << "forward reorder col: " << this->count_collisions(ord) << std::endl;
	
	
	return changed;
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

vector<int>& operator-=(vector<int>& vec, const Operation& op)
{
	for (const auto& res : op.res) {
		vec[res.id] -= 1;
	}

	return vec;
}

vector<int>& operator+=(vector<int>& vec, const Operation& op)
{
	for (const auto& res : op.res) {
		vec[res.id] += 1;
	}

	return vec;
}