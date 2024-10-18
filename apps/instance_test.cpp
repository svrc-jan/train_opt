#include "instance.hpp"
#include "solution.hpp"

int main(int argc, char const *argv[])
{
	std::string file_name = "../data/test/swapping1.json";

    if (argc > 1) {
        file_name = std::string(argv[1]);
    }

	uint x = -1;

	Instance inst("name", file_name);
	Solution sol(&inst);
	
	std::vector<uint> path_lens(inst.n_train, 0);
	for (uint n = 0; n < 1000000; n++) {
		// std::cout << "iter " << n << std::endl; 
		sol.make_random_branching_choice();
		for (uint i = 0; i < inst.n_train; i++) {
			uint l = sol.paths[i].size();
			if (l != path_lens[i]) {
				if (path_lens[i] != 0)
					std::cout << "train " << i 
						<< " change path len " << path_lens[i] 
						<< " -> " << l << std::endl;
				path_lens[i] = l; 
			}
		}
	}
	

	return 0;
}
