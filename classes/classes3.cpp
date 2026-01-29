#include <iostream>
#include <compare>
#include <vector>
#include <algorithm>

// Operators overloading

struct Complex {
    double re = 0.0;
    double im = 0.0;

    Complex& operator=(const Complex& other) & { // оператор '=' теперь определён только для lvalue (&& для rvalue)
        re = other.re;
        im = other.im;
        return *this;
    }

    Complex() = default;
    Complex(double re): re(re) {}
    Complex(double re, double im): re(re), im(im) {}

    Complex& operator+=(const Complex& other) {
        re += other.re;
        im += other.im;
        return *this;
    }
};

Complex operator+(const Complex& a, const Complex& b){
    Complex result = a;
    result += b;
    return result;
}

bool operator<(const Complex& a, const Complex& b) {
    return a.re < b.re || a.re == b.re && a.im < b.im;
}

bool operator>(const Complex& a, const Complex& b) {
    return b < a;
}

bool operator>=(const Complex& a, const Complex& b) {
    return !(a < b);
}

bool operator<=(const Complex& a, const Complex& b) {
    return !(a > b);
}

bool operator==(const Complex& a, const Complex& b) {
    return a.re == b.re && a.im == b.im;
}


struct Complex2 {
    double re = 0.0;
    double im = 0.0;

    Complex2& operator=(const Complex2& other) & { // оператор '=' теперь определён только для lvalue (&& для rvalue)
        re = other.re;
        im = other.im;
        return *this;
    }

    Complex2() = default;
    Complex2(double re): re(re) {}
    Complex2(double re, double im): re(re), im(im) {}

    auto operator<=>(const Complex2& other) const = default;

    Complex2& operator+=(const Complex2& other) {
        re += other.re;
        im += other.im;
        return *this;
    }
};

Complex2 operator+(const Complex2& a, const Complex2& b){
    Complex2 result = a;
    result += b;
    return result;
}


struct UserId {
    int value = 0;

    UserId& operator++() { // ++val
        ++value;
        return *this;
    }

    UserId operator++(int) { // val++
        UserId copy = *this;
        ++value;
        return copy;
    }
};


struct Greater {
    bool operator()(int x, int y) const{
        return x > y;
    }
};


int main() {
    double x1 = 4.6;
    const double y1 = 6.8;
    double z1 = 4.8;
    double x2 = 7.2;
    const double y2 = 2.8;
    double z2 = 9.3;
    Complex c1(x1, y1);
    Complex c2(x2, y2);
    Complex c3 = c1 + c2;
    std::cout << c1.re << ' ' << c1.im << '\n';
    std::cout << c2.re << ' ' << c2.im << '\n';
    std::cout << c3.re << ' ' << c3.im << '\n';
    Complex c;
    c1 = c + 3.14;
    c2 = 3.14 + c;
    std::cout << c1.re << ' ' << c1.im << '\n';
    std::cout << c2.re << ' ' << c2.im << '\n';
    c2 += c3;
    std::cout << c2.re << ' ' << c2.im << '\n';
    std::cout << c3.re << ' ' << c3.im << '\n';
    std::cout << (c2 <= c3) << '\n';
    std::cout << (c2 == c3) << '\n';
    c3 = c2;
    std::cout << (c2 <= c3) << '\n';
    std::cout << (c2 != c3) << '\n';

    Complex2 cp1(x1, y1);
    Complex2 cp2(x2, y2);
    std:: cout << (cp1 != cp2);

    UserId u1(6);
    u1++;
    std::cout << u1.value;

    std::vector<int> v(10);
    std::sort(v.begin(), v.end(), Greater());
}