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
            cap = other.cap;
            data = new int[cap];
            for (size_t i = 0; i < other.sz; ++i) {
                data[i] = other.data[i];
            }
            sz = other.sz;
        }
    }

    int operator[](size_t index) {
        return data[index];
    }

    const int operator[](size_t index) const {
        return data[index];
    }

    ~Array() {
        delete[] data;
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
            for (size_t i = 0; i < sz; ++i) {
                new_data[i + 1] = data[i];
            }
            new_data[0] = value;
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
        Array result = *this;
        result.push_back(value);
        return result;
    }

    friend Array operator+(int value, const Array& other) {
        Array result = other;
        result.push_forward(value);
        return result;
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
        size_t new_sz = b.sz * value;
        int* new_data = new int[new_sz];
        for (size_t i = 0; i < new_sz; ++i) {
            new_data[i] = b.data[i % sz];
        }
        b.sz = new_sz;
        b.cap = b.sz;
        delete[] b.data;
        b.data = new_data;
        return b;
    }

    friend Array operator*(int value, Array& other) {
        return other * value;
    }

    Array& operator-(int value) {
        size_t new_sz = 0;
        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                ++new_sz;
            }
        }

        size_t temp = 0;
        size_t new_cap = new_sz;
        int* new_data = new int[new_cap];
        for (size_t i = 0; i < sz; ++i) {
            if (data[i] != value) {
                new_data[temp++] = data[i];
            }
        }
        delete[] data;
        data = new_data;
        sz = new_sz;;
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