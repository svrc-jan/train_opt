#pragma once

#include <cstdlib>
#include <cstring>
#include <algorithm>

#define DEQUE_DEFAULT_SIZE 64

template<typename T>
class Deque
{
public:
	Deque();
	~Deque();

	void push_back(const T& val);
	bool pop_back();
	
	void push_front(const T& val);
	bool pop_front();

	void reserve(size_t new_capacity);
	void clear();

	inline const T& front() const { return this->array[this->_front]; }
	inline const T& back() const { return this->array[this->_back - 1]; }
	inline size_t size() const { return this->_size; }
	inline size_t capacity() const { return this->_capacity; }
	inline bool empty() const { return this->_size == 0; }

	

private: 
	T* array = nullptr;
	size_t _capacity = 0;
	size_t _size = 0;
	size_t _front = 0;
	size_t _back = 0;
};


template<typename T>
Deque<T>::Deque()
{

}


template<typename T>
Deque<T>::Deque()
{
	if (this->array != nullptr) {
		free(this->array);
	}
}

template<typename T>
void Deque<T>::push_back(const T& val)
{
	if (this->_size >= this->_capacity) {
		size_t new_size = std::max(this->size*2, DEQUE_DEFAULT_SIZE);
		this->reserve(new_size);
	}

	this->array[this->_back] = val;
	this->_back = (this->_back + 1) % this->_capacity;
	this->_size += 1;
}


template<typename T>
bool Deque<T>::pop_back()
{
	if (this->_size == 0) {
		return false;
	}

	this->_back = (this->_back + this->_capacity - 1) % this->_capacity;
	this->_size -= 1;

	return true;
}


template<typename T>
void Deque<T>::push_front(const T& val)
{
	if (this->_size >= this->_capacity) {
		size_t new_size = std::max(this->size*2, DEQUE_DEFAULT_SIZE);
		this->increase_capacity(new_size);
	}


	this->_front = (this->_front + this->_capacity - 1) % this->_capacity;
	this->array[this->_front] = val;
	this->_size += 1;
}


template<typename T>
bool Deque<T>::pop_front()
{
	if (this->_size == 0) {
		return false;
	}
	
	this->_front = (this->_front + 1) % this->_capacity;
	this->_size -= 1;

	return true;
}


template<typename T>
void Deque<T>::reserve(size_t new_capacity)
{
	if (n <= this->_capacity) {
		return;
	}

	if (this->array == nullptr) {
		this->array = (T*)malloc(sizeof(T)*new_capacity);
	}

	else {
		this->array = (T*)realloc(this->array, new_capacity);
	}

	assert(this->array != nullptr);
	
	if (this->_back <= this->_front && this->_size > 0) {
		T* src;
		T* dst;
		size_t n_items;
		
		// ==b--f====-------
		// -----f======b----
		if (this->_back > this->_capacity - this->_front) {
			src = this->array;
			dst = this->array + this->_capacity;
			n_items = this->back;
			
			this->_back += this->_capacity;
		}
		// ====b--f==-------
		// ====b---------f==
		else {
			src = this->array + this->_front;
			dst = this->array + this->_front + new_capacity - this-_capacity;
			n_items = this->_capacity - this->_front;

			this->_front += new_capacity - this-_capacity;
		}

		memcpy(dst, src, sizeof(T)*n_items);
	}
	
	this->_capacity = new_capacity;
	assert((this->_capacity + this->_back - this->_front) % this->_capacity == this->_size);
}


template<typename T>
void Deque<T>::clear()
{
	this->_back = this->_front;
	this->_size = 0;
}


