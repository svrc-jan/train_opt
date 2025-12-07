#include <print>

#include "instance.hpp"
#include "preprocess.hpp"
#include "path_selector.hpp"
#include "graph.hpp"


using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_full_0.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	print("{}\n", file_name);

	Instance inst(file_name);
	Path_selector path_sel(inst);
	Preprocess prepr(inst);
	Graph graph(prepr);

	vector<vector<int>> paths;
	path_sel.select_all_paths_by_res_imp(paths);
	graph.add_all_paths(paths);

	if (!graph.make_order()) {
		print("make order failed\n");
		return 1;
	}

	if (!graph.make_time()) {
		print("make time failed");
		return 2;
	}


	return 0;
}
