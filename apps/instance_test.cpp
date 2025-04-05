#include "instance.hpp"
#include "backtrack.hpp"

int main(int argc, char const *argv[])
{
	// std::string file_name = "../data/phase1/line1_full_0.json";
	std::string file_name = "../data/test/infeasible2.json";

    if (argc > 1) {
        file_name = std::string(argv[1]);
    }

	uint x = -1;

	Instance inst("name", file_name);
	Backtrack bktr(inst);
	bktr.solve();

	// uint max_res = 0;
	// uint max_op = 0;

	// std::cout << inst.n_op << " " << inst.n_train << std::endl;

	return 0;
}
