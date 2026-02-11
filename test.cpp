#include <iostream>

struct Base {
    virtual void f(int x) {
        std::cout << 1 << '\n';
    }
};

struct Derived: Base {
    void f(int x) override{
        std::cout << 2 << '\n';
    }
};

int main() {
    int x;
    Derived d;
    Base* b = &d;
    d.f(x);
    b->f(x);
}