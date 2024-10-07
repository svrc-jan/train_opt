#pragma once

template<typename T>
void print_vec(std::ostream& os, const std::vector<T>& vec)
{
	os << "[";
	for (size_t i = 0; i < vec.size(); i++) {
		os << vec[i] << (i < vec.size() - 1 ? ", " : "");

#if defined VEC_TRUNCATE_SIZE && VEC_TRUNCATE_SIZE > 0 
		if (i >= VEC_TRUNCATE_SIZE) {
			os << "...";
			break;
		}
#endif

	}
	os << "]";
}

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& vec) {
    print_vec(os, vec);
	return os;
}
