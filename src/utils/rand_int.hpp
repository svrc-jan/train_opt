#pragma once

#include <vector>
#include <random>


typedef std::uniform_int_distribution<> unif_int_dist;


class Rand_int_gen
{
public:
	Rand_int_gen(uint seed=0);
	~Rand_int_gen();

	int operator()(const int range);
	int operator()(const int start, const int end);
	int operator()(const std::vector<int>& vec);


private:
	std::mt19937* rng;
	unif_int_dist dist;

	void init_rng(const uint seed);
};
