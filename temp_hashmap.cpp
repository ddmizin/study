#include <iostream>
#include <functional>
#include <stdexcept>
#include <initializer_list>
#include <utility>

template<typename Key, typename Value, typename Hash = std::hash<Key>>
struct HashMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        
        Node(const Key& k, const Value& v, Node* n = nullptr) 
            : key(k), value(v), next(n) {}
        
        Node(Key&& k, Value&& v, Node* n = nullptr) 
            : key(std::move(k)), value(std::move(v)), next(n) {}
    };

    struct Bucket {
        Node* head;
        
        Bucket() : head(nullptr) {}
        
        ~Bucket() {
            clear();
        }
        
        void clear() {
            Node* current = head;
            while (current) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            head = nullptr;
        }
        
        // Добавление в цепочку
        bool insert(const Key& key, const Value& value) {
            // Проверяем, нет ли уже такого ключа
            Node* current = head;
            while (current) {
                if (current->key == key) {
                    return false; // Ключ уже существует
                }
                current = current->next;
            }
            
            // Добавляем новый узел в начало цепочки
            head = new Node(key, value, head);
            return true;
        }
        
        bool insert(Key&& key, Value&& value) {
            Node* current = head;
            while (current) {
                if (current->key == key) {
                    return false;
                }
                current = current->next;
            }
            
            head = new Node(std::move(key), std::move(value), head);
            return true;
        }
        
        // Поиск в цепочке
        Node* find(const Key& key) {
            Node* current = head;
            while (current) {
                if (current->key == key) {
                    return current;
                }
                current = current->next;
            }
            return nullptr;
        }
        
        const Node* find(const Key& key) const {
            Node* current = head;
            while (current) {
                if (current->key == key) {
                    return current;
                }
                current = current->next;
            }
            return nullptr;
        }
        
        // Удаление из цепочки
        bool erase(const Key& key) {
            if (!head) return false;
            
            // Если удаляем голову
            if (head->key == key) {
                Node* to_delete = head;
                head = head->next;
                delete to_delete;
                return true;
            }
            
            // Ищем в середине списка
            Node* current = head;
            while (current->next) {
                if (current->next->key == key) {
                    Node* to_delete = current->next;
                    current->next = current->next->next;
                    delete to_delete;
                    return true;
                }
                current = current->next;
            }
            
            return false;
        }
        
        // Проверка наличия ключа
        bool contains(const Key& key) const {
            return find(key) != nullptr;
        }
    };

    Bucket* buckets;       // Массив бакетов
    size_t capacity;       // Вместимость (количество бакетов)
    size_t size;           // Количество элементов
    Hash hasher;           // Функция хэширования
    float max_load_factor; // Максимальный коэффициент загрузки

    // Вычисление индекса бакета
    size_t bucket_index(const Key& key) const {
        return hasher(key) % capacity;
    }

    // Рехэширование таблицы
    void rehash(size_t new_capacity) {
        if (new_capacity == 0) new_capacity = 1;
        
        Bucket* new_buckets = new Bucket[new_capacity];
        
        // Перемещаем все элементы в новую таблицу
        for (size_t i = 0; i < capacity; ++i) {
            Node* current = buckets[i].head;
            while (current) {
                Node* next = current->next;
                size_t new_index = hasher(current->key) % new_capacity;
                
                // Вставляем в новый бакет
                current->next = new_buckets[new_index].head;
                new_buckets[new_index].head = current;
                
                current = next;
            }
            buckets[i].head = nullptr; // Старый бакет теперь пуст
        }
        
        delete[] buckets;
        buckets = new_buckets;
        capacity = new_capacity;
    }

    // Проверка необходимости рехэширования
    void check_rehash() {
        if (load_factor() > max_load_factor) {
            rehash(capacity * 2);
        }
    }

