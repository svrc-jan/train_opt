#pragma once

#include <cstdint>
#include <cstring>

class Flag
{
public:
	Flag(const int n_items=0);
	~Flag();

	inline void resize_for_items(const int n_items);
	
	inline bool get(int item);
	inline void set_true(int item);
	inline void set_false(int item);

	inline void clear();
	inline void update(const Flag& other);

	static inline int get_required_size(const int n_items);

private:
	int size = 0;
	uint64_t* ptr = nullptr;

	inline void alloc_for_size();
	inline void free();

};


Flag::Flag(const int n_items)
{
	if (n_items > 0) {
		this->resize_for_items(n_items);
	}
}


Flag::~Flag()
{
	this->free();
}

void Flag::resize_for_items(const int n_items)
{
	this->size = this->get_required_size(n_items);
	this->alloc_for_size();
}


int Flag::get_required_size(const int n_items)
{
	return (n_items - 1)/64 + 1;
}


void Flag::alloc_for_size()
{
	if (this->ptr != nullptr) {
		this->free();
	}
	this->ptr = new uint64_t[this->size];
}


void Flag::free()
{
	delete [] this->ptr;
	this->ptr = nullptr;
}


void Flag::clear()
{
	memset(this->ptr, 0, sizeof(uint64_t)*this->size);
}


bool Flag::get(int item)
{
	return (this->ptr[item/64] & (1 << (item % 64))) != 0;
}


void Flag::set_true(int item)
{
	this->ptr[item/64] |= (1 << (item % 64));
}


void Flag::set_false(int item)
{
	this->ptr[item/64] &= ~(1 << (item % 64));
}


void Flag::update(const Flag& other)
{
	for (int i = 0; i < this->size; i++) {
		this->ptr[i] |= other.ptr[i];
	}
}