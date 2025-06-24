#include <filesystem>

#include "instance.hpp"
#include "bin_vec.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/testing/headway1.json";

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	Instance inst(file_name);

	cout << bin_vec::to_vector(inst.start_res_vec) << endl;

	return 0;
}
