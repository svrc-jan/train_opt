#pragma once

#include "utils.hpp"



template<typename T>
void print_vec(std::ostream& os, const std::vector<T>& vec)
{
	os << "[";
	for (size_t i = 0; i < vec.size(); i++) {
		os << vec[i] << (i < vec.size() - 1 ? ", " : "");

#if defined VEC_TRUNCATE_SIZE && VEC_TRUNCATE_SIZE > 0 
		if (i >= VEC_TRUNCATE_SIZE) {
			os << "...";
			break;
		}
#endif

	}
	os << "]";
}

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec)
{
	print_vec(os, vec);
	return os;
}

template<typename T>
void print_list(std::ostream& os, const std::list<T>& lst)
{
	os << "[";
	int i = 0;
	for (auto it = lst.begin(); it != lst.end(); it++) {
		os << *it << (i < (int)lst.size() - 1 ? ", " : "");

#if defined VEC_TRUNCATE_SIZE && VEC_TRUNCATE_SIZE > 0 
		if (i >= VEC_TRUNCATE_SIZE) {
			os << "...";
			break;
		}
#endif
		i++;
	}
	os << "]";
}

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::list<T>& lst)
{
	print_list(os, lst);
	return os;
}


template<typename Tk, typename Tv>
void print_map(std::ostream& os, const std::map<Tk, Tv>& mp)
{
	os << "{";
	size_t i = 0;
	for (auto it = mp.begin(); it != mp.end(); it++) {
		os << it->first << ":" << it->second << (i < mp.size() - 1 ? ", " : "");

#if defined VEC_TRUNCATE_SIZE && VEC_TRUNCATE_SIZE > 0 
		if (i >= VEC_TRUNCATE_SIZE) {
			os << "...";
			break;
		}
#endif
		i++;
	}
	os << "}";
}


template<typename Tk, typename Tv>
std::ostream& operator<< (std::ostream& os, const std::map<Tk, Tv>& mp)
{
	print_map(os, mp);
	return os;
}


template<typename Ta, typename Tb>
void print_pair(std::ostream& os, const std::pair<Ta, Tb>& pr)
{
	os << "(" << pr.first << ", " << pr.second << ")";
}


template<typename Ta, typename Tb>
std::ostream& operator<< (std::ostream& os, const std::pair<Ta, Tb>& pr)
{
	print_pair(os, pr);
	return os;
}


template<typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	std::vector<T> res = a;

	for (size_t i = 0; i < a.size(); i++) {
		res[i] += b[i];
	}

	return res;
}


template<typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	std::vector<T> res = a;

	for (size_t i = 0; i < a.size(); i++) {
		res[i] -= b[i];
	}

	return res;
}


template<typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());

	for (size_t i = 0; i < a.size(); i++) {
		a[i] += b[i];
	}

	return a;
}


template<typename T>
std::vector<T>& operator-=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());

	for (size_t i = 0; i < a.size(); i++) {
		a[i] -= b[i];
	}

	return a;
}


