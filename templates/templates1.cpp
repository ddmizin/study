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

// шаблонная рекурсия

template <int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 1;
};


template <int N, int D>
struct IsPrimeHelper {  
    static constexpr bool value = N%D == 0 ? false : IsPrimeHelper<N, D - 1>::value;
};

template <int N>
struct IsPrimeHelper<N, 1> {  
    static constexpr bool value = true;
};

template <int N>
struct IsPrime {
    static constexpr bool value = IsPrimeHelper<N, N - 1>::value;
};

template <>
struct IsPrime<1> {
    static constexpr bool value = false;
};

template <int N>
constexpr bool is_prime = IsPrime<N>::value; // шаблонная переменная

int main() {
    int x = 2;
    int& z = x;
    long long y = 1;
    swap(x, y);
    std::cout << x << ' ' << y << '\n';
    
    f(x);

    S<int, int> s;
    
    static_assert(is_prime<257>); // error if expression = false
    std::cout << Fibonacci<20>::value << '\n';
    std::string result = is_prime<257> == 1 ? "Is_prime" : "Is_not_prime";
    std::cout << result << '\n';
}