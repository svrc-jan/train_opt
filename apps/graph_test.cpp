#include <print>
#include "utils/rand_int.hpp"
#include "path_selector.hpp"
#include "graph.hpp"


using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_full_0.json";

	char* train_opt_inst;
	if ((train_opt_inst = getenv("TRAIN_OPT_INST")) != nullptr) {
		file_name = string(train_opt_inst);
	}

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


	Rand_int_gen rng;

	int n_iter = 10000;
	for (int i = 0; i < n_iter; i++) {
		int l1 = rng(prepr.n_levels());
		int l2 = rng(prepr.n_levels());

		if (prepr.levels[l1].train == prepr.levels[l2].train) {
			// println("iter {}: same train", i);
			continue;
		}

		if (!graph.add_edge({l1, l2, 1})) {
			// println("iter {}: cycle!", i);
			continue;
		}
		
		assert(graph.get_time(l1) + 1 <= graph.get_time(l2));
	}

	double avg_delay = 0;

	for (int t = 0; t < prepr.n_trains(); t++) {
		int l = prepr.trains[t].level_last();
		double delay = graph.get_time(l) - prepr.levels[l].time_lb;
		avg_delay += delay/prepr.n_trains();
	}
	
	println("avg delay {:.1f}", avg_delay);

	return 0;
}
