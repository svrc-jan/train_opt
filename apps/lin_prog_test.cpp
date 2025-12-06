#include <filesystem>

#include <iostream>

#include "utils/stl_print.hpp"
#include "instance.hpp"
#include "preprocess.hpp"
#include "path_selector.hpp"
#include "lin_prog.hpp"


using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/wab_large_1.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	cout << file_name << endl;
	Instance inst(file_name);
	Preprocess prepr(inst);
	Path_selector path_sel(inst);
	Lin_prog lin_prog(inst, prepr);

	vector<double> op_imp;
	path_sel.get_op_importance(op_imp);
	
	vector<double> res_imp;
	path_sel.get_res_importance(res_imp, op_imp);

	vector<int> path;
	vector<vector<int>> paths;
	for (int t = 0; t < inst.n_trains(); t++) {
		path_sel.select_path_by_res_imp(path, t, res_imp);
		paths.push_back(path);
	}

	lin_prog.init_model();
	for (int t = 0; t < inst.n_trains(); t++) {
		lin_prog.add_path(t, paths[t]);
	}

	bool ret = lin_prog.dfs_resolve();
	
	if (ret) {
		cout << "solved" << endl;
	}


	return 0;
}
