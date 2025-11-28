#pragma once

#include <vector>

template<typename T>
struct Prio_queue_item
{
	T value;
	int prio;
};

template<typename T>
class Prio_queue
{
public:
	void push(const T value, int prio);
	bool pop();

	inline void push_back(const T value, int prio) { heap.emplace_back(value, prio); }
	inline void pop_back() { this->heap.pop_back(); }

	inline const Prio_queue_item<T>& top() const { return this->heap[0]; }
	inline const Prio_queue_item<T>& back() const { return this->heap.back(); }

	inline void reserve(size_t n) { this->heap.reserve(n); }

	inline void clear() { this->heap.clear(); }
	inline bool empty() const { return this->heap.size() == 0; }
	

private:
	std::vector<Prio_queue_item<T>> heap = {};

	void heapify(size_t idx);
};


template<typename T>
void Prio_queue<T>::push(const T value, int prio)
{
	size_t idx = this->heap.size();
	heap.emplace_back(value, prio);

	size_t parent = (idx - 1)/2;
	while (idx != 0 && heap[parent].prio > heap[idx].prio){
		std::swap(heap[parent], heap[idx]);

		idx = parent;
		parent = (idx - 1)/2;
	}
}


template<typename T>
bool Prio_queue<T>::pop()
{
	if (this->empty()) {
		return false;
	}

	this->heap[0] = this->heap.back();
	this->heap.pop_back();
	this->heapify(0);

	return true;
}

template<typename T>
void Prio_queue<T>::heapify(size_t idx)
{
	size_t left = 2*idx + 1;
	size_t right = 2*idx + 2;

	size_t smallest = idx;

	if (left < this->heap.size() && this->heap[left].prio < this->heap[smallest].prio) {
		smallest = left;
	}

	if (right < this->heap.size() && this->heap[right].prio < this->heap[smallest].prio) {
		smallest = right;
	}

	if (smallest != idx) {
		std::swap(this->heap[idx], this->heap[smallest]);
		this->heapify(smallest);
	}
}