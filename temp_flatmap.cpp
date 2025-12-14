#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <initializer_list>
#include <utility>

template<typename Key, typename Value>
struct FlatMap {
private:
    std::pair<Key, Value>* data; // Массив пар ключ-значение
    size_t capacity;             // Вместимость
    size_t size;                 // Текущее количество элементов

    // Бинарный поиск позиции для вставки/поиска
    size_t find_position(const Key& key) const {
        if (size == 0) return 0;
        
        size_t left = 0;
        size_t right = size;
        
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            
            if (data[mid].first < key) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        return left;
    }

    // Увеличение вместимости
    void resize(size_t new_capacity) {
        std::pair<Key, Value>* new_data = new std::pair<Key, Value>[new_capacity];
        
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = std::move(data[i]);
        }
        
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    // Конструкторы
    FlatMap() : data(nullptr), capacity(0), size(0) {}
    
    FlatMap(std::initializer_list<std::pair<Key, Value>> init_list) : 
        capacity(init_list.size()), size(0) {
        data = new std::pair<Key, Value>[capacity];
        
        for (const auto& pair : init_list) {
            insert(pair.first, pair.second);
        }
    }
    
    // Конструктор копирования
    FlatMap(const FlatMap& other) : 
        capacity(other.capacity), size(other.size) {
        data = new std::pair<Key, Value>[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    
    // Конструктор перемещения
    FlatMap(FlatMap&& other) noexcept : 
        data(other.data), capacity(other.capacity), size(other.size) {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    
    // Оператор присваивания
    FlatMap& operator=(const FlatMap& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new std::pair<Key, Value>[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    FlatMap& operator=(FlatMap&& other) noexcept {
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
    ~FlatMap() {
        delete[] data;
    }

    // Доступ к элементам
    Value& operator[](const Key& key) {
        size_t pos = find_position(key);
        
        if (pos < size && data[pos].first == key) {
            return data[pos].second;
        }
        
        // Если ключ не найден, вставляем новый
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        
        // Сдвигаем элементы вправо
        for (size_t i = size; i > pos; --i) {
            data[i] = std::move(data[i - 1]);
        }
        
        data[pos].first = key;
        data[pos].second = Value();
        ++size;
        
        return data[pos].second;
    }
    
    Value& at(const Key& key) {
        size_t pos = find_position(key);
        
        if (pos < size && data[pos].first == key) {
            return data[pos].second;
        }
        
        throw std::out_of_range("Key not found");
    }
    
    const Value& at(const Key& key) const {
        size_t pos = find_position(key);
        
        if (pos < size && data[pos].first == key) {
            return data[pos].second;
        }
        
        throw std::out_of_range("Key not found");
    }

    // Вставка элементов
    bool insert(const Key& key, const Value& value) {
        size_t pos = find_position(key);
        
        // Если ключ уже существует
        if (pos < size && data[pos].first == key) {
            return false;
        }
        
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        
        // Сдвигаем элементы вправо
        for (size_t i = size; i > pos; --i) {
            data[i] = std::move(data[i - 1]);
        }
        
        data[pos] = std::make_pair(key, value);
        ++size;
        
        return true;
    }
    
    bool insert(Key&& key, Value&& value) {
        size_t pos = find_position(key);
        
        if (pos < size && data[pos].first == key) {
            return false;
        }
        
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        
        for (size_t i = size; i > pos; --i) {
            data[i] = std::move(data[i - 1]);
        }
        
        data[pos] = std::make_pair(std::move(key), std::move(value));
        ++size;
        
        return true;
    }
    
    template<typename... Args>
    bool emplace(Args&&... args) {
        // Создаем временную пару
        std::pair<Key, Value> temp(std::forward<Args>(args)...);
        return insert(std::move(temp.first), std::move(temp.second));
    }

    // Удаление элементов
    bool erase(const Key& key) {
        size_t pos = find_position(key);
        
        if (pos >= size || data[pos].first != key) {
            return false;
        }
        
        // Сдвигаем элементы влево
        for (size_t i = pos; i < size - 1; ++i) {
            data[i] = std::move(data[i + 1]);
        }
        
        --size;
        return true;
    }
    
    void clear() {
        size = 0;
    }

    // Поиск
    bool contains(const Key& key) const {
        size_t pos = find_position(key);
        return pos < size && data[pos].first == key;
    }
    
    size_t count(const Key& key) const {
        return contains(key) ? 1 : 0;
    }
    
    // Итераторы
    std::pair<Key, Value>* begin() { return data; }
    std::pair<Key, Value>* end() { return data + size; }
    const std::pair<Key, Value>* begin() const { return data; }
    const std::pair<Key, Value>* end() const { return data + size; }
    const std::pair<Key, Value>* cbegin() const { return data; }
    const std::pair<Key, Value>* cend() const { return data + size; }

    // Информация
    bool empty() const { return size == 0; }
    size_t get_size() const { return size; }
    size_t get_capacity() const { return capacity; }

    // Поиск с возвратом итератора
    std::pair<Key, Value>* find(const Key& key) {
        size_t pos = find_position(key);
        if (pos < size && data[pos].first == key) {
            return &data[pos];
        }
        return end();
    }
    
    const std::pair<Key, Value>* find(const Key& key) const {
        size_t pos = find_position(key);
        if (pos < size && data[pos].first == key) {
            return &data[pos];
        }
        return end();
    }

    // Нижняя и верхняя границы (как в std::map)
    std::pair<Key, Value>* lower_bound(const Key& key) {
        size_t pos = find_position(key);
        return &data[pos];
    }
    
    const std::pair<Key, Value>* lower_bound(const Key& key) const {
        size_t pos = find_position(key);
        return &data[pos];
    }
    
    std::pair<Key, Value>* upper_bound(const Key& key) {
        size_t pos = find_position(key);
        if (pos < size && data[pos].first == key) {
            return &data[pos + 1];
        }
        return &data[pos];
    }
    
    const std::pair<Key, Value>* upper_bound(const Key& key) const {
        size_t pos = find_position(key);
        if (pos < size && data[pos].first == key) {
            return &data[pos + 1];
        }
        return &data[pos];
    }
};

// Вспомогательная функция для вывода
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const FlatMap<Key, Value>& map) {
    os << "{";
    for (size_t i = 0; i < map.get_size(); ++i) {
        auto it = map.begin() + i;
        os << it->first << ": " << it->second;
        if (i < map.get_size() - 1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}