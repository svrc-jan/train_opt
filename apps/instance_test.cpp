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

	int max_n_succ = 0;
	map<int, int> unlock_path_len;

	for (Op& op : inst.ops) {
		max_n_succ = max(max_n_succ, op.n_succ());
		for (int i = 0; i < op.n_res(); i++) {
			unlock_path_len[op.res_info[i].unlock_jumps] += 1;
		}
	}

	cout << unlock_path_len << endl;
	cout << max_n_succ << endl;



	return 0;
}
