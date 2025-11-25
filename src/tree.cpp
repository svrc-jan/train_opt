#include "tree.hpp"

#include "iostream"
#include "graph.hpp"


Tree::Tree(const Instance& inst) : inst(inst)
{
	
}

bool Tree::solve()
{
	Graph graph(this->inst);

	vector<int> order;
	vector<int> time;
	vector<int> op_in;
	vector<int> op_out;

	int it = 0;
	while (true) {
		if (!graph.get_order(order, time, op_in)) {
			cout << "not solved" << endl;
			return false;
		}

		make_op_out(op_out, op_in);
		Res_col rc;
		
		if (!this->get_res_col(rc, order, time, op_in, op_out)) {
			break;
		}

		// cout << "adding: " << rc.first.end << " -(" << rc.first.res_time << ")-> " << rc.second.start << endl;

		// graph.add_res_cons(rc.first.end, rc.second.start, rc.first.res_time);
		it += 1;
	}

	cout << "solved in " << it << " iter" << endl;

	return true;
}


void Tree::make_op_out(vector<int>& op_out, const vector<int>& op_in)
{

}


bool Tree::get_res_col(Res_col& result, const vector<int>& order, const vector<int>& time,
	const vector<int>& op_in, const vector<int>& op_out)
{

}
