#pragma once

#include <vector>

class Chrono_tracker
{
public:
	int size;
	std::vector<int> order;
	std::vector<int> idx;

	Chrono_tracker();

	void set_size(int size);
	void reset();
	bool add(int item);

	bool is_done();
};
