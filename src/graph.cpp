#include "graph.hpp"

#include <iostream>


Graph::Graph(const Instance& inst) : inst(inst)
{
	this->n_ops = this->inst.n_ops();
	this->n_res = this->inst.n_res();

	this->time.resize(this->n_ops);
	this->state.resize(this->n_ops, OP_VALID);
	this->path_prev.resize(this->n_ops, -1);
	this->path_succ.resize(this->n_ops, -1);

	for (auto& train : this->inst.trains) {
		this->update_path(train.op_start, train.ops[0].start_lb);
	}

	this->n_res_cons.resize(this->n_ops, 0); 
	this->res_cons_idx.resize(this->n_ops, -1);

	this->n_pred.resize(this->n_ops);
	this->res_locks.resize(this->n_res);
}


bool Graph::update_path(const int op, const int op_time)
{
	auto& train = this->inst.trains[this->inst.ops[op].train];

	for (int i = train.op_start; i <= train.op_last(); i++) {
		this->time[i] = MAX_INT;
	}

	this->time[op] = op_time;
	this->prio_queue.push(op, this->time[op]);

	while (!this->prio_queue.empty()) {
		auto curr = this->prio_queue.top();
		this->prio_queue.pop();

		int o = curr.value;
		int t = curr.prio;

		if (t > this->time[o]) {
			continue;
		}

		auto& curr_op = this->inst.ops[o];
		for (int s : curr_op.succ) {
			if (!this->state[s]) {
				continue;
			}

			int alt_time = max(this->inst.ops[s].start_lb, time[o] + curr_op.dur);
			if (alt_time < time[s]) {
				time[s] = alt_time;
				this->prio_queue.push(s, time[s]);
			}
		}
	}

	
	int prev;
	int prev_time;
	int curr = train.op_last();
	this->path_succ[curr] = -2;

	while (curr != op) {
		prev_time = MAX_INT;
		prev = -1;
		
		for (int p : this->inst.ops[curr].prev) {
			if (time[p] < prev_time) {
				prev_time = time[p];
				prev = p;
			}
		}

		if (prev == -1) {
			return false;
		}

		this->path_prev[curr] = prev;
		this->path_succ[prev] = curr;
		curr = prev;
	}

	return true;
}


bool Graph::make_order(Res_col& res_col)
{
	memset(this->time.data(), 0, sizeof(int)*this->n_ops);
	memset(this->n_pred.data(), 0, sizeof(int)*this->n_ops);
	
	vector<bool> added(this->n_ops, false);

	for (int r = 0; r < this->n_res; r++) {
		this->res_locks[r] = {-1, -1, -1, -1};
	}

	this->prio_queue.clear();
	for (auto& train : this->inst.trains) {
		int o = train.op_start;
		this->time[o] = this->inst.ops[o].start_lb,

		this->n_pred[o] += 1;
		if (this->n_pred[o] >= this->n_res_cons[o] + 1) {
			assert(!added[o]);
			added[o] = true;

			this->prio_queue.push(o, this->time[o]);
		}
	}

	while (!this->prio_queue.empty()) {
		auto curr = this->prio_queue.top();
		this->prio_queue.pop();

		int o = curr.value;
		int t = curr.prio;

		assert(t == this->time[o]);
		
		int o_prev = this->path_prev[o];
		if (o_prev >= 0) {
			auto& op_prev = this->inst.ops[o_prev];
			
			for (auto& res : op_prev.res) {
				auto& rl = this->res_locks[res.idx];
				assert(rl.unlock == o_prev);
				rl.time = t + res.time;
			}

			int rc_idx = res_cons_idx[o_prev];
			while (rc_idx >= 0)	{
				const Res_cons& rc = this->res_cons[rc_idx];
				this->time[rc.op] = max(time[rc.op], t + rc.time);

				this->n_pred[rc.op] += 1;
				if (this->n_pred[rc.op] >= this->n_res_cons[rc.op] + 1) {
					assert(!added[rc.op]);
					added[rc.op] = true;

					this->prio_queue.push(rc.op, this->time[rc.op]);
				}
				rc_idx = rc.next_idx;
			}	
		}

		auto& op = this->inst.ops[o];
		int o_succ = this->path_succ[o];
		int t_end = t + op.dur;
		
		for (auto& res : op.res) {
			auto& rl = this->res_locks[res.idx];
			int release_time = (o_succ != -2) ? MAX_INT : t_end;
			if (rl.unlock == o_prev && o_prev >= 0) {
				rl.unlock = o;
				rl.time = release_time;
				rl.res_time = res.time;
			}
			else if (rl.time <= t) {
				rl.lock = o;
				rl.unlock = o;
				rl.time = release_time;
				rl.res_time = res.time;
			}
			else {
				res_col.first = {rl.lock, rl.unlock, rl.res_time};
				res_col.second = {o, o, res.time};
				res_col.res = res.idx;

				assert(this->inst.ops[res_col.first.unlock].train != this->inst.ops[o].train);


				return false;
			}
		}

		if (o_succ >= 0) {
			int op_time = max(t_end, this->inst.ops[o_succ].start_lb);
			this->time[o_succ] = max(this->time[o_succ], op_time);	

			this->n_pred[o_succ] += 1;
			if (this->n_pred[o_succ] >= this->n_res_cons[o_succ] + 1) {
				assert(!added[o_succ]);
				added[o_succ] = true;
					
				this->prio_queue.push(o_succ, this->time[o_succ]);
			}
		}
	}

	return true;
}


void Graph::extend_res_col(Res_col& res_col)
{
	this->extend_res_unlock(res_col.first.unlock, res_col.res);
	this->extend_res_unlock(res_col.second.unlock, res_col.res);
}


void Graph::extend_res_unlock(int& op_unlock, int res)
{
	while (true) {
		int op_succ = this->path_succ[op_unlock];
		if (op_succ < 0 || this->inst.ops[op_succ].res.find(res) < 0) {
			break;
		}
		op_unlock = op_succ;
	}
}


int Graph::add_res_cons(const Res_col& res_col, bool second_to_first)
{
	int op_from;
	int op_to;
	int res_time;

	if (!second_to_first) {
		op_from = res_col.first.unlock;
		op_to = res_col.second.lock;
		res_time = res_col.first.res_time;
	}
	else {
		op_from = res_col.second.unlock;
		op_to = res_col.first.lock;
		res_time = res_col.second.res_time;
	}

	int res_cons_idx = this->res_cons_idx[op_from];
	if (res_cons_idx == -1) {
		this->res_cons_idx[op_from] = this->res_cons.size();
	}
	else {
		while (this->res_cons[res_cons_idx].next_idx >= 0) {
			res_cons_idx = this->res_cons[res_cons_idx].next_idx;
		}	
		this->res_cons[res_cons_idx].next_idx = this->res_cons.size();
	}

	this->res_cons.push_back({
		.op = op_to,
		.time = res_time,
		.next_idx = -1
	});
	
	this->n_res_cons[op_to] += 1;
	
	return res_cons_idx;
}


void Graph::remove_last_res_cons(const Res_col& res_col, bool second_to_first, int res_cons_idx)
{
	int op_from;
	int op_to;

	if (!second_to_first) {
		op_from = res_col.first.unlock;
		op_to = res_col.second.lock;
	}
	else {
		op_from = res_col.second.unlock;
		op_to = res_col.first.lock;
	}
	
	if (res_cons_idx == -1) {
		this->res_cons_idx[op_from] = -1;
	}
	else {
		this->res_cons[res_cons_idx].next_idx = -1;
	}

	this->n_res_cons[op_to] -= 1;
	this->res_cons.pop_back();
}
