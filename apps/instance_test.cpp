#include <filesystem>

#include <iostream>

#include "utils/stl_print.hpp"
#include "instance.hpp"

using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/wab_large_1.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	cout << file_name << endl;
	Instance inst(file_name);
	
	// for (Instance::Op& op : inst.ops) {
	// 	for (Instance::Res& res : op.res) {
	// 		bool res_found = false;
	// 		bool res_missing = false;

	// 		for (int s : op.succ) {
	// 			if (inst.ops[s].res.find_sorted(res.idx) == -1) {
	// 				res_missing = true;
	// 			}
	// 			else {
	// 				res_found = true;
	// 			}
	// 		}

	// 		assert(!(res_found && res_missing));
	// 	}
	// }


	return 0;
}
