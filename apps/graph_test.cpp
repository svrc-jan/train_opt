#include "utils/rand_int.hpp"
#include "instance.hpp"
#include "graph.hpp"


#include <filesystem>

using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_critical_0.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	Instance inst(file_name);

	Graph g();

	Rand_int_gen rng();

	for (int i = 0; i < 5; i++) {
		
	}
	

	return 0;
}
