#pragma once

#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <random>

// #include <boost/multiprecision/cpp_int.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define VEC_TRUNCATE_SIZE 0

// typedef boost::multiprecision::uint256_t mask_t;

bool file_exists(const std::string& name);

json get_json_file(const std::string& file_name);

template<typename T>
void print_vec(std::ostream& os, const std::vector<T>& vec);

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T> & vec);


template<typename Tk, typename Tv>
void print_map(std::ostream& os, const std::map<Tk, Tv>& mp);

template<typename Tk, typename Tv>
std::ostream& operator<< (std::ostream& os, const std::map<Tk, Tv>& mp);


typedef std::uniform_int_distribution<> unif_int_dist;


template<typename T>
std::vector<T> operator+(std::vector<T> a, std::vector<T> b);

template<typename T>
std::vector<T> operator-(std::vector<T> a, std::vector<T> b);


template<typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b);


template<typename T>
std::vector<T>& operator-=(std::vector<T>& a, const std::vector<T>& b);

class Rand_int_gen
{
public:
	Rand_int_gen(int seed=0);
	~Rand_int_gen();

	int operator()(int range);
	int operator()(int start, int end);


private:
	std::mt19937* rng;
	unif_int_dist dist;

	void init_rng(uint seed);
};

#include "utils.tpp"
