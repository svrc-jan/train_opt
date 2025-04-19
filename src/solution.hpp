#pragma once

#include "instance.hpp"
#include "utils.hpp"


std::vector<int> make_random_path(const Instance& inst, int train);
std::vector<int> make_order(const Instance& inst, std::vector<std::vector<int>> paths);