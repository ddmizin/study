// Слудующий код должен компилироваться и работать без ошибок. std запрещен
#include <assert.h>
#include <iostream>


class Array {
    size_t sz;
    size_t cap;
    int* data;
public:
    Array(): sz(0), cap(0), data(nullptr) {}

    Array(const Array& other): sz(0), cap(0), data(nullptr) {
        if (other.sz > 0) {
            sz = other.sz;
            cap = other.cap;
            data = new int[cap];
            for (size_t i = 0; i < sz; ++i) {
                data[i] = other.data[i];
            }
        }
    }

    ~Array() {
        delete[] data;
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            sz = other.sz;
            cap = other.cap;
            delete[] data;
            int* new_data = new int[cap];
            for (size_t i = 0; i < sz; ++i) {
                new_data[i] = other.data[i];
            }
            data = new_data;
        }
        return *this;
    }

    int operator[](size_t index) {
        return data[index];
    }

    const int operator[](size_t index) const {
        return data[index];
    }

    void push_back(int val) {
        if (sz == cap) {
            size_t new_cap = cap * 2;
            int* new_data = new int[new_cap];
            for (size_t i = 0; i < sz; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[sz] = val;
        ++sz;
    }

    void push_forward(int value) {
        int* new_data;
        size_t new_cap;
        if (sz == cap) {
            new_cap = cap * 2;
            new_data = new int[new_cap];
        } else{
            new_cap = cap;
            new_data = new int[new_cap];
        }
        new_data[0] = value;
        for (size_t i = 0; i < sz; ++i){
            new_data[i+1] = data[i];
        }
        delete[] data;
        data = new_data;
        cap = new_cap;
        ++sz;
    }
    
    Array operator+(int val){
        Array b = *this;
        b.push_back(val);
        return b;
    }

    friend Array operator+(int val, Array& arr){
        Array b = arr;
        b.push_forward(val);
        return b;
    }

    Array& operator-(int val){
        size_t new_sz = 0;
        for (size_t i = 0; i < sz; ++i){
            if(data[i] != val){
                ++new_sz;
            }
        }
        size_t new_cap = new_sz;
        size_t  temp = 0;
        int* new_data = new int[new_cap];
        for (size_t i = 0; i < sz; ++i){
            if (data[i] != val){
                new_data[temp] = data[i];
                ++temp;
            }
        }
        sz=new_sz;
        cap=new_cap;
        delete[] data;
        data = new_data;
        return *this;
    }

    Array& operator*(int val){
        if(sz!=0 && val == 0){
        size_t new_cap = sz*val;
        int* new_data = new int[new_cap];
        for (size_t i = 0; i < new_cap; ++i){
            new_data[i] = data[i%sz];
        }
        sz = new_cap;
        cap = new_cap;
        delete[] data;
        data = new_data;

        } else{
            sz = 0;
            cap = 0;
            if(data != nullptr){delete[] data;}
            data = nullptr;
        }
        return *this;
    }
    friend Array& operator*(int val, Array& other){
        if(other.sz!=0 && val == 0){
        size_t new_cap = other.sz*val;
        int* new_data = new int[new_cap];
        for (size_t i = 0; i < new_cap; ++i){
            new_data[i] = other.data[i%other.sz];
        }
        other.sz = new_cap;
        other.cap = new_cap;
        delete[] other.data;
        other.data = new_data;

        } else{
            other.sz = 0;
            other.cap = 0;
            if(other.data != nullptr){delete[] other.data;}
            other.data = nullptr;
        }
        return other;
    }
};

int main() {
    Array a;

    a = a + 1;
    
    a = a + 2;
    

    assert(a[0] == 1);
    assert(a[1] == 2);

    a = 0 + a;

    assert(a[0] == 0);
    assert(a[1] == 1);
    assert(a[2] == 2);

    a = 2 * a;

    assert(a[0] == 0);
    assert(a[1] == 1);
    assert(a[2] == 2);
    assert(a[3] == 0);
    assert(a[4] == 1);
    assert(a[5] == 2);

    a = a - 1;

    assert(a[0] == 0);
    assert(a[1] == 2);
    assert(a[2] == 0);
    assert(a[3] == 2);
}