#include <iostream>

template <typename T, typename U = int>
void swap(T& x, U& y) {
    T t = x;
    x = y;
    y = t;
}

template <typename T>
void f(T x) {
    std::cout << 1 << '\n';
}

void f(int x) {
    std::cout << 2 << '\n';
}


// Template specializations

template <typename T>
class vector {
    T* arr;
    size_t sz;
    size_t cap;
};


template <>
class vector<bool> {
    char* arr;
    size_t sz;
    size_t cap;
};

template <typename T, typename U>
struct S {};

template <typename W>
struct S<W, W> {};

template <typename W>
struct S<int, W> {};

template <typename W>
struct S<W, int> {};

template <>
struct S<int, int> {};

template <typename T>
struct A {};

template <typename T>
struct A<T&> {};

template <typename T>
struct A<const T> {};

int main() {
    int x = 2;
    int& z = x;
    long long y = 1;
    swap(x, y);
    std::cout << x << ' ' << y << '\n';
    
    f(x);

    S<int, int> s;
}