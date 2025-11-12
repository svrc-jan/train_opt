#pragma once

#include <vector>
#include <utility>


struct Vector_hasher {
    int operator()(const std::vector<int> &v) const {
        int hash = v.size();
        for(auto &i : v) {
            hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};


struct Pair_hasher {
    int operator()(const std::pair<int, int> &p) const {
        int hash = 2;
        
		hash ^= p.first + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= p.second + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        return hash;
    }
};
