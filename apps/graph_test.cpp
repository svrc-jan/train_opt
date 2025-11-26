#include "iostream"
#include "utils/rand_int.hpp"
#include "utils/stl_print.hpp"
#include "instance.hpp"
#include "graph.hpp"


using namespace std;

int main(int argc, char const *argv[])
{
	string file_name = "data/nor1_full_0.json";

    if (argc > 1) {
        file_name = string(argv[1]);
    }

	cout << file_name << endl;

	Instance inst(file_name);
	Graph graph(inst);

	vector<int> order(graph.n_nodes);
	vector<int> start_time(graph.n_nodes);
	vector<int> op_prev(graph.n_nodes);

	graph.make_order(order, start_time, op_prev);

	return 0;
}
