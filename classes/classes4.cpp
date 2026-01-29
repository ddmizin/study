#include <iostream>

struct S {
    int x = 0;
    double y = 0.0;

    void f(int z) {
        std:: cout << x + z << '\n';
    }
};


enum E { // поля глобальные
    White,
    Gray,
    Black
};

enum class F : int8_t {
    White = 2,
    Gray = 3,
    Black = 9,
    Green
};


int main() {
    // int S::* p = &S::x; // указатель на поле класса
    // S s{1, 3.14};
    // std::cout << s.*p << '\n';
    // s.*p = 2;
    // S* ps = &s;
    // std::cout << ps->*p << '\n';
    // void (S::* pf) (int) = &S::f; // указатель на метод класса
    // (s.*pf)(3);
    // (ps->*pf)(5);

    E e = White;
    int ef = Gray;
    std::cout << ef << '\n';

    F f = F::Green;
    std::cout << static_cast<int>(f) << '\n';

}