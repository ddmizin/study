#include <iostream>
#include <vector>


int divide(int a, int b) {
    if (b == 0) {
        throw std::logic_error("Divide by zero!");
    }
    return a / b;
}


struct A {
    A() {
        std::cout << "A" << '\n';
    }
    A(const A&) {
        std::cout << "copy" << '\n';
    }

    ~A() {
        std::cout << "~A()" << '\n';
    }
};

void f(int x) {
    A a;
    if (x == 0) {
        throw a;
    }
}

int main() {
    try {
        std::cout << divide(1, 0) << '\n';
    } catch (std::logic_error& err) {
        std::cout << err.what() << '\n';
    }
    
    try {
        f(0);
        new int[400000000000];
    } catch(A& a) {
        std::cout << "Caught!" << '\n';
    } catch(std::bad_alloc& ex) {
        std::cout << ex.what() << '\n';
    }

    try {
        throw 1;
    } catch(double d) {
        std::cout << "double" << '\n';
    } catch(long long l) {
        std::cout << "long long" << '\n';
    } catch (...) {
        std::cout << "other" << '\n';
    }
}