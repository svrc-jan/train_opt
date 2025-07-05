#include <filesystem>

#include "instance.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/testing/headway1.json";

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	Instance inst(file_name);

	return 0;
}
