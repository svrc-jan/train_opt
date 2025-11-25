#include "chrono_tracker.hpp"

#include <cstring>

Chrono_tracker::Chrono_tracker()
{

}


void Chrono_tracker::set_size(int size)
{
	this->size = size;
	this->order.reserve(size);
	this->idx.resize(size);
}


void Chrono_tracker::reset()
{
	this->order.clear();
	memset(this->idx.data(), -1, sizeof(int)*this->size);
}


bool Chrono_tracker::add(int item)
{
	if (this->idx[item] == -1) {
		this->idx[item] = this->order.size();
		this->order.push_back(item);
		
		return true;
	}

	return false;
}


bool Chrono_tracker::is_done()
{
	return (int)this->order.size() == this->size;
}
