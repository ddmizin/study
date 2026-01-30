#include <iostream>
#include <vector>
#include <numbers>

// Виртуальные функции и полиморфизм


struct Base {
    virtual void f() const {
        std::cout << 1 << '\n';
    }

    virtual ~Base() = default;
};

struct Derived: Base {
    int* p = new int(0);

    void f() const override {
        std::cout << 2 << '\n';
    }

    ~Derived() {
        delete p;
    }
};


// Абстрактные классы

struct Shape {
    virtual double area() const = 0 ; // pure virtual function
    virtual ~Shape() = default;
};

double Shape::area() const {
    return 0;
}

struct Square: Shape {
    double a;
    
    Square(double a): a(a) {}
    
    double area() const override {
        return a * a;
    }
};

struct Circle: Shape {
    double r;
    
    Circle(double r): r(r) {}
    
    double area() const override {
        return std::numbers::pi * r * r;
    }
};

int main() {
    Derived d;
    Base& b = d;
    Base* bp = new Derived();
    b.f();
    delete bp;
    std::cout << typeid(b).name() << '\n';
    int x;
    std::cout << typeid(x).name() << '\n';

    Square sq(1.0);
    Circle sc(2.0);
    Shape& s1 = sq;
    Shape& s2 = sc;
    s1.area();
    s2.area();

    std::vector<Shape*> v;
    v.push_back(new Square(1.0));
    v.push_back(new Circle(1.5));

    for (Shape* s: v) {
        std::cout << s->area() << '\n';
        delete s;
    }

    std::cout << sc.Shape::area() << '\n';
    std::cout << typeid(s1).name() << '\n';

    Derived* pd = dynamic_cast<Derived*>(&b);
    std::cout << typeid(pd).name() << '\n';
}