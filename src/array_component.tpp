#pragma once

#include "array_component.hpp"

#define INIT_COMP_ARRAY_CAP 64UL

template<typename T>
T* Array_entry<T>::begin()
{
	return (this->ptr != nullptr) ?
		(*this->ptr) + this->idx :
		nullptr; 
}


template<typename T>
T* Array_entry<T>::end()
{
	return (this->ptr != nullptr) ?
		(*this->ptr) + this->idx + this->size :
		nullptr; 
}


template<typename T>
Array_block<T> Array_entry<T>::get_block()
{
	Array_block<T> rv;
	
	rv.begin_ptr = this->begin();
	rv.end_ptr = this->end();

	return rv;
}


template<typename T>
T& Array_entry<T>::get_elem(size_t elem_idx)
{
	assert(elem_idx < this->size);
	return this->begin()[elem_idx];
}


template<typename T>
Array_component<T>::Array_component()
{

}


template<typename T>
Array_component<T>::~Array_component()
{
	if (this->array != nullptr) {
		free(this->array);
	}
}


template<typename T>
Array_entry<T> Array_component<T>::add_entry(size_t size)
{
	Array_entry<T> rv;

	if (size == 0) {
		rv.ptr = nullptr;
		rv.idx = 0;
		rv.size = 0;

		return rv;
	}

	this->adjust_capacity(this->array_size + size);
	
	rv.ptr = &(this->array);
	rv.idx = this->array_size;
	rv.size = size;

	this->array_size += size;

	return rv;
}


// template<typename T>
// Array_entry<T> Array_component<T>::add_entry(const vector<T> &vals)
// {
// 	Array_entry<T> rv = this->add_entry(vals.size());

// 	for (int i = 0; i < vals.size(); i++) {
// 		rv[i] = vals[i];
// 	}

// 	return rv;
// }


template<typename T>
void Array_component<T>::clear()
{
	this->array_size = 0;
}


template<typename T>
void Array_component<T>::adjust_capacity(size_t new_size)
{
	size_t new_cap = max(this->array_cap, INIT_COMP_ARRAY_CAP);

	while (new_cap < new_size) {
		new_cap *=2;
	}

	this->reserve(new_cap);
}


template<typename T>
void Array_component<T>::reserve(size_t new_cap)
{
	if (this->array_cap < new_cap) {
		this->array_cap = new_cap;

		if (this->array == nullptr) {
			this->array = (T*)malloc(sizeof(T)*this->array_cap);
		}
		else {
			this->array = (T*)realloc(this->array, sizeof(T)*this->array_cap);
		}

		assert(this->array != nullptr);
	}
}
