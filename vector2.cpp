#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

template <typename T>
class Vector {
    T* arr;
    size_t sz;
    size_t cap;

    Vector(size_t sz, size_t cap): arr(new T[cap]), sz(sz), cap(cap) {}
    
    
public:
    Vector(): arr(nullptr), sz(0), cap(0) {}

    explicit Vector(size_t n): Vector(n, n) {
        if (n > 0) {
            std::memset();
        }
    }



};

int main() {}