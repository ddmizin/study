// Реализуйе  фукнцию toOneArg с слудующим поведением. std запрещен
#include <assert.h>
#include <iostream>

int plus(int a, int b){
    return a + b;
}

int minus(int a, int b){
    return a - b;
}

class toOneArg {
    int (*func)(int, int);
    int val;
public: 
    toOneArg(int (*func)(int, int), int x): func(func), val(x) {}

    int operator()(int v2) {
        return func(v2, val);
    }
};

int main(int, char**){
    auto increment = toOneArg(plus, 1);
    assert(increment(2) == 3);

    auto decrement = toOneArg(minus, 1);
    assert(decrement(2) == 1);
    return 0;
}
