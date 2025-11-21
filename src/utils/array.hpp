#pragma once

#include <algorithm>
#include <vector>

template<typename T>
struct Array
{
	T* ptr = nullptr;
	int size = 0;

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

	int find(const T& x) const;
	int find_sorted(const T& x) const;

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
int Array<T>::find(const T& x) const
{
	for (int i = 0; i < this->size; i++) {
		if (x == this->ptr[i]) {
			return i;
		}
	}

	return -1;
}

template<typename T>
int Array<T>::find_sorted(const T& x) const
{
	if (this->size == 0) {
		return -1;
	}

	int l = 0;
	int r = this->size - 1;

	while (l <= r) {
		int m = l + (r - l)/2;

		if (x == this->ptr[m]) {
			return m;
		}

		if (x < this->ptr[m]) {
			r = m - 1;
		}
		else {
			l = m + 1;
		}
	}

	return -1;
}



template<typename T>
void Array<T>::assign_ptr(const std::vector<T>& vec, int& idx)
{
	if (this->size > 0) {
		this->ptr = (T*)&(vec[idx]);
		idx += this->size;
	}
}


