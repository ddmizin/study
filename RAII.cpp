#include <iostream>

// Resource Acquisition Is Initialization (Приобретение ресурса есть инициализация)

template <typename T>
struct UniquePtr {
    T* p;
    UniquePtr(T* p): p(p) {}
    UniquePtr(const UniquePtr&) = delete;
    
    ~UniquePtr() {
        delete p;
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    T& operator*() {
        return *p;
    }
};

void g(UniquePtr<int>& p) {
    if (*p == 0) {
        throw 1;
    }
}


void f(int x) {
    UniquePtr<int> s(new int(x));
    g(s);
}
 
int main() {
    try {
        f(0);
    } catch(...){
        std::cout << 111;
    }
}