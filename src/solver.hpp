#pragma once

#include <print>
#include <unordered_map>
#include "utils/hasher.hpp"
#include "graph.hpp"


class Solver
{
public:
	const Instance& inst;
	const Preprocess& prepr;
	Graph& graph;

	Solver(Graph& inst);
	

	bool solve_with_train_prio(const std::vector<double>& prio);

private:
	std::vector<int> node_train = {};

};