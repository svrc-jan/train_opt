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
	vector<vector<int>> paths;
	path_sel.select_all_paths_by_res_imp(paths);

	for (auto& p : paths) {
		cout << p << endl;
	}

	return 0;
}
