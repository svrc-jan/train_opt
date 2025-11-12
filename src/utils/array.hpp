#pragma once

template<typename T>
struct Array
{
	T* ptr = nullptr;
	int size = 0;

	inline T* begin() { return this->ptr; }
	inline T* end() { return this->ptr + this->size; }
	inline T& operator[](size_t idx) { return this->ptr[idx]; }

	inline const T* begin() const { return this->ptr; }
	inline const T* end() const { return this->ptr + this->size; }
	inline const T& operator[](size_t idx) const { return this->ptr[idx]; }
};