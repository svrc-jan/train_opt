#include "instance.hpp"
#include "solution.hpp"

#include <filesystem>

using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/testing/headway1.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	Instance inst("name", file_name);

	Rand_int_gen rng;

	Solution sol(inst, rng);
	for (int i = 0; i < inst.n_train; i++) {
		cout << sol.make_random_path(i) << endl;
	}

	return 0;
}
