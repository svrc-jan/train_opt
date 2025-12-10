#include <print>
#include "utils/rand_int.hpp"
#include "path_selector.hpp"
#include "graph.hpp"
#include "schedule.hpp"


using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "";

	char* train_opt_inst;
	if ((train_opt_inst = getenv("TRAIN_OPT_INST")) != nullptr) {
		file_name = string(train_opt_inst);
		println("setting env var inst: {}", file_name);
	}

    if (argc > 1) {
        file_name = string(argv[1]);
		println("setting arg inst: {}", file_name);
    }

	if (file_name.length() == 0) {
		println(stderr, "no instance set");
		return 1;
	}

	Instance inst(file_name);
	Path_selector path_sel(inst);
	Preprocess prepr(inst);
	Graph graph(prepr);
	Schedule sched(graph);

	vector<vector<int>> paths;
	path_sel.select_all_paths_by_res_imp(paths);
	
	graph.set_all_paths(paths);
	sched.set_all_paths(paths);
	
	Schedule::Res_edges res_edges;
	if (sched.process_from_start(res_edges)) {
		println("no res col");
	}
	else {
		println("res edges: {} -> {} or {} -> {}",
			res_edges.first.vertex_from,
			res_edges.first.vertex_to,
			res_edges.second.vertex_from,
			res_edges.second.vertex_to
		);
	}

	return 0;
}
