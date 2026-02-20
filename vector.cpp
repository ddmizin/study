#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <utility>

template <typename T>
class Vector {
    T* data;
    size_t sz;
    size_t cap;

    Vector(size_t sz, size_t cap): data(cap > 0 ? static_cast<T*>(operator new[](cap * sizeof(T))) : nullptr), sz(sz), cap(cap) {
        for (size_t i = 0; i < sz; ++i) {
            new (data + i) T();
        }
    }

    void check_index(size_t index) const {
        if (index >= sz) {
            throw std::out_of_range("Vector::at - index out of range!");
        }
    }

    void safe_copy(T* dest, const T* src, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            new (dest + i) T(src[i]);
        }
    }

    void safe_move(T* dest, T* src, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            new (dest + i) T(std::move(src[i]));
            src[i].~T();
        }
    }

    void destroy_range(T* start, T* end) {
        for (T* ptr = start; ptr != end; ++ptr) {
            ptr->~T();
        }
    }

    void reallocate(size_t new_cap) {
        if (new_cap <= cap) {
            return;
        }

        T* new_arr = static_cast<T*>(operator new[](new_cap * sizeof(T)));
        
        try {
            for (size_t i = 0; i < sz; ++i) {
                new (new_arr + i) T(std::move(data[i]));
                data[i].~T();
            }
        } catch (...) {
            destroy_range(new_arr, new_arr + sz);
            operator delete[](new_arr);
            throw;
        }

        operator delete[](data);
        data = new_arr;
        cap = new_cap;
    }

public:
    Vector(): data(nullptr), sz(0), cap(0) {}

    explicit Vector(size_t n): Vector(n, n) {
    }

    Vector(size_t n, const T& value): Vector(0, n) {
        for (size_t i = 0; i < n; ++i) {
            new (data + i) T(value);
        }
        sz = n;
    }

    Vector(const Vector& other): Vector(0, other.cap) {
        try {
            safe_copy(data, other.data, other.sz);
            sz = other.sz;
        } catch (...) {
            destroy_range(data, data + sz);
            operator delete[](data);
            throw;
        }
    }

    Vector(std::initializer_list<T> list): Vector(0, list.size()) {
        try {
            const T* src = list.begin();
            for (size_t i = 0; i < list.size(); ++i) {
                new (data + i) T(src[i]);
            }
            sz = list.size();
        } catch (...) {
            destroy_range(data, data + sz);
            operator delete[](data);
            throw;
        }
    }

    Vector(Vector&& other) noexcept : data(other.data), sz(other.sz), cap(other.cap) {
        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    ~Vector() noexcept {
        destroy_range(data, data + sz);
        operator delete[](data);
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            destroy_range(data, data + sz);
            operator delete[](data);
            
            data = other.data;
            sz = other.sz;
            cap = other.cap;
            
            other.data = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    void swap(Vector& other) noexcept {
        std::swap(data, other.data);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    friend void swap(Vector& v1, Vector& v2) noexcept {
        v1.swap(v2);
    }

    size_t size() const noexcept { 
        return sz; 
    }

    size_t capacity() const noexcept { 
        return cap; 
    }

    bool empty() const noexcept { 
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

    void push_back(T&& value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            reallocate(new_cap);
        }
        new (data + sz) T(std::move(value));
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            --sz;
            data[sz].~T();
        }        
    }

    void clear() noexcept {
        destroy_range(data, data + sz);
        sz = 0;
    }

    void resize(size_t new_sz, const T& value = T()) {
        if (new_sz > sz) {
            if (new_sz > cap) {
                size_t new_cap = std::max(cap * 2, new_sz);
                reallocate(new_cap);
            }
            
            for (size_t i = sz; i < new_sz; ++i) {
                new (data + i) T(value);
            }
        } 
        else if (new_sz < sz) {
            destroy_range(data + new_sz, data + sz);
        }
        
        sz = new_sz;
    }

    void shrink_to_fit() {
        if (sz < cap) {
            if (sz == 0) {
                destroy_range(data, data + sz);
                operator delete[](data);
                data = nullptr;
                cap = 0;
            } 
            else {
                T* new_arr = static_cast<T*>(operator new[](sz * sizeof(T)));
                try {
                    for (size_t i = 0; i < sz; ++i) {
                        new (new_arr + i) T(std::move(data[i]));
                        data[i].~T();
                    }
                } catch (...) {
                    destroy_range(new_arr, new_arr + sz);
                    operator delete[](new_arr);
                    throw;
                }
                
                operator delete[](data);
                data = new_arr;
                cap = sz;
            }
        }
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("Vector::front - vector is empty!");
        }
        return data[0];
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Vector::front - vector is empty!");
        }
        return data[0];
    }

    T& back() {
        if (empty()) {
            throw std::out_of_range("Vector::back - vector is empty!");
        }
        return data[sz - 1];
    }

    const T& back() const {
        if (empty()) {
            throw std::out_of_range("Vector::back - vector is empty!");
        }
        return data[sz - 1];
    }

    T* begin() noexcept { 
        return data; 
    }

    const T* begin() const noexcept { 
        return data; 
    }

    T* end() noexcept { 
        return data + sz; 
    }

    const T* end() const noexcept { 
        return data + sz; 
    }
};

int main() {}