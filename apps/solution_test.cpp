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

	Instance inst(file_name);

	Rand_int_gen rng;

	Solution sol(inst, rng);
	sol.make_all_random_paths();
	cout << sol.make_earliest_start() << endl;
	cout << sol.make_latest_start() << endl;
	cout << sol.make_order() << endl;


	return 0;
}
