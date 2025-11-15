#pragma once

#include <cassert>
#include <vector>
#include <map>

using namespace std;

struct Disjoint_set {

	int n_items;
	int n_sets;

	vector<int> parent = {};
	vector<int> size = {};

	Disjoint_set(int n_items) {
		this->n_items = n_items;
		this->n_sets = n_items;

		this->parent.resize(n_items);
		this->size.resize(n_items);

		for (int i = 0; i < n_items; i++) {
			parent[i] = i;
			size[i] = 1;
		}
	}

	int find_set(int v) {
		while (v != this->parent[v]) {
			this->parent[v] = this->parent[this->parent[v]];
			v = this->parent[v];
		}

		return v;
	}

	void union_set(int a, int b) {
		a = find_set(a);
		b = find_set(b);
		if (a != b) {
			if (size[a] < size[b]) {
				swap(a, b);
			}

			this->parent[b] = a;
			this->size[a] += this->size[b];

			this->n_sets -= 1;
		}
	}

	pair<vector<int>, vector<int>> get_sets()
	{
		vector<int> sets_size;
		sets_size.reserve(this->n_sets);
		map<int, int> idx_map;
		
		int idx = 0;

		for (int i = 0; i < this->n_items; i++) {
			if (i == this->parent[i]) {
				idx_map[i] = idx;
				sets_size.push_back(this->size[i]);
				
				idx += this->size[i];
			}
		}

		vector<int> sets_item(this->n_items);
		for (int i = 0; i < n_items; i++) {
			int& idx = idx_map[this->find_set(i)];
			sets_item[idx++] = i;
		}

		return {sets_size, sets_item};
	}


};
