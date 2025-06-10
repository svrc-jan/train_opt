#pragma once

#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <random>


// #include <boost/multiprecision/cpp_int.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define VEC_TRUNCATE_SIZE 0

// typedef boost::multiprecision::uint256_t mask_t;


typedef std::uniform_int_distribution<> unif_int_dist;


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


template<typename Ta, typename Tb>
void print_pair(std::ostream& os, const std::pair<Ta, Tb>& pr);

template<typename Ta, typename Tb>
std::ostream& operator<< (std::ostream& os, const std::pair<Ta, Tb>& pr);

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
	Rand_int_gen(uint seed=0);
	~Rand_int_gen();

	int operator()(const int range);
	int operator()(const int start, const int end);


private:
	std::mt19937* rng;
	unif_int_dist dist;

	void init_rng(const uint seed);
};


bool move_out_of_build_dir(void);

#include "utils.tpp"
