#include "bin_vec.hpp"

namespace bin_vec
{
	int n_blocks = 0;
	void set_req_n_blocks(int n_bits)
	{
		n_blocks = std::max(n_blocks, (n_bits - 1)/block_n_bits + 1);
	}

	int get_n_blocks()
	{
		return n_blocks;
	}

	void clear(block_t* t)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = 0;
		}
	}

	void copy(block_t* t, const block_t* a)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = a[i];
		}
	}

	void fill(block_t* t, const std::vector<int>& bit_idx)
	{
		for (const auto& i : bit_idx) {
			t[i/block_n_bits] |= (1 << i % block_n_bits);
		}
	}

	void not_(block_t* t, const block_t* a)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = ~a[i];
		}
	}

	void and_(block_t* t, const block_t* a, const block_t* b)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = a[i] & b[i];
		}
	}

	void or_(block_t* t, const block_t* a, const block_t* b)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = a[i] | b[i];
		}
	}

	void and_not(block_t* t, const block_t* a, const block_t* b)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = a[i] & ~b[i];
		}
	}

	void or_not(block_t* t, const block_t* a, const block_t* b)
	{
		for (int i = 0; i < n_blocks; i++) {
			t[i] = a[i] | ~b[i];
		}
	}

	int count_overlap(const block_t* a, const block_t* b)
	{
		int c = 0;
		
		for (int i = 0; i < n_blocks; i++) {
			c += std::__popcount(a[i] & b[i]);
		}

		return c;
	}

	std::vector<int> to_vector(const block_t* x)
	{
		std::vector<int> v;
		
		for (int i = 0; i < n_blocks; i++) {
			block_t blk = x[i];
			for (int j = 0; j < block_n_bits; j++) {
				v.push_back((blk & (1 << j)) != 0);

				// if (blk == 0 && i == n_blocks - 1) {
				// 	break;
				// }
			}
		}

		return v;
	}

}