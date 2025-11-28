#include "iostream"
#include "instance.hpp"
#include "tree.hpp"


using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_full_0.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	cout << file_name << endl;

	Instance inst(file_name);
	Tree tree(inst);

	tree.solve(0);
	cout << "solved in " << tree.n_solve_calls << " calls, obj: " << tree.obj_ub << endl;

	
	return 0;
}
