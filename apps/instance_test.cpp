#include <filesystem>

#include "instance.hpp"

using namespace std;

int main(int argc, char const *argv[])
{

	string file_name = "data/nor1_critical_0.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	Instance inst(file_name);

	for (const Train& train : inst.trains) {
		cout << train.idx << "(" << train.n_ops() << ")" << endl;
	}

	return 0;
}
