#include <iostream>
#include <assert.h>

// Следующий код должен компилироваться и работать без ошибок. std запрещен

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

    Array& operator=(const Array& other) {
        if (this != &other) {
            delete[] data;
            sz = other.sz;
            cap = other.cap;
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

    void push_back(int value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            int* new_data = new int[new_cap];
            
            for (size_t i = 0; i < sz; ++i) {
                new_data[i] = data[i];
            }
            new_data[sz] = value;
            delete[] data;
            data = new_data;
            cap = new_cap;
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
            data = new_data;
            cap = new_cap;
        } else {
            for (size_t i = sz; i > 0; --i) {
                data[i] = data[i - 1];
            }
            data[0] = value;
        }
        ++sz;
    }

    Array operator+(int value) {
        Array b = *this;
        b.push_back(value);
        return b;
    }

    Array& operator+=(int value) {
        push_back(value);
        return *this;
    }

    friend Array operator+(int value, Array& other) {
        Array b = other;
        b.push_forward(value);
        return b;
    }

    Array operator*(int value) {
        if (value == 0) {
            Array b = *this;
            delete[] b.data;
            b.data = nullptr;
            b.sz = 0;
            b.cap = 0;
            return b;
        }
        Array b = *this;
        size_t new_sz = b.sz * 2;
        int* new_data = new int[new_sz];

        for (size_t i = 0; i < new_sz; ++i) {
            new_data[i] = data[i % sz];
        }
        delete[] b.data;
        b.data = new_data;
        b.sz = new_sz;
        b.cap = b.sz;
        return b;
    }

    friend Array operator*(int value, Array& other) {
        return other * value;
    }

    Array& operator-(int value) {
        size_t new_cap = 0;

        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                ++new_cap;
            }
        }
        int* new_data = new int[new_cap];
        size_t temp = 0;
        size_t new_sz = new_cap;

        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                new_data[temp++] = data[i];
            }
        }
        delete[] data;
        data = new_data;
        sz = new_sz;
        cap = new_cap;
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