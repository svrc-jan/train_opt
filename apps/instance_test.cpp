#include <filesystem>

#include <iostream>

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

	for (Train& train : inst.trains) {
		cout << train.idx << " : " << train.levels[0].dur_to_end << endl;
	}



	return 0;
}
