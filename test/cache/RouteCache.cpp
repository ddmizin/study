#include "cache/RouteCache.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

RouteCache::RouteCache(std::string filepath, size_t max_size, std::chrono::seconds ttl) 
    : cache_filepath_(std::move(filepath)), max_size_(max_size), ttl_(ttl) {
    load_from_file();
}

RouteCache::~RouteCache() {
    save_to_file();
}

void RouteCache::load_from_file() {
    std::ifstream file(cache_filepath_);
    if (!file.is_open()) {
        // Файл не существует или не может быть открыт, это нормально при первом запуске.
        return;
    }

    try {
        json j;
        file >> j;

        for (auto& [key, val] : j.items()) {
            if (val.is_object() && val.contains("response") && val.contains("timestamp")) {
                auto timestamp_from_file = std::chrono::system_clock::from_time_t(val["timestamp"].get<std::time_t>());
                
                // Проверяем, не истек ли срок жизни записи
                if (std::chrono::system_clock::now() - timestamp_from_file > ttl_) {
                    continue; // Пропускаем просроченные записи
                }

                // Добавляем в кэш, если есть место и такого ключа еще нет
                if (map_.find(key) == map_.end() && list_.size() < max_size_) {
                    list_.emplace_front(key, CacheItem{val["response"].get<std::string>(), timestamp_from_file});
                    map_[key] = list_.begin();
                }
            }
        }
    } catch (const json::parse_error& e) {
        std::cerr << "Предупреждение: не удалось прочитать или распарсить файл кэша '" << cache_filepath_ << "'. Ошибка: " << e.what() << ". Будет создан новый кэш.\n";
    }
}

void RouteCache::save_to_file() {
    json j;
    // Итерируемся в обратном порядке, чтобы сохранить более старые записи первыми (хотя для JSON-объекта порядок не важен)
    for (auto it = list_.rbegin(); it != list_.rend(); ++it) {
        json val;
        val["response"] = it->second.response;
        val["timestamp"] = std::chrono::system_clock::to_time_t(it->second.timestamp);
        j[it->first] = val;
    }

    std::ofstream file(cache_filepath_);
    if (file.is_open()) {
        file << j.dump(4); // Записываем с красивым форматированием
    } else {
        std::cerr << "Ошибка: не удалось сохранить кэш в файл '" << cache_filepath_ << "'.\n";
    }
}

std::optional<std::string> RouteCache::get(const std::string& key) {
    auto it = map_.find(key);
    if (it == map_.end()) return std::nullopt;

    if (std::chrono::system_clock::now() - it->second->second.timestamp > ttl_) {
        list_.erase(it->second);
        map_.erase(it);
        return std::nullopt;
    }

    list_.splice(list_.begin(), list_, it->second);
    return it->second->second.response;
}

void RouteCache::put(const std::string& key, const std::string& value) {
    auto it = map_.find(key);
    if (it != map_.end()) {
        list_.splice(list_.begin(), list_, it->second);
        it->second->second = {value, std::chrono::system_clock::now()};
        return;
    }

    if (list_.size() >= max_size_) {
        map_.erase(list_.back().first);
        list_.pop_back();
    }

    list_.emplace_front(key, CacheItem{value, std::chrono::system_clock::now()});
    map_[key] = list_.begin();
}