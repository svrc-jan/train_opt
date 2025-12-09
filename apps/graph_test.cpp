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
	graph.set_all_paths(paths);


	return 0;
}
