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
	
	for (uint n = 0; n < 1000000; n++) {
		// std::cout << "iter " << n << std::endl; 
		sol.make_random_fork_choice();
		for (uint )
	}
	

	return 0;
}
