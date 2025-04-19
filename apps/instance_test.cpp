#include "instance.hpp"
#include "solution.hpp"

int main(int argc, char const *argv[])
{
	// std::string file_name = "../data/phase1/line1_full_0.json";
	std::string file_name = "../data/testing/headway1.json";
	// std::string file_name = "../data/test/infeasible2.json";

    if (argc > 1) {
        file_name = std::string(argv[1]);
    }

	Instance inst("name", file_name);
	
	std::vector<std::vector<int>> paths;
	for (int i = 0; i < inst.n_train; i++) {
		auto path = make_random_path(inst, i);
		paths.push_back(path);
	}

	make_order(inst, paths);

	return 0;
}
