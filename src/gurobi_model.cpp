#include "gurobi_model.hpp"

#include <print>

using namespace std;

#define RES_MIN_DUR 0.1
#define EPS 1e-06

Gurobi_model::Gurobi_model(const Preprocess& prepr, const GRBEnv& grb_env)
	: inst(prepr.inst), prepr(prepr), grb_env(grb_env)
{
	for (int r = 0; r < this->inst.n_res(); r++) {
		res_uses.push_back({});
	}
}


Gurobi_model::~Gurobi_model()
{
	if (this->model != nullptr) {
		delete this->model;
	}
}


void Gurobi_model::init_model()
{
	this->model = new GRBModel(this->grb_env);

	this->model->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

	this->var_time.resize(this->prepr.n_levels());

	for (int l = 0; l < this->prepr.n_levels(); l++) {
		auto& level = this->prepr.levels[l];

		double var_lb = (double)level.time_lb;
		double var_ub = (level.time_ub < INT_MAX) ? (double)level.time_ub : GRB_INFINITY;
		double var_obj = (level.ops_out.size == 0) ? 1.0 : 0.0;
		string var_name = "time" + to_string(l);

		this->var_time[l] = this->model->addVar(var_lb, var_ub, var_obj, GRB_CONTINUOUS, var_name);
	}
}


void Gurobi_model::add_path(int train_idx, const vector<int>& path)
{
	for (int o : path) {
		auto& op = this->inst.ops[o];

		int lvl_start = this->prepr.op_level[o].first;
		int lvl_end = this->prepr.op_level[o].second;

		this->model->addConstr(this->var_time[lvl_start]
			+ (double)op.dur <= this->var_time[lvl_end]);

		for (auto& res : op.res) {
			auto& ru = this->res_uses[res.idx];
			if (ru.size() > 0 && ru.back().train == train_idx) {
				ru.back().level_lock = lvl_start;
				ru.back().res_time = max((double)res.time, RES_MIN_DUR);
			}
			else {
				ru.push_back({
					.train = train_idx,
					.level_lock = this->prepr.op_level[o].first,
					.level_unlock = this->prepr.op_level[o].second,
					.res_time = max((double)res.time, RES_MIN_DUR)
				});
			}
		}
	}
}



struct Interval
{
	int idx;
	double start;
	double end;

	inline bool operator<(const Interval& other) 
	{
		return (this->start < other.start) ? true 
			: ((this->start > other.start) ? false : (this->end < other.end));
	}
};

bool Gurobi_model::find_res_col(Res_col& res_col)
{
	
	vector<double> curr_sol(this->prepr.n_levels());
	
	for (int l; l < this->prepr.n_levels(); l++) {
		curr_sol[l] = this->var_time[l].get(GRB_DoubleAttr_X);
	}


	double best_overlap = 0.0;

	for (int r = 0; r < this->inst.n_res(); r++) {
		int n_ints = this->res_uses[r].size();
#ifndef NO_VLA
		Interval ints[n_ints];
#else
		vector<Interval> ints(n_ints);
#endif
		for (int i = 0; i < n_ints; i++) {
			auto& ru = this->res_uses[r][i];
			ints[i] = {
				.idx = i,
				.start = curr_sol[ru.level_lock],
				.end = curr_sol[ru.level_unlock] + ru.res_time
			};
		}

#ifndef NO_VLA
		sort(ints, ints + n_ints);
#else
		sort(ints.begin(), ints.end());
#endif
		bool search_done = false;
		for (int i = 0; i < n_ints && !search_done; i++) {
			for (int j = i + 1; j < n_ints && !search_done; j++) {
				if (ints[i].end > ints[j].start + EPS) {
					double overlap = ints[i].end - ints[j].start;
					if (overlap > best_overlap) {
						best_overlap = overlap;

						res_col.first = this->res_uses[r][ints[i].idx];
						res_col.second = this->res_uses[r][ints[j].idx];
					}
				}
				else {
					break;
				}
			}	
		}
	}

	return best_overlap == 0.0;
}


bool Gurobi_model::solve()
{
	
	while (true) {
		this->model->optimize();
		int status = this->model->get(GRB_IntAttr_Status);

		if (status != GRB_OPTIMAL) {
			println("optimization failed, status {}", status);
			return false;
		}

		Res_col res_col;
		if (this->find_res_col(res_col)) {
			println("optimization finished");
			break;
		}

		this->add_res_constr(res_col);
	}

	return true;
}


bool Gurobi_model::add_res_constr(const Res_col& res_col)
{
	pair<int, int> key1 = {res_col.first.level_unlock, res_col.second.level_lock};
	pair<int, int> key2 = {res_col.second.level_unlock, res_col.first.level_lock};
	
	double time1 = res_col.first.res_time;
	double time2 = res_col.second.res_time;

	assert(this->var_order.find(key1) == this->var_order.end());
	assert(this->var_order.find(key2) == this->var_order.end());

	string name1 = "order" + to_string(key1.first) + "_" + to_string(key1.second);
	GRBVar var1 = this->model->addVar(0.0, 1.0, 0.0, GRB_BINARY, name1);

	string name2 = "order" + to_string(key2.first) + "_" + to_string(key2.second);
	GRBVar var2 = this->model->addVar(0.0, 1.0, 0.0, GRB_BINARY, name1);

#if defined STATIC_LOG_LEVEL && STATIC_LOG_LEVEL >= 100
	println("adding constr: {} -({:.1f})> {} or {} -({:.1f})> - {}",
		key1.first, time1, key1.second, key2.first, time2, key2.second);
#endif

	this->model->addGenConstrIndicator(var1, 1, this->var_time[key1.first] + time1 <= this->var_time[key1.second]);
	this->model->addGenConstrIndicator(var2, 1, this->var_time[key2.first] + time2 <= this->var_time[key2.second]);
	this->model->addConstr(var1 + var2 == 1);
	this->model->update();

	this->var_order[key1] = var1;
	this->var_order[key2] = var2;

	return true;
}
