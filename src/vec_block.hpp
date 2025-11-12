#pragma once

#include <vector>
#include <iostream>

using namespace std;

template<typename T>
struct Vec_block
{
	T* _begin;
	T* _end;

	T* begin() 
	{ return this->_begin; }
	
	T* end() 
	{ return this->_end; }

	T* begin() 
	{ return this->_begin; }
	
	T* end() 
	{ return this->_end; }
	
	size_t size() const
	{ return (size_t)(this->_end - this->_begin); }
	
	T& operator[](size_t idx) 
	{ return *(this->_begin + idx); }

	const T& operator[](size_t idx) const
	{ return this->p_vec->operator[](this->begin_idx + idx); }

	void push_back(const T value);
};

template<typename T>
bool operator<(const Vec_block<T>& lhs, const Vec_block<T>& rhs)
{
	size_t min_size = min(lhs.size(), rhs.size());

	for (size_t i = 0; i < min_size; i++) {
		if (lhs[i] < rhs[i]) {
			return true;
		}

		if (lhs[i] > rhs[i]) {
			return false;
		}
	}

	if (lhs.size() < rhs.size()) {
		return true;
	}

	return false;
}

template<typename T>
bool operator==(const Vec_block<T>& lhs, const Vec_block<T>& rhs)
{
	if (lhs.size() != rhs.size()) {
		return false;
	}

	for (size_t i = 0; i < lhs.size(); i++) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

template<typename T>
bool operator==(const Vec_block<T>& lhs, const vector<T>& rhs)
{
	if (lhs.size() != rhs.size()) {
		return false;
	}

	for (size_t i = 0; i < lhs.size(); i++) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}



template<typename T>
std::ostream& operator<< (std::ostream& os, const Vec_block<T>& vb)
{
	os << "[";

	for (size_t i = vb.begin_idx; i < vb.end_idx; i++) {
		os << vb.p_vec->operator[](i) << (i < vb.end_idx - 1 ? ", " : "");
	}
	
	os << "]";

	return os;
}

template<typename T>
void Vec_block<T>::push_back(const T value)
{
	if (this->p_vec->size() <= this->end_idx) {
		this->p_vec->resize(this->end_idx + 1);
	}
	
	this->p_vec->operator[](this->end_idx++) = value;
}