public:
    // Конструкторы
    HashMap(size_t initial_capacity = 8) 
        : capacity(initial_capacity), size(0), max_load_factor(0.75f) {
        if (capacity == 0) capacity = 1;
        buckets = new Bucket[capacity];
    }
    
    HashMap(std::initializer_list<std::pair<Key, Value>> init_list) 
        : capacity(8), size(0), max_load_factor(0.75f) {
        buckets = new Bucket[capacity];
        for (const auto& pair : init_list) {
            insert(pair.first, pair.second);
        }
    }
    
    // Конструктор копирования
    HashMap(const HashMap& other) 
        : capacity(other.capacity), size(other.size), 
          hasher(other.hasher), max_load_factor(other.max_load_factor) {
        buckets = new Bucket[capacity];
        
        for (size_t i = 0; i < capacity; ++i) {
            const Node* current = other.buckets[i].head;
            while (current) {
                size_t index = bucket_index(current->key);
                buckets[index].insert(current->key, current->value);
                current = current->next;
            }
        }
    }
    
    // Конструктор перемещения
    HashMap(HashMap&& other) noexcept 
        : buckets(other.buckets), capacity(other.capacity), size(other.size),
          hasher(std::move(other.hasher)), max_load_factor(other.max_load_factor) {
        other.buckets = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    
    // Оператор присваивания
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            delete[] buckets;
            
            capacity = other.capacity;
            size = other.size;
            hasher = other.hasher;
            max_load_factor = other.max_load_factor;
            
            buckets = new Bucket[capacity];
            for (size_t i = 0; i < capacity; ++i) {
                const Node* current = other.buckets[i].head;
                while (current) {
                    size_t index = bucket_index(current->key);
                    buckets[index].insert(current->key, current->value);
                    current = current->next;
                }
            }
        }
        return *this;
    }
    
    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            delete[] buckets;
            
            buckets = other.buckets;
            capacity = other.capacity;
            size = other.size;
            hasher = std::move(other.hasher);
            max_load_factor = other.max_load_factor;
            
            other.buckets = nullptr;
            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }
    
    // Деструктор
    ~HashMap() {
        delete[] buckets;
    }

    // Вставка элементов
    bool insert(const Key& key, const Value& value) {
        check_rehash();
        
        size_t index = bucket_index(key);
        if (buckets[index].insert(key, value)) {
            ++size;
            return true;
        }
        return false;
    }
    
    bool insert(Key&& key, Value&& value) {
        check_rehash();
        
        size_t index = bucket_index(key);
        if (buckets[index].insert(std::move(key), std::move(value))) {
            ++size;
            return true;
        }
        return false;
    }
    
    template<typename K, typename V>
    bool emplace(K&& key, V&& value) {
        return insert(std::forward<K>(key), std::forward<V>(value));
    }

    // Доступ к элементам
    Value& operator[](const Key& key) {
        size_t index = bucket_index(key);
        Node* node = buckets[index].find(key);
        
        if (!node) {
            // Создаем новый элемент
            insert(key, Value());
            node = buckets[index].find(key);
        }
        
        return node->value;
    }
    
    Value& at(const Key& key) {
        size_t index = bucket_index(key);
        Node* node = buckets[index].find(key);
        
        if (!node) {
            throw std::out_of_range("Key not found");
        }
        
        return node->value;
    }
    
    const Value& at(const Key& key) const {
        size_t index = bucket_index(key);
        const Node* node = buckets[index].find(key);
        
        if (!node) {
            throw std::out_of_range("Key not found");
        }
        
        return node->value;
    }

    // Удаление элементов
    bool erase(const Key& key) {
        size_t index = bucket_index(key);
        if (buckets[index].erase(key)) {
            --size;
            return true;
        }
        return false;
    }
    
    void clear() {
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i].clear();
        }
        size = 0;
    }

    // Поиск
    bool contains(const Key& key) const {
        size_t index = bucket_index(key);
        return buckets[index].contains(key);
    }
    
    // Итератор (упрощенный)
    class Iterator {
    private:
        HashMap* map;
        size_t bucket_index;
        Node* current_node;
        
        void find_next_non_empty() {
            while (bucket_index < map->capacity && !current_node) {
                ++bucket_index;
                if (bucket_index < map->capacity) {
                    current_node = map->buckets[bucket_index].head;
                }
            }
        }
        
    public:
        Iterator(HashMap* m, size_t index, Node* node) 
            : map(m), bucket_index(index), current_node(node) {
            if (current_node == nullptr && map) {
                find_next_non_empty();
            }
        }
        
        std::pair<Key, Value>& operator*() {
            return *reinterpret_cast<std::pair<Key, Value>*>(&current_node->key);
        }
        
        std::pair<Key, Value>* operator->() {
            return reinterpret_cast<std::pair<Key, Value>*>(current_node);
        }
        
        Iterator& operator++() {
            if (current_node) {
                current_node = current_node->next;
                if (!current_node) {
                    ++bucket_index;
                    find_next_non_empty();
                }
            }
            return *this;
        }
        
        bool operator==(const Iterator& other) const {
            return current_node == other.current_node;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    
    Iterator begin() {
        return Iterator(this, 0, capacity > 0 ? buckets[0].head : nullptr);
    }
    
    Iterator end() {
        return Iterator(this, capacity, nullptr);
    }

    // Информация
    bool empty() const { return size == 0; }
    size_t get_size() const { return size; }
    size_t get_capacity() const { return capacity; }
    size_t bucket_count() const { return capacity; }
    
    float load_factor() const {
        return capacity > 0 ? static_cast<float>(size) / capacity : 0.0f;
    }
    
    float get_max_load_factor() const { return max_load_factor; }
    void set_max_load_factor(float factor) { max_load_factor = factor; }
    
    // Резервирование
    void reserve(size_t new_capacity) {
        if (new_capacity > capacity) {
            rehash(new_capacity);
        }
    }
    
    void rehash_to_load_factor() {
        size_t new_capacity = static_cast<size_t>(size / max_load_factor) + 1;
        rehash(new_capacity);
    }
};

// Вспомогательная функция для вывода
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const HashMap<Key, Value>& map) {
    os << "{";
    bool first = true;
    
    // Простой вывод через итерацию по всем бакетам
    for (size_t i = 0; i < map.bucket_count(); ++i) {
        // Здесь нужно было бы использовать итераторы для красивого вывода
        // Для простоты выведем только размер
    }
    
    os << "size: " << map.get_size() << "}";
    return os;
}