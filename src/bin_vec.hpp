#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include <iostream>
#include <bit>

namespace bin_vec {
	using block_t = uint64_t;
	const int block_n_bits = std::numeric_limits<block_t>::digits;

	void set_req_n_blocks(int n_bits);
	int get_n_blocks();

	void clear(block_t* t);
	void fill(block_t* t, const std::vector<int>& bit_idx);

	void not_(block_t* t, const block_t* a);
	void and_(block_t* t, const block_t* a, const block_t* b);
	void or_(block_t* t, const block_t* a, const block_t* b);
	void and_not(block_t* t, const block_t* a, const block_t* b);
	void or_not(block_t* t, const block_t* a, const block_t* b);

	int count_overlap(const block_t* a, const block_t* b);

	std::vector<int> to_vector(const block_t* x);
}
