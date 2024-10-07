#pragma once

#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define VEC_TRUNCATE_SIZE 0

bool file_exists(const std::string& name);

json get_json_file(const std::string& file_name);

template<typename T>
void print_vec(std::ostream& os, const std::vector<T>& vec);

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T> & vec);

#include "utils.tpp"
