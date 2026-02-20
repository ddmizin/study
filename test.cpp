#include <iostream>

template <typename T>
class Vector {
    size_t sz;
    size_t cap;
    T* data;

    void safe_copy(T* dst, const T* src, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            new (dst + i) T(src[i]);
        }
    }

    void safe_move(T* dst, const T* src, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            new (dst + i) T(std::move(src[i]));
        }
    }

    void destroy_range(T* start, T* end) {
        for (T* ptr = start; ptr != end; ++ptr) {
            ptr->~T();
        }
    }

    void check_index(size_t index) {
        if (index >= sz) {
            throw std::out_of_range("Vector::at - index out of range!");
        }
    }

    void reallocate(size_t new_cap) {
        if (new_cap <= cap) {
            return;
        }
        T* new_data = static_cast<T*>(operator new[](new_cap * sizeof(T)));
        
        try {
            for (size_t i = 0; i < sz; ++i) {
                new (new_data + i) T(std::move(data[i]));
                data[i].~T();
            }
        } catch (...) {
            destroy_range(new_data, new_data + sz);
            operator delete[](new_data);
            throw;
        }
        operator delete[](data);
        data = new_data;
        cap = new_cap;
    }

public:
    Vector(): sz(0); cap(0); data(nullptr) {}

    Vector(size_t n): sz(n), cap(n), data(static_cast<T*>(operator new[](n * sizeof(T)))) {
        for (size_t i = 0; i < n; ++i) {
            new (data + i) T();
        }
    }

    Vector(size_t n, const T& value): sz(n), cap(n), data(static_cast<T*>(operator new[](n * sizeof(T)))) {
        for (size_t i = 0; i < n; ++i) {
            new (data + i) T(value);
        }
    }

    Vector(const Vector& other): sz(other.sz), cap(other.cap), data(static_cast<T*>(operator new[](other.cap * sizeof(T)))) {
        try {
            safe_copy(data, other.data, other.sz);
        } catch (...) {
            destroy_range(data, data + sz);
            operator delete[](data);
            throw;
        }
    }

    Vector(std::initializer_list<T> list): sz(list.size()), cap(list.size()), data(static_cast<T*>(operator new[](list.size() * sizeof(T)))) {
        try {
            const T* src = list.begin();
            for (size_t i = 0; i < list.size(); ++i) {
                new (data + i) T(src[i]);
            }
        } catch (...) {
            destroy_range(data, data + sz);
            operator delete[](data);
            throw;
        }
    }

    ~Vector() noexcept {
        destroy_range(data, data + sz);
        operator delete[](data);
    }

    void swap(Vector& other) {
        std::swap(data, other.data);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }

    friend void swap(Vector& a, Vector& b) {
        a.swap(b);
    }

    size_t size() {
        return sz;
    }

    size_t capacity() {
        return cap;
    }

    bool empty() {
        return sz == 0;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    T& at(size_t index) {
        check_index(index);
        return data[index];
    }

    const T& at(size_t index) const {
        check_index(index);
        return data[index];
    }

    void reserve(size_t new_cap) {
        if (new_cap > cap) {
            reallocate(new_cap);
        }
    }

    void push_back(const T& value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            reallocate(new_cap);
        }
        new (data + sz) T(value);
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            --sz;
            data[sz].~T();
        }
    }

    void clear() {
        destroy_range(data, data + sz);
        sz = 0;
    }

    void resize(size_t new_sz, const T& value = T()) {
        if (new_sz > sz) {
            if (new_sz > cap) {
                size_t new_cap = std::max(new_sz, cap * 2);
                reallocate(new_cap);
            }
            for (size_t i = sz; i < new_sz; ++i) {
                new (data + i) T(value);
            }
        } else if (new_sz < sz) {
            destroy_range(data + new_sz, data + sz);
        }
        sz = new_sz;
    }

    
};

int main() {
    return 0;
}