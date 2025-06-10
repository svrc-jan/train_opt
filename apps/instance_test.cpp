#include "instance.hpp"

#include <filesystem>


int main(int argc, char const *argv[])
{
	std::string file_name = "data/testing/headway1.json";

	if (argc > 1) {
		file_name = std::string(argv[1]);
	}

	Instance inst(file_name);


	return 0;
}
