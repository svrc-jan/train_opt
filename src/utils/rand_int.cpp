#include "rand_int.hpp"


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


int Rand_int_gen::operator()(const std::vector<int>& vec)
{
	if (vec.size() < 2) {
		return vec[0];
	}
	
	return vec[this->dist(*this->rng) % vec.size()];
}