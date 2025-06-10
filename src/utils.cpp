#include "utils.hpp"

bool file_exists(const std::string& name) {
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}   
}


std::string get_parent_dir(const std::string& s)
{	
	if (s.rfind("/", 0) == 0) {
		return s;
	}

	if (s.rfind("./", 0) == 0) {
		return "." + s; 
	}

	return "../" + s;
}

json get_json_file(const std::string& file_name)
{
	std::string file_name_parent = get_parent_dir(file_name);

	std::ifstream file;

	if (file_exists(file_name)) {
		// std::cerr << "json file '" << file_name << "' found" << std::endl;
		file.open(file_name);
	}
	
	else if (file_exists(file_name_parent)) {
		// std::cerr << "json file '" << file_name_parent << "' found" << std::endl;
		file.open(file_name_parent);
	}
		
	else {
		std::cerr << "json file '" << file_name << "' not found" << std::endl;
		exit(1);
	}

	json config = json::parse(file);

	return config;
}


Rand_int_gen::Rand_int_gen(const uint seed)
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


int Rand_int_gen::operator()(const int range)
{
	if (range < 2) {
		return 0;
	}

	return this->dist(*this->rng) % range;
}

int Rand_int_gen::operator()(const int start, const int end)
{
	if (end < start + 2) {
		return start;
	}
	
	return start + this->dist(*this->rng) % (end - start);
}

