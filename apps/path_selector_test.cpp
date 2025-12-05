#include "iostream"

#include "utils/stl_print.hpp"
#include "instance.hpp"
#include "path_selector.hpp"

using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/wab_large_1.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	cout << file_name << endl;
	Instance inst(file_name);


	Path_selector path_sel(inst);

	// for (int i = 0; i < inst.n_ops(); i++) {
	// 	auto& op = inst.ops[i];
	// 	printf("%d:%d%s", op.prev.size, op.succ.size, ((i == inst.n_ops() - 1) ? "\n" : ", "));
	// }

	vector<double> op_imp;
	path_sel.get_op_importance(op_imp);
	
	vector<double> res_imp;
	path_sel.get_res_importance(res_imp, op_imp);

	vector<int> path;
	vector<vector<int>> paths;
	for (int t = 0; t < inst.n_trains(); t++) {
		auto& train = inst.trains[t];

		path_sel.select_path_by_res_imp(path, t, res_imp);
		

		for (int o = train.op_start; o < train.op_end(); o++) {
			op_imp[o] = 0.0;
		}

		for (int o : path) {
			op_imp[o] = 1.0;
		}

		paths.push_back(path);
	}

	for (auto& p : paths) {
		cout << p << endl;
	}

	return 0;
}
