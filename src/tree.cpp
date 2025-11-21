#include "tree.hpp"

#include "iostream"
#include "graph.hpp"


Tree::Tree(const Instance& inst) : inst(inst)
{
	
}

bool Tree::solve()
{
	Graph graph(this->inst);

	vector<int> order;
	vector<int> time;
	vector<int> op_in;
	vector<int> op_out;

	int it = 0;
	while (true) {
		if (!graph.get_order(order, time, op_in)) {
			cout << "not solved" << endl;
			return false;
		}

		make_op_out(op_out, op_in);
		Res_col rc;
		
		if (!this->get_res_col(rc, order, time, op_in, op_out)) {
			break;
		}

		// cout << "adding: " << rc.first.end << " -(" << rc.first.res_time << ")-> " << rc.second.start << endl;

		graph.add_res_cons(rc.first.end, rc.second.start, rc.first.res_time);
		it += 1;
	}

	cout << "solved in " << it << " iter" << endl;

	return true;
}


void Tree::make_op_out(vector<int>& op_out, const vector<int>& op_in)
{
	op_out.resize(this->inst.n_levels());
	memset(op_out.data(), -1, sizeof(int)*this->inst.n_levels());

	for (auto& train : this->inst.trains) {

		int curr = train.level_last();
		op_out[curr] = -2;

		while (curr != train.level_begin) {
			int o = op_in[curr];
			int prev = this->inst.ops[o].level_start;
			
			op_out[prev] = o;
			curr = prev;
		}
	}
}


bool Tree::get_res_col(Res_col& result, const vector<int>& order, const vector<int>& time,
	const vector<int>& op_in, const vector<int>& op_out)
{
	vector<Res_col> res_col(this->inst.n_res);
	memset(res_col.data(), -1, sizeof(Res_col)*this->inst.n_res);

	for (int l : order) {

		int i = op_in[l];
		int j = op_out[l];

		if (j == -1) {
			continue;
		}

		int t = this->inst.levels[l].train;

		if (i >= 0) {
			auto& op = this->inst.ops[i];
			for (auto& res : op.res) {
				Res_col& rc = res_col[res.idx];
				if (rc.first.train == t) {
					if (rc.second.train != -1) {
						rc.first.end = l;
						rc.first.res_time = res.time;
					}
					else {
						rc.first = {
							.train = -1,
							.start = -1,
							.end = -1,
							.res_time = -1
						};
					}
					
				}
				else if (rc.second.train == t) {
					rc.second.end = l;
					rc.second.res_time = res.time;
				}
			}
		}
		
		if (j >= 0) {
			auto& op = this->inst.ops[j];
			for (auto& res : op.res) {
				Res_col& rc = res_col[res.idx];
				if (rc.first.train == -1) {
					rc.first.train = t;
					rc.first.start = l;
				}
				else if (rc.second.train == -1 && rc.first.train != t) {
					rc.second.train = t;
					rc.second.start = l;
				}
			}
		}
	}

	int select_res = -1;
	int select_time = MAX_INT;

	for (int r = 0; r < this->inst.n_res; r++) {
		Res_col& rc = res_col[r];

		if (rc.first.train == -1 || rc.second.train == -1) {
			continue;
		}

		assert(rc.first.train != rc.second.train);

		int rc_time = max(time[rc.first.start], time[rc.second.start]);
		assert(min(time[rc.first.end], time[rc.second.end]) >= rc_time);

		if (rc_time < select_time) {
			select_time = rc_time;
			select_res = r;
		}
	}

	if (select_res >= 0) {
		result = res_col[select_res];
		return true;
	}

	return false;
}
