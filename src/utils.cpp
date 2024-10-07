#include "utils.hpp"

bool file_exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

json get_json_file(const std::string& file_name)
{
	if (!file_exists(file_name)) {
        std::cerr << "json file '" << file_name << "' not found" << std::endl;
        exit(1);
    }

    std::ifstream file(file_name);
	json config = json::parse(file);

	return config;
}

