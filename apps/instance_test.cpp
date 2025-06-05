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


	return 0;
}
