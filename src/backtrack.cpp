#include "backtrack.hpp"

bool operator<(const Op_st& x, const Op_st& y) {
    return x.start > y.start;
}

void Backtrack::solve()
{
    std::vector<Op_st> ops;
    ops.reserve(this->inst.n_op/4);

    std::vector<int> train_op(this->inst.n_train, -1);
    std::vector<int> train_end(this->inst.n_train, 0);

    std::vector<int> res(this->inst.n_res, 0);

    this->rec_call(ops, train_op, train_end, res);
}

void Backtrack::get_op_start(Op_st& op_st, std::vector<int>& train_end, 
    std::vector<int>& res, int start)
{
    const Operation& succ = this->inst.ops[op_st.op_idx];
    op_st.start = std::max(succ.start_lb, train_end[op_st.train_idx]);

    if (op_st.start > succ.start_ub) {
        op_st.start = -2;
        return;
    }

    bool res_locked = false;
    // for (int res_idx : succ.res_idx) {
    //     if (res[res_idx] < 0) {
    //         res_locked = true;
    //         break;
    //     }

    //     op_st.start = std::max(op_st.start, res[res_idx]);

    //     if (op_st.start > succ.start_ub) {
    //         op_st.start = -2;
    //         return;
    //     }
    // }

    if (res_locked) {
        op_st.start = -1;
        return;
    }
}

bool Backtrack::rec_call(std::vector<Op_st>& ops, std::vector<int>& train_op, 
        std::vector<int>& train_end, std::vector<int>& res)
{
    // this->print_ops(ops);
    std::priority_queue<Op_st> ops_q;

    int start = 0;
    if (!ops.empty()) {
        start = ops.back().start;
    } 
    
    bool trains_done = true;
    for (int train_idx = 0; train_idx < this->inst.n_train; train_idx++) {
        if (train_op[train_idx] == -1) {            
            Op_st op_st;
            op_st.op_idx = this->inst.trains[train_idx].begin_idx;
            op_st.train_idx = train_idx;

            this->get_op_start(op_st, train_end, res, start);

            if (op_st.start == -2) {        
                std::cout << "start ub reached" << std::endl;
                return false;
            }

            if (op_st.start >= 0) {
                ops_q.push(op_st);
            }

            trains_done = false;
            continue;
        }

        if (this->inst.ops[train_op[train_idx]].n_succ == 0) {
            continue;
        }

        trains_done = false;

        // for (int succ_idx : this->inst.ops[train_op[train_idx]].succ) {
        //     Op_st op_st;
        //     op_st.op_idx = succ_idx;
        //     op_st.train_idx = train_idx;

        //     this->get_op_start(op_st, train_end, res, start);

        //     if (op_st.start == -2) {        
        //         std::cout << "start ub reached" << std::endl;
        //         return false;
        //     }

        //     if (op_st.start >= 0) {
        //         ops_q.push(op_st);
        //     }
        // }
    }

    if (trains_done) {
        std::cout << "trains done" << std::endl;
        return true;
    }

    std::unordered_map<int, int> old_res;
    for (; !ops_q.empty(); ops_q.pop()) {
        Op_st op_st = ops_q.top();

        int prev_idx = train_op[op_st.train_idx];
        int prev_end = train_end[op_st.train_idx];

        const Operation& prev = this->inst.ops[prev_idx];
        const Operation& next = this->inst.ops[op_st.op_idx];

        // unlock prev resources
        // if (prev_idx >= 0) {
        //     for (int i = 0 ; i < prev.n_res; i++) {
        //         int res_idx = prev.res_idx[i];
        //         assert(res[res_idx] == -1);
        //         // old_res[res_idx] = res[res_idx];
        //         res[res_idx] = op_st.start + prev.res_release_time[i];
        //     }
        // }


        // lock next resources
        // for (int res_idx : next.res_idx) {
        //     assert(res[res_idx] != -1);
        //     old_res[res_idx] = res[res_idx];
        //     res[res_idx] = -1;
        // }

        train_op[op_st.train_idx] = op_st.op_idx;
        train_end[op_st.train_idx] = op_st.start + next.dur;

        ops.push_back(op_st);

        bool rv = this->rec_call(ops, train_op, train_end, res);
        if (rv) {
            return true;
        }

        ops.pop_back();

        train_op[op_st.train_idx] = prev_idx;
        train_end[op_st.train_idx] = prev_end;


        // unlock next resources
        // for (int res_idx : next.res_idx) {
        //     res[res_idx] = old_res[res_idx];
        // }

        // // prev resources
        // if (prev_idx >= 0) {
        //     for (int res_idx : prev.res_idx) {
        //         res[res_idx] = -1;
        //     }
        // }
    }

    std::cout << "no ops possible" << std::endl;
    return false;
}


void Backtrack::print_ops(const std::vector<Op_st>& ops)
{
    using namespace std;
    int n = ops.size();
    if (n == 0) {
        cout << "[-]" << endl;
        return;
    }

    cout << "[";
    for (int i = 0; i < n; i++) {
        cout << ops[i].op_idx << "(" << ops[i].train_idx << ")" << ":" << ops[i].start << (i == n - 1 ? "" : ", ");
    }
    cout << "]" << endl;
}