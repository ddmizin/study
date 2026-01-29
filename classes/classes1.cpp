#include <iostream>
#include <cstring>

struct S {
private:
    int l = 2;
    friend void g(S, int);
public:
    int x = 1;
    void f(int y) const {
        std::cout << x + y << '\n';
    }
    void ff() const {
        std::cout << this->x << '\n';
    }
    struct C {
        char c = 'a';
        char& k(){
            return c;
        }
    };
};

void g(S s, int y){
    std::cout << s.l + y << '\n';
}


class Complex {
public:
    double re = 0.0;
    double im = 0.0;
public:
    Complex() = default;
    Complex(double re);
    Complex(double re, double im): re(re), im(im) {}
    Complex(const Complex& other) = default;
};

Complex::Complex(double re): re(re) {}


class String {
public:
    char* arr;
    size_t sz;
    size_t cap;
public:
    String() {} // default constructor
    String(size_t n, char c): arr(new char[n + 1]), sz(n), cap(n + 1) {
        memset(arr, c, n);
        arr[sz] = '\0';
    }
    String(std::initializer_list<char> list)
            : arr(new char[list.size() + 1]), sz(list.size()), cap(sz + 1) {
        std::copy(list.begin(), list.end(), arr);
        arr[sz] = '\0';
    }
    String(const String& other): arr(new char[other.cap]), sz(other.sz), cap(other.cap) { // copy constructor
        memcpy(arr, other.arr, sz + 1);
    }
    ~String() {
        delete[] arr;
    }
};


int main(){
    S s;
    std::cout << s.x << '\n';
    s.f(5);
    s.ff();
    S::C c;
    std::cout << sizeof(s) << '\n';
    std::cout << c.k() << '\n';
    g(s, 5);

    Complex cmp = {7.0};
    Complex cmp2 = {5.4, 6.7};
    Complex cmp3;
    Complex cmp4 = cmp2;
    std::cout << cmp4.im;

    String str; // default initialization
    String str2(5, 'r');
    String str3 = {'a', 'b', 'c', 'd'};
    String str4 = str2;
}