#pragma once

#include <cstdlib>
#include <cassert>
#include <vector>

using namespace std;


template<typename T>
struct Array_block
{
	T* begin_ptr = nullptr;
	T* end_ptr = nullptr;

	T* begin() { return this->begin_ptr; }
	T* end() { return this->end_ptr;}
};

struct Array_entry_local
{
	size_t idx = 0;
	size_t size = 0;
};

template<typename T>
struct Array_entry
{
	T** ptr = nullptr;
	size_t idx = 0;
	size_t size = 0;

	T* begin();
	T* end();

	Array_block<T> get_block();
	T& get_elem(size_t elem_idx);
	
	Array_block<T> operator()() { return this->get_block(); }
	T& operator[](size_t ei) { return this->get_elem(ei); }

};

template<typename T>
class Array_component
{
public:
	Array_component();
	~Array_component();
	
	Array_entry<T> add_entry(size_t size);
	void clear();

	size_t size() { return this->array_size; }
	void reserve(size_t cap);

	T* begin() { return this->array; }
	T* end() { return this->array + this->array_size; }
	
private:
	size_t array_cap = 0;
	size_t array_size = 0;
	T* array = nullptr;

	void adjust_capacity(size_t new_size);
};

#include "array_component.tpp"
