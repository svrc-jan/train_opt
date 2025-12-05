#pragma once

#include <vector>

struct Disjoint_set
{
	int n_items;
	int n_sets;

	std::vector<int> parent = {};
	std::vector<int> size = {};

	Disjoint_set(int n_items);
	
	int find_set(int v);
	void union_set(int a, int b);

	std::vector<int> get_result();
};
