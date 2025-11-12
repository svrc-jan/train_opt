#include <filesystem>

#include "instance.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	// string file_name = "data/testing/headway1.json";
	string file_name = "data/nor1_critical_0.json";
	

	if (argc > 1) {
		file_name = string(argv[1]);
	}

	Instance inst(file_name);
	// for (const Op& op : inst.v_op) {
	// 	cout << "Op " << op.idx << " - start: " << op.start_point_idx << ", end: " << op.end_point_idx << endl;
	// }

	// for (const Point& point : inst.v_point) {
	// 	cout << "Point " << point.idx << " - in: " << point.vb_in_op << ", out: " << point.vb_out_op << endl;
	// }

	for (const Train& train : inst.v_train) {
		cout << train.idx << "(" << train.end - train.begin << ")" << endl;
	}

	return 0;
}
