#include "iostream"

#include "instance.hpp"
#include "path_selector.hpp"

using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/wab_large_1.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	cout << file_name << endl;
	Instance inst(file_name);
	Path_selector path_sel(inst);

	for (int t = 0; t < inst.n_trains(); t++) {
		path_sel.make_train_lp(t);
	}

	return 0;
}
