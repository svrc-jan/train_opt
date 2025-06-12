#include "binary_vector.hpp"


using Bin_vec = Binary_vector;


// default constructor
Bin_vec::Binary_vector()
{

}


// move constructor
Bin_vec::Binary_vector(Bin_vec&& other)
{
	this->size = other.size;
	this->arr = other.arr;

	other.size = 0;
	other.arr = nullptr;
}


// copy constructor
Bin_vec::Binary_vector(const Bin_vec& other)
{
	this->create_arr(other.size);
	std::memcpy(this->arr, other.arr, sizeof(bin_vec_block_t)*other.size);
}


Bin_vec::Binary_vector(const int n_bits)
{
	this->create_arr(Bin_vec::get_req_size(n_bits));
}


Bin_vec::Binary_vector(const int n_bits, const vector<int>& idx)
{
	this->create_arr(Bin_vec::get_req_size(n_bits));
	this->fill(idx);
}


Bin_vec::~Binary_vector()
{
	if (arr != nullptr) {
		delete[] arr;
	}
}


void Bin_vec::create_arr(int size)
{
	this->size = size;
	arr = new bin_vec_block_t[this->size];
}


void Bin_vec::fill(const vector<int>& idx)
{
	for (const int& x : idx) {
		arr[x/RES_BLOCK_SIZE] = (1 << (x % RES_BLOCK_SIZE));
	}
}


vector<bool> Bin_vec::to_vector()
{
	vector<bool> x;
	x.reserve(this->size*RES_BLOCK_SIZE);
	for (int i = 0; i < this->size; i++) {
		bin_vec_block_t block = this->arr[i];
		for (int j = 0; j < RES_BLOCK_SIZE; j++) {
			x.push_back(block & 1);
			block >>= 1;

			if ((i == this->size - 1) && (block == 0)) {
				break;
			}
		}
	}

	return x;
}


Bin_vec Bin_vec::operator~() const
{
	Bin_vec x;
	x.create_arr(this->size);
	
	for (int i = 0; i < this->size; i++) {
		x.arr[i] = ~this->arr[i];
	}

	return x;
}


Bin_vec Bin_vec::operator&(const Bin_vec& other) const
{
	Bin_vec x;
	x.create_arr(this->size);
	
	for (int i = 0; i < this->size; i++) {
		x.arr[i] = this->arr[i] & other.arr[i];
	}

	return x;
}


Bin_vec Bin_vec::operator|(const Bin_vec& other) const
{
	Bin_vec x;
	x.create_arr(this->size);
	
	for (int i = 0; i < this->size; i++) {
		x.arr[i] = this->arr[i] | other.arr[i];
	}

	return x;
}


Bin_vec& Bin_vec::operator&=(const Bin_vec& other)
{
	for (int i = 0; i < this->size; i++) {
		this->arr[i] &= other.arr[i];
	}

	return *this;
}


Bin_vec& Bin_vec::operator|=(const Bin_vec& other)
{
	for (int i = 0; i < this->size; i++) {
		this->arr[i] |= other.arr[i];
	}

	return *this;
}


Bin_vec Bin_vec::or_neg(const Bin_vec& other) const
{
	Bin_vec x;
	x.create_arr(this->size);
	
	for (int i = 0; i < this->size; i++) {
		x.arr[i] = this->arr[i] | ~other.arr[i];
	}

	return x;
}


Bin_vec Bin_vec::and_neg(const Bin_vec& other) const
{
	Bin_vec x;
	x.create_arr(this->size);
	
	for (int i = 0; i < this->size; i++) {
		x.arr[i] = this->arr[i] & ~other.arr[i];
	}

	return x;
}


Bin_vec& Bin_vec::or_neg_eq(const Bin_vec& other)
{
	for (int i = 0; i < this->size; i++) {
		this->arr[i] &= ~other.arr[i];
	}

	return *this;
}


Bin_vec& Bin_vec::and_neg_eq(const Bin_vec& other)
{
	for (int i = 0; i < this->size; i++) {
		this->arr[i] &= other.arr[i];
	}

	return *this;
}

bool Bin_vec::operator&&(const Bin_vec& other) const
{
	bin_vec_block_t x = 0;

	for (int i = 0; i < this->size; i++) {
		x |= (this->arr[i] & other.arr[i]);
	}

	return x != 0;
}
