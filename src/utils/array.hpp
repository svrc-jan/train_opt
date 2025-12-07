#pragma once

#include <algorithm>
#include <vector>

template<typename T>
struct Array
{
	T* ptr = nullptr;
	int size = 0;

	inline void push_back(const T& x) { this->ptr[this->size++] = x; }

	inline T* begin() { return this->ptr; }
	inline T* end() { return this->ptr + this->size; }
	inline T& back() { return this->ptr[this->size - 1]; }
	inline T& operator[](int idx) { return this->ptr[idx]; }

	inline const T* begin() const { return this->ptr; }
	inline const T* end() const { return this->ptr + this->size; }
	inline const T& back() const { return this->ptr[this->size - 1]; }
	inline const T& operator[](int idx) const { return this->ptr[idx]; }

	void sort() { std::sort(this->begin(), this->end()); }
	bool is_asc() const;

	template<typename X>
	int find(const X& x) const;

	template<typename X>
	int find_sorted(const X& x) const;

	void assign_ptr(const std::vector<T>& vec, int& idx);
};


template<typename T>
bool Array<T>::is_asc() const
{
	for(int i = 1; i < this->size; i++) {
		if (this->ptr[i-1] > this->ptr[i]) {
			return false;
		}
	}

	return true;
}


template<typename T>
template<typename X>
int Array<T>::find(const X& x) const
{
	for (int i = 0; i < this->size; i++) {
		if (this->ptr[i] == x) {
			return i;
		}
	}

	return -1;
}


template<typename T>
template<typename X>
int Array<T>::find_sorted(const X& x) const
{
	if (this->size == 0) {
		return -1;
	}

	int l = 0;
	int r = this->size - 1;

	while (l <= r) {
		int m = l + (r - l)/2;

		if (this->ptr[m] == x) {
			return m;
		}

		if (this->ptr[m] < x) {
			l = m + 1;
		}
		else {
			r = m - 1;
		}
	}

	return -1;
}



template<typename T>
void Array<T>::assign_ptr(const std::vector<T>& vec, int& idx)
{
	if (this->size > 0) {
		this->ptr = (T*)vec.data() + idx;
		idx += this->size;
	}
}


