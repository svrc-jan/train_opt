#include <filesystem>

#include "base_data.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	// string file_name = "data/testing/headway1.json";
	string file_name = "data/phase1/line1_critical_0.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	Base_data bd(file_name);

	for (auto& train : bd.trains) {
		for (auto& op : train.ops) {
			cout << '[' << op.dur << ", " << op.succ.size << "] ";
		}
		cout << endl;
	}

	return 0;
}
