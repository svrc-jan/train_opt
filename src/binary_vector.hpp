#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <limits>


using std::vector;

using bin_vec_block_t = uint64_t;
#define RES_BLOCK_SIZE std::numeric_limits<bin_vec_block_t>::digits

class Binary_vector
{
private:
	int size = 0;
	bin_vec_block_t* arr = nullptr;

	void create_arr(const int n_res);


public:
	Binary_vector();

	Binary_vector(Binary_vector&& other);
	Binary_vector& operator=(Binary_vector&& other);

	Binary_vector(const Binary_vector& other);
	Binary_vector& operator=(Binary_vector&& other);

	Binary_vector(const int n_bits);
	Binary_vector(const int n_bits, const vector<int>& idx);

	~Binary_vector();

	inline static int get_req_size(int n_bits)
	{
		return (n_bits-1)/RES_BLOCK_SIZE + 1;
	}

	void fill(const vector<int>& idx);

	vector<bool> to_vector();

	Binary_vector operator~() const;
	
	Binary_vector operator&(const Binary_vector& other) const;
	Binary_vector operator|(const Binary_vector& other) const;

	Binary_vector& operator&=(const Binary_vector& other);
	Binary_vector& operator|=(const Binary_vector& other);

	Binary_vector or_neg(const Binary_vector& other) const;
	Binary_vector and_neg(const Binary_vector& other) const;

	Binary_vector& or_neg_eq(const Binary_vector& other);
	Binary_vector& and_neg_eq(const Binary_vector& other);

	bool operator&&(const Binary_vector& other) const;

};
