// Следующий код должен компилироваться и работать без ошибок. std запрещен
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
            cap = sz;
            data = new int[cap];

            for (size_t i = 0; i < sz; ++i) {
                data[i] = other.data[i];
            }
        }
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            sz = other.sz;
            cap = sz;
            delete[] data;
            data = new int[cap];

            for (size_t i = 0; i < sz; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ~Array() {
        delete[] data;
    }

    void push_back(int value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            int* new_data = new int[new_cap];

            for (size_t i = 0; i < sz; ++i) {
                new_data[i] = data[i];
            }
            new_data[sz] = value;
            delete[] data;
            cap = new_cap;
            data = new_data;
        } else {
            data[sz] = value;
        }
        ++sz;
    }

    void push_forward(int value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            int* new_data = new int[new_cap];
            new_data[0] = value;

            for (size_t i = 0; i < sz; ++i) {
                new_data[i + 1] = data[i];
            }
            delete[] data;
            cap = new_cap;
            data = new_data;
        } else {
            for (size_t i = sz; i > 0; --i) {
                data[i] = data[i - 1];
            }
            data[0] = value;
        }
        ++sz;
    }

    Array operator+(int value) const {
        Array b = *this;
        b.push_back(value);
        return b;
    }

    friend Array operator+(int value, const Array& other) {
        Array b = other;
        b.push_forward(value);
        return b;
    }

    int& operator[](size_t index) {
        return data[index];
    }

    const int& operator[](size_t index) const {
        return data[index];
    }

    Array& operator*(int value) {
        if (value == 0) {
            delete[] data;
            data = nullptr;
            sz = 0;
            cap = 0;
            return *this;
        }
        size_t new_sz = sz * value;
        int* new_data = new int[new_sz];
        
        for (size_t i = 0; i < new_sz; ++i) {
            new_data[i] = data[i % sz];
        }
        
        delete[] data;
        data = new_data;
        sz = new_sz;
        cap = new_sz;
        return *this;
    }

    friend Array operator*(int value, Array& other) {
        return other * value;
    }

    Array& operator-(int value) {
        int new_sz = 0;

        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                ++new_sz;
            }
        }
        int new_cap = new_sz;
        size_t temp = 0;
        int* new_data = new int[new_cap];
        
        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                new_data[temp] = data[i];
                ++temp;
            }
        }
        delete[] data;
        cap = new_cap;
        sz = new_sz;
        data = new_data;
        return *this;
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
    std::cout << "All tests complited";
}