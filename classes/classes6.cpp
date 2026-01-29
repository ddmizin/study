#include <iostream>

// Виртуальное наследование

struct Base {
    int x = 1;
};

struct Derived: Base {
    int y = 2;
};

void f(Base& b) {
    std::cout << b.x << '\n';
}

void g(Base* b) {
    std::cout << b->x << '\n';
}


struct Granny {
    int g = 0;
};

struct Mom: virtual Granny {
    int m = 0;
};

struct Dad: virtual Granny {
    int d = 0;
};

struct Son: Mom, Dad
{
    int s = 0;
};


int main(){
    Derived d;
    f(d);
    g(&d);
    Son s;
    std::cout << s.g << '\n';
    s.g = 4;
    std::cout << s.g << '\n';
    Granny& g = s;
    std::cout << g.g << '\n';
}