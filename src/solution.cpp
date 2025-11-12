#include "solution.hpp"

void Sol_graph::add(int from, int to, int d)
{
	this->prev[to].push_back(from);
	this->succ[from].push_back(to);
	this->dur[{from, to}] = d;
}

inline void remove_from_vec(std::vector<int>& v, const int x)
{
	auto it = v.end();
	while (it != v.begin()) {
		it--;
		if (*it == x) {
			v.erase(it);
			return;
		}
	};
}

void Sol_graph::remove(int from, int to)
{
	remove_from_vec(this->prev[to], from);
	remove_from_vec(this->prev[from], to);
	this->dur.erase({from, to});
}

Solution::Solution(const Instance& inst, Rand_int_gen& rng)
	: inst(inst), rng(rng)
{
	
}


void Solution::make_random_paths()
{
	this->v_op.clear();
	this->mp_res_use.clear();

	for (auto& train : this->inst.v_train) {
		this->v_op.push_back(train.begin);

		int from = train.begin;

		for (int res : this->get_op(from).v_res) {
			this->mp_res_use[res].push_back({
				.lock = from
			});
		}

		while (true) {
			const Op& op = this->get_op(from);
			int to = this->rng(op.v_succ);

			this->v_op.push_back(to);
			this->graph.add(from, to, op.dur);

			const pair<vector<int>, vector<int>>& res_diff = this->inst.res_diff(from, to);

			for (int res : res_diff.first) {
				Res_use& res_use = this->mp_res_use[res].back();
				res_use.unlock = to;
				res_use.time = this->inst.res_time(res, to);
			}

			for (int res : res_diff.second) {
				this->mp_res_use[res].push_back({
					.lock = to
				});
			}
			
			from = to;
		}
	}
}