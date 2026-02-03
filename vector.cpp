#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <utility>

template <typename T>
class Vector {
    T* arr;
    size_t sz;
    size_t cap;

    Vector(size_t sz, size_t cap): arr(cap > 0 ? static_cast<T*>(operator new[](cap * sizeof(T))) : nullptr), sz(sz), cap(cap) {
        for (size_t i = 0; i < sz; ++i) {
            new (arr + i) T();
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
                new (new_arr + i) T(std::move(arr[i]));
                arr[i].~T();
            }
        } catch (...) {
            destroy_range(new_arr, new_arr + sz);
            operator delete[](new_arr);
            throw;
        }

        operator delete[](arr);
        arr = new_arr;
        cap = new_cap;
    }

public:
    Vector(): arr(nullptr), sz(0), cap(0) {}

    explicit Vector(size_t n): Vector(n, n) {
    }

    Vector(size_t n, const T& value): Vector(0, n) {
        for (size_t i = 0; i < n; ++i) {
            new (arr + i) T(value);
        }
        sz = n;
    }

    Vector(const Vector& other): Vector(0, other.cap) {
        try {
            safe_copy(arr, other.arr, other.sz);
            sz = other.sz;
        } catch (...) {
            destroy_range(arr, arr + sz);
            operator delete[](arr);
            throw;
        }
    }

    Vector(std::initializer_list<T> list): Vector(0, list.size()) {
        try {
            const T* src = list.begin();
            for (size_t i = 0; i < list.size(); ++i) {
                new (arr + i) T(src[i]);
            }
            sz = list.size();
        } catch (...) {
            destroy_range(arr, arr + sz);
            operator delete[](arr);
            throw;
        }
    }

    Vector(Vector&& other) noexcept : arr(other.arr), sz(other.sz), cap(other.cap) {
        other.arr = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    ~Vector() noexcept {
        destroy_range(arr, arr + sz);
        operator delete[](arr);
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
            destroy_range(arr, arr + sz);
            operator delete[](arr);
            
            arr = other.arr;
            sz = other.sz;
            cap = other.cap;
            
            other.arr = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    void swap(Vector& other) noexcept {
        std::swap(arr, other.arr);
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
        return arr[index]; 
    }

    const T& operator[](size_t index) const { 
        return arr[index]; 
    }

    T& at(size_t index) {
        check_index(index);
        return arr[index];
    }

    const T& at(size_t index) const {
        check_index(index);
        return arr[index];
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
        new (arr + sz) T(value);
        ++sz;
    }

    void push_back(T&& value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            reallocate(new_cap);
        }
        new (arr + sz) T(std::move(value));
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            --sz;
            arr[sz].~T();
        }        
    }

    void clear() noexcept {
        destroy_range(arr, arr + sz);
        sz = 0;
    }

    void resize(size_t new_sz, const T& value = T()) {
        if (new_sz > sz) {
            if (new_sz > cap) {
                size_t new_cap = std::max(cap * 2, new_sz);
                reallocate(new_cap);
            }
            
            for (size_t i = sz; i < new_sz; ++i) {
                new (arr + i) T(value);
            }
        } 
        else if (new_sz < sz) {
            destroy_range(arr + new_sz, arr + sz);
        }
        
        sz = new_sz;
    }

    void shrink_to_fit() {
        if (sz < cap) {
            if (sz == 0) {
                destroy_range(arr, arr + sz);
                operator delete[](arr);
                arr = nullptr;
                cap = 0;
            } 
            else {
                T* new_arr = static_cast<T*>(operator new[](sz * sizeof(T)));
                try {
                    for (size_t i = 0; i < sz; ++i) {
                        new (new_arr + i) T(std::move(arr[i]));
                        arr[i].~T();
                    }
                } catch (...) {
                    destroy_range(new_arr, new_arr + sz);
                    operator delete[](new_arr);
                    throw;
                }
                
                operator delete[](arr);
                arr = new_arr;
                cap = sz;
            }
        }
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("Vector::front - vector is empty!");
        }
        return arr[0];
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Vector::front - vector is empty!");
        }
        return arr[0];
    }

    T& back() {
        if (empty()) {
            throw std::out_of_range("Vector::back - vector is empty!");
        }
        return arr[sz - 1];
    }

    const T& back() const {
        if (empty()) {
            throw std::out_of_range("Vector::back - vector is empty!");
        }
        return arr[sz - 1];
    }

    T* data() noexcept { 
        return arr; 
    }

    const T* data() const noexcept { 
        return arr; 
    }

    T* begin() noexcept { 
        return arr; 
    }

    const T* begin() const noexcept { 
        return arr; 
    }

    T* end() noexcept { 
        return arr + sz; 
    }

    const T* end() const noexcept { 
        return arr + sz; 
    }
};

int main() {}