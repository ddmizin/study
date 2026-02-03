#include <iostream>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <initializer_list>

template <typename T>
class Vector {
    T* arr;
    size_t sz;
    size_t cap;

    Vector(size_t sz, size_t cap): arr(new T[cap]), sz(sz), cap(cap) {}

    void check_index(size_t index) {
        if (index >= sz) {
            throw std::out_of_range("Vector::at - index out of range!");
        }
    }

    void reallocate(size_t new_cap) {
        if (new_cap <= cap) {
            return;
        }

        T* new_arr = new T[new_cap];

        if (sz > 0) {
            std::memcpy(new_arr, arr, sz * sizeof(T));
        }
    
        delete[] arr;
        arr = new_arr;
        cap = new_cap; 
    }

public:
    Vector(): arr(nullptr), sz(0), cap(0) {}

    Vector(size_t n): Vector(n, n) {
        std::memset(arr, T(), n * sizeof(T));
    }

    Vector(size_t n, const T& value): Vector(n, n) {
        std::memset(arr, value, n * sizeof(T));
    }

    Vector(std::initializer_list<T> list): Vector(list.size(), list.size()) {
        std::copy(list.begin(), list.end(), arr);
    }

    Vector(const Vector& other): Vector(other.sz, other.cap) {
        std::memcpy(arr, other.arr, other.sz * sizeof(T));
    }

    Vector(Vector&& other) noexcept : arr(other.arr), sz(other.sz), cap(other.cap) {
        other.arr = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    ~Vector() noexcept {
        delete[] arr;
    }

    Vector& operator=(const Vector other) {
        swap(other);
        return *this;
    }

    void swap(Vector other) noexcept {
        std::swap(arr, other.arr);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    friend void swap(Vector& v1, Vector& v2) {
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
        reallocate(new_cap);
    }

    void push_back(const T& value) {
        if (sz == cap) {
            size_t new_cap = (cap == 0) ? 1 : cap * 2;
            reallocate(new_cap);
        }
        arr[sz] = value;
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            --sz;
        }
    }

    void clear() noexcept {
        sz = 0;
    }

    void resize(size_t new_sz, const T& value = T()) {
        if (new_sz > sz) {
            size_t new_cap = std::max(cap * 2, new_sz);
            reallocate(new_cap);
        }
        
        if (new_sz > sz) {
            for (size_t i = sz; i < new_sz; ++i) {
                arr[i] = value;
            }
        }

        sz = new_sz;
    }

    void shrink_to_fit() {
        if (sz < cap) {
            if (sz == 0) {
                delete[] arr;
                arr = nullptr;
                cap = 0;
            }
            else {
                T* new_arr = new T[sz];
                if (sz > 0) {
                    std::memcpy(new_arr, arr, sz * sizeof(T));
                }
                delete[] arr;
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
            throw std::out_of_range("Vector::back - vector is empty!")
        }
        return arr[sz - 1];
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