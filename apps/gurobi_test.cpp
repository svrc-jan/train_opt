#include <filesystem>

#include <iostream>

#include "utils/stl_print.hpp"
#include "instance.hpp"
#include "preprocess.hpp"
#include "path_selector.hpp"
#include "gurobi_model.hpp"


using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/nor1_full_0.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	GRBEnv grb_env = GRBEnv();
	grb_env.set(GRB_IntParam_OutputFlag, 0);

	cout << file_name << endl;
	Instance inst(file_name);
	Preprocess prepr(inst);
	Path_selector path_sel(inst);
	Gurobi_model grb_model(prepr, grb_env);

	vector<double> op_imp;
	path_sel.get_op_importance(op_imp);
	
	vector<double> res_imp;
	path_sel.get_res_importance(res_imp, op_imp);

	vector<vector<int>> paths(inst.n_trains());
	for (int t = 0; t < inst.n_trains(); t++) {
		path_sel.select_path_by_res_imp(paths[t], t, res_imp);
	}

	grb_model.init_model();
	for (int t = 0; t < inst.n_trains(); t++) {
		grb_model.add_path(t, paths[t]);
	}

	grb_model.solve();

	return 0;
}
