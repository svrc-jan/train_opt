#include "path_selector.hpp"

#include "coin/CoinBuild.hpp"

Path_selector::Path_selector(const Instance& inst) : inst(inst)
{
	train_lps.resize(inst.n_trains(), nullptr);
}

Path_selector::~Path_selector()
{
	for (auto lp : this->train_lps) {
		if (lp != nullptr) {
			delete lp;
		}
	}
}


bool Path_selector::make_train_lp(int t)
{
	if (this->train_lps[t] != nullptr) {
		return false;
	}

	auto& train = this->inst.trains[t];

	int n_row = train.ops.size;
	int n_col = 0;
	for (auto& op : train.ops) {
		n_col += op.succ.size;
	}

	ClpSimplex* lp = new ClpSimplex();
	CoinBuild col_build(1);

	int col_rows[2];
	double col_elem[2] = {1.0, -1.0};

	for (int row = 0; row < train.ops.size; row++) {
		auto& op = train.ops[row];

		col_rows[0] = row;
		
		for (int s : op.succ) {
			int row_succ = s - train.op_start;
			
			col_rows[1] = row_succ;
			col_build.addColumn(2, col_rows, col_elem, 0.0, 1.0, 0.0);
		}
	}

	lp->addColumns(col_build);

	for (int row = 0; row < train.ops.size; row++) {
		auto& op = train.ops[row];
		double row_bnd;

		if (op.prev.size == 0) {
			row_bnd = -1.0;
		}
		else if (op.succ.size == 0) {
			row_bnd = 1.0;
		}
		else {
			row_bnd = 0;
		}

		lp->setRowBounds(row, row_bnd, row_bnd);
	}
}

