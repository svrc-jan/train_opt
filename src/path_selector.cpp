#include "path_selector.hpp"

#include <cstdio>

Path_selector::Path_selector(const Instance& inst) : inst(inst)
{

}


Path_selector::~Path_selector()
{

}


struct Prio_op
{
	int op;
	double prio;

	bool operator<(const Prio_op& other) const { return this->prio > other.prio; }
};


void Path_selector::select_all_paths_by_res_imp(vector<vector<int>>& paths)
{
	vector<double> op_imp;
	this->get_op_importance(op_imp);
	
	vector<double> res_imp;
	this->get_res_importance(res_imp, op_imp);

	paths.resize(this->inst.n_trains());
	for (int t = 0; t < inst.n_trains(); t++) {
		this->select_path_by_res_imp(paths[t], t, res_imp);
	}
}


bool Path_selector::select_path_by_res_imp(vector<int>& path, const int train_idx,
	vector<double>& res_imp)
{
	auto& train = this->inst.trains[train_idx];

	int n_train_ops = train.ops.size;

#ifndef NO_VLA
	double op_cost[n_train_ops];
#else
	vector<double> op_cost(n_train_ops);
#endif

	for (int i = 0; i < train.ops.size; i++) {
		double cost = 0;
		auto& op = this->inst.ops[i + n_train_ops];
 
		for (auto& res : op.res) {
			cost += res_imp[res.idx];
		}
		op_cost[i] = cost;
	}

#ifndef NO_VLA
	return this->select_path(path, train_idx, op_cost, n_train_ops);
#else
	return this->select_path(path, train_idx, op_cost);
#endif
}


bool Path_selector::select_path(vector<int>& path, const int train_idx,
	const double op_costs[], int n_train_ops)
{
	assert(train_idx >= 0 && train_idx < this->inst.n_trains());

	auto& train = this->inst.trains[train_idx];
	assert(n_train_ops == train.ops.size && n_train_ops > 0);

	int op_offset = train.op_start;

	priority_queue<Prio_op> prio_queue;
#ifndef NO_VLA
	int op_pred[n_train_ops];
	double op_dist[n_train_ops];

	for (int i = 0; i < n_train_ops; i++) {
		op_pred[i] = -1;
	}

	for (int i = 0; i < n_train_ops; i++) {
		op_dist[i] = PATH_SELECT_MAX;
	}
#else
	vector<int> op_pred(n_train_ops, -1);
	vector<double> op_dist(n_train_ops, PATH_SELECT_MAX);
#endif


	op_dist[0] = 0.0;
	prio_queue.push({
		.op = 0,
		.prio = 0.0
	});

	while (!prio_queue.empty()) {
		auto curr = prio_queue.top();
		prio_queue.pop();

		int i = curr.op;
		double dist = curr.prio;

		if (dist > op_dist[i]) {
			continue;
		}

		auto& op = this->inst.ops[i + op_offset];
		double succ_dist = dist + op_costs[i];

		for (int s : op.succ) {
			int j = s - op_offset;

			if (succ_dist < op_dist[j]) {
				op_dist[j] = succ_dist;
				op_pred[j] = i;

				prio_queue.push({
					.op = j,
					.prio = succ_dist
				});
			}
		}
	}	

	path.clear();
	path.push_back(train.op_last());
	
	while (path.back() != train.op_start) {
		int p = op_pred[path.back() - op_offset];
		if (p < 0) {
			return false; // path not found
		}
		path.push_back(p + op_offset);
	}
	
	reverse(path.begin(), path.end());
	return true;
}


void Path_selector::get_op_importance(vector<double>& op_imp)
{
	op_imp.resize(this->inst.n_ops());

	vector<int> st;

#ifndef NO_VLA
	double forw_imp[this->inst.max_n_train_ops];
	double backw_imp[this->inst.max_n_train_ops];
	int in_count[this->inst.max_n_train_ops];
#else
	vector<double> forw_imp(this->inst.max_n_train_ops);
	vector<double> backw_imp(this->inst.max_n_train_ops);
	vector<int> in_count(this->inst.max_n_train_ops);
#endif

	for (auto& train : this->inst.trains) {
		int n_train_ops = train.ops.size;
		int op_offset = train.op_start;

		for (int i = 0; i < n_train_ops; i++) {
			forw_imp[i] = (i == 0) ? 1.0 : 0.0;
		}
	
		for (int i = 0; i < n_train_ops; i++) {
			auto& op = this->inst.ops[i + op_offset];
			in_count[i] = op.pred.size;
		}

		st.push_back(0);

		while (st.size() > 0) {
			int i = st.back();
			st.pop_back();

			auto& op = this->inst.ops[i + op_offset];
			for (int s : op.succ) {
				int j = s - op_offset;
				forw_imp[j] += forw_imp[i]/(double)op.succ.size;

				in_count[j] -= 1;
				if (in_count[j] == 0) {
					st.push_back(j);
				}
			}
		}

		for (int i = 0; i < n_train_ops; i++) {
			backw_imp[i] = (i == n_train_ops - 1) ? 1.0 : 0.0;
		}
	
		for (int i = 0; i < n_train_ops; i++) {
			auto& op = this->inst.ops[i + op_offset];
			in_count[i] = op.succ.size;
		}

		st.push_back(n_train_ops - 1);

		int i, j;
		while (st.size() > 0) {
			i = st.back();
			st.pop_back();

			auto& op = this->inst.ops[i + op_offset];
			for (int p : op.pred) {
				j = p - op_offset;
				backw_imp[j] += backw_imp[i]/(double)op.pred.size;

				in_count[j] -= 1;
				if (in_count[j] == 0) {
					st.push_back(j);
				}
			}
		}

		for (int i = 0; i < n_train_ops; i++) {
			op_imp[i + op_offset] = (backw_imp[i] + forw_imp[i])/2;
			// assert(abs(backw_imp[i] - forw_imp[i]) < 1e-6);
			// printf("%.2f:%.2f%s", backw_imp[i], forw_imp[i], ((i == n_train_ops - 1) ? "\n" : ", "));
		}
	}
}

void Path_selector::get_res_importance(vector<double>& res_imp, const vector<double>& op_imp)
{
	res_imp.resize(this->inst.n_res());
	for (int i = 0; i < this->inst.n_res(); i++) {
		res_imp[i] = 0.0;
	}

	for (int i = 0; i < this->inst.n_ops(); i++) {
		auto& op = this->inst.ops[i];
		for (auto& res : op.res) {
			res_imp[res.idx] += op_imp[i];
		}
	}
}

