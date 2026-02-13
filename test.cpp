#include <iostream>

template<class B>
struct Base {
    void f(){
        static_cast<B*>(this)->f();
    }
};

struct Derived: Base<Derived> {
    void f(){
        std::cout << 1;
    }
};

int main() {
    Derived d;
    Base<Derived>* b = &d;
    b->f();

}