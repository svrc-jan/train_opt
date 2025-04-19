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


Rand_int_gen::Rand_int_gen(int seed)
{
    this->init_rng(seed);
    this->dist = unif_int_dist(0);
}

Rand_int_gen::~Rand_int_gen()
{
    delete this->rng;
}


void Rand_int_gen::init_rng(uint seed)
{
	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	this->rng = new std::mt19937(seed);
}


int Rand_int_gen::operator()(int range)
{
    if (range < 2) {
        return 0;
    }

    return this->dist(*this->rng) % range;
}

int Rand_int_gen::operator()(int start, int end)
{
    if (end < start + 2) {
        return start;
    }
    
    return start + this->dist(*this->rng) % (end - start);
}

