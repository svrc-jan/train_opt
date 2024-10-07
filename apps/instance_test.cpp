#include "instance.hpp"


int main(int argc, char const *argv[])
{
	std::string file_name = "../data/test/swapping1.json";

    if (argc > 1) {
        file_name = std::string(argv[1]);
    }

	uint x = -1;

	Instance inst("name", file_name);

	return 0;
}


