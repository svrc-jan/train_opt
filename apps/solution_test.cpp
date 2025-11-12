#include "instance.hpp"
#include "solution.hpp"

#include <filesystem>

using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_critical_0.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	Instance inst(file_name);

	Rand_int_gen rng;

	Solution sol(inst, rng);
	sol.make_random_paths();

	for (auto& x : sol.mp_res_use) {
		cout << x.first << " : ";
		for (auto& ru : x.second) {
			cout << "(lock=" << ru.lock << ", unlock=" << ru.unlock << ", time=" << ru.time <<"), ";
		}
		cout << endl;
	}
	

	return 0;
}
