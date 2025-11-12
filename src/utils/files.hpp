#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool file_exists(const std::string& name);
json get_json_file(const std::string& file_name);
