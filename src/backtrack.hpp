#pragma once

#include <queue>
#include "instance.hpp"

struct Op_st
{
    int op_idx;
    int train_idx;
    int start = -1;
};

bool operator<(const Op_st& x, const Op_st& y);


class Backtrack
{
private:
    const Instance& inst;

public:
    Backtrack(Instance& inst_) : inst(inst_) {}

    void solve();

    bool rec_call(std::vector<Op_st>& ops, std::vector<int>& train_op, 
        std::vector<int>& train_end, std::vector<int>& res);

    void get_op_start(Op_st& op_st, std::vector<int>& train_end, 
        std::vector<int>& res, int start);

    static void print_ops(const std::vector<Op_st>& ops);
};


