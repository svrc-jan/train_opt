#pragma once

#include <vector>

#include "utils/prio_queue.hpp"
#include "utils/deque.hpp"
#include "instance.hpp"

using namespace std;

enum Op_state_enum 
{
	OP_INVALID,
	OP_VALID,
	OP_REQUIRED
};


enum Make_order_return_enum
{
	RES_COL,
	UB_REACHED,
	TRAIN_UNFINISHED,
	FEASIBLE
};

struct Res_lock;
struct Res_col;
struct Res_cons;

class Graph
{
public:
	Graph(const Instance& inst);

	int n_ops;
	int n_res;

	vector<int> time = {};
	vector<int> state = {};
	vector<pair<int, int>> path = {};

	bool update_path(const int train_idx);
	int make_order(vector<int>& order, Res_col& res_col, int& obj);

	bool lock_path(const int last_op);
	bool reroute_path(const int start_op, const int last_op);

	void extend_res_col(Res_col& res_col);
	void extend_res_unlock(int& op_unlock, int res);
	
	int add_res_cons(const Res_col& res_col, bool second_to_first);
	void remove_last_res_cons(const Res_col& res_col, bool second_to_first, int res_cons_idx);

private:
	const Instance& inst;
	
	Prio_queue<int> prio_queue;
	Deque<int> deque;


	vector<int> n_res_cons = {};
	vector<int> res_cons_idx = {};
	vector<Res_cons> res_cons = {};

	vector<int> n_pred = {};
	vector<Res_lock> res_locks = {};

	void make_path_prev();
};


struct Res_lock
{
	int lock = -1;
	int unlock = -1;
	int time = -1;
	int res_time = -1;
};


struct Res_col
{
	struct {
		int lock = -1;
		int unlock = -1;
		int res_time = -1;
	} first;

	struct {
		int lock = -1;
		int unlock = -1;
		int res_time = -1;
	} second;
	int res;
};


struct Res_cons
{
	int op = -1;
	int time = 0;
	int next_idx = -1;
};






