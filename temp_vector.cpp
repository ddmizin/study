#include <iostream>
#include <stdexcept>
#include <initializer_list>

template<typename T>
struct Vector {
private:
    T* data;           // Указатель на массив элементов
    size_t capacity;   // Вместимость вектора
    size_t size;       // Текущее количество элементов

    // Увеличение вместимости вектора
    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        
        // Копируем существующие элементы
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        
        // Освобождаем старую память
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    // Конструкторы
    Vector() : data(nullptr), capacity(0), size(0) {}
    
    Vector(size_t initial_size) : capacity(initial_size), size(initial_size) {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = T(); // Инициализация значениями по умолчанию
        }
    }
    
    Vector(std::initializer_list<T> init_list) : 
        capacity(init_list.size()), size(init_list.size()) {
        data = new T[capacity];
        size_t i = 0;
        for (const auto& item : init_list) {
            data[i++] = item;
        }
    }
    
    // Конструктор копирования
    Vector(const Vector& other) : 
        capacity(other.capacity), size(other.size) {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    
    // Конструктор перемещения
    Vector(Vector&& other) noexcept : 
        data(other.data), capacity(other.capacity), size(other.size) {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    
    // Оператор присваивания копированием
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            capacity = other.capacity;
            size = other.size;
            other.data = nullptr;
            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }
    
    // Деструктор
    ~Vector() {
        delete[] data;
    }

    // Методы доступа к элементам
    T& operator[](size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    const T& operator[](size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    T& at(size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    const T& at(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    T& front() {
        if (size == 0) throw std::out_of_range("Vector is empty");
        return data[0];
    }
    
    T& back() {
        if (size == 0) throw std::out_of_range("Vector is empty");
        return data[size - 1];
    }
    
    T* get_data() { return data; }
    const T* get_data() const { return data; }

    // Информация о состоянии
    bool empty() const { return size == 0; }
    size_t get_size() const { return size; }
    size_t get_capacity() const { return capacity; }

    // Модификация вектора
    void push_back(const T& value) {
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = value;
    }
    
    void push_back(T&& value) {
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = std::move(value);
    }
    
    void pop_back() {
        if (size > 0) {
            --size;
        }
    }
    
    void clear() {
        size = 0;
    }
    
    void reserve(size_t new_capacity) {
        if (new_capacity > capacity) {
            resize(new_capacity);
        }
    }
    
    void shrink_to_fit() {
        if (size < capacity) {
            resize(size);
        }
    }

    // Итераторы (упрощенные)
    T* begin() { return data; }
    T* end() { return data + size; }
    const T* begin() const { return data; }
    const T* end() const { return data + size; }
    const T* cbegin() const { return data; }
    const T* cend() const { return data + size; }
};

// Вспомогательная функция для вывода вектора
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.get_size(); ++i) {
        os << vec[i];
        if (i < vec.get_size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}