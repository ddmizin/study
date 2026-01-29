#include <iostream>

// Наследование

struct Granny {
    friend int main();
public:
    int x = 1;

    void f() {}
};

struct Mom: private Granny {
    friend struct Son;
    int y = 2;

    void g() {
        std::cout << x << '\n';
    }
};

struct Son: Mom {
    int z = 3;

    void h(Granny& g) {
        std::cout << g.x << '\n';
    }
};


struct Base {
    int x;

    void f(int) {
        std::cout << 1;
    }
};

struct Derived: Base {
    int f(double) {
        std::cout << 2;
        return 0;
    }
};

int main() {
    // Son s;
    // Granny g;
    // s.h(g);

    Derived d;
    d.f(0);
}