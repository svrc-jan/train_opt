#include "solution.hpp"


std::vector<int> make_random_path(const Instance& inst, int train)
{
    std::vector<int> path;
    path.push_back(inst.trains[train].begin_idx);

    Rand_int_gen rig;

    while (true) {
        const Operation& last_op = inst.ops[path.back()];
        if (last_op.n_succ == 0) {
            break;
        }

        path.push_back(last_op.succ[rig(last_op.n_succ)]);
    }

    return path;
}

std::map<int, int> make_earlies_start_map(const Instance& inst)
{
    std::map<int, int> earliest_start;
    std::function<int(int)> rec_call = [&](int op_idx) -> int {
        auto it = earliest_start.find(op_idx);
        if (it != earliest_start.end()) {
            return it->second;
        }

        const Operation& op = inst.ops[op_idx];
        int start = op.start_lb;
        for (int prev_idx : op.prev) {
            const Operation& prev = inst.ops[prev_idx];
            start = std::max(start, rec_call(prev_idx) + prev.dur);
        }

        earliest_start[op_idx] = start;

        return start;
    };

    for (int op_idx = 0; op_idx < inst.n_op; op_idx++) {
        rec_call(op_idx);
    }

    return earliest_start;
}


std::vector<int> make_order(const Instance& inst, std::vector<std::vector<int>> paths)
{
    std::vector<int> order;
    std::map<int, int> earliest_start = make_earlies_start_map(inst);
    std::map<int, int> prev_op;

    order.reserve(std::accumulate(paths.begin(), paths.end(), 0,
        [](int sum, auto const& v) -> size_t { return sum + v.size(); }));

    for (auto p : paths) {
        order.insert(order.end(), p.begin(), p.end());

        prev_op[p[0]] = -1;
        for (size_t i = 1; i < p.size(); i++) {
            prev_op[p[i]] = p[i-1];
        }
    }
    
    std::map<int, std::vector<int>> op_res_vec;

    for (auto p : paths) {
        for (int i : p ) {
            op_res_vec[i] = std::vector<int>(inst.n_res, 0);
            
            if (prev_op[i] >= 0) {
                const Operation& prev = inst.ops[prev_op[i]];
                for (Res const& res : prev.res) {
                    op_res_vec[i][res.id] -= 1;
                }

            }

            const Operation& op = inst.ops[i];
            for (Res const& res : op.res) {
                op_res_vec[i][res.id] += 1;
            }
        }
    }

    // std::cout << op_res_vec << std::endl;

// std::cout << earliest_start << std::endl;

    std::sort(order.begin(), order.end(), [&earliest_start](int const& a, int const& b){
            if (earliest_start[a] == earliest_start[b]) {
                return a < b;
            }
            return earliest_start[a] < earliest_start[b];
        });

    
    std::vector<int> state_res_vec(inst.n_res, 1);
    for (int i : order) {
        state_res_vec -= op_res_vec[i];
        std::cout << i << " : " << state_res_vec << std::endl;
    }

    
    // std::cout << res_vec;




    return order;
}