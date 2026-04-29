#include "cache/RouteCache.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstddef>
#include <chrono>
#include <utility>
#include <ctime>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

RouteCache::RouteCache(std::string filepath, size_t max_size, std::chrono::seconds ttl) 
    : cache_filepath_(std::move(filepath)), max_size_(max_size), ttl_(ttl) {
    LoadFromFile();
}

RouteCache::~RouteCache() {
    SaveToFile();
}

void RouteCache::LoadFromFile() {
    std::ifstream file(cache_filepath_);
    if (!file.is_open()) {
        return;
    }

    try {
        json j;
        file >> j;

        for (auto& [key, val] : j.items()) {
            if (val.is_object() && val.contains("response") && val.contains("timestamp")) {
                auto timestamp_from_file = std::chrono::system_clock::from_time_t(val["timestamp"].get<std::time_t>());
                
                if (std::chrono::system_clock::now() - timestamp_from_file > ttl_) {
                    continue;
                }

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

void RouteCache::SaveToFile() {
    json j;
    for (auto it = list_.rbegin(); it != list_.rend(); ++it) {
        json val;
        val["response"] = it->second.response_;
        val["timestamp"] = std::chrono::system_clock::to_time_t(it->second.timestamp_);
        j[it->first] = val;
    }

    std::ofstream file(cache_filepath_);
    if (file.is_open()) {
        file << j.dump(4);
    } else {
        std::cerr << "Ошибка: не удалось сохранить кэш в файл '" << cache_filepath_ << "'.\n";
    }
}

std::optional<std::string> RouteCache::Get(const std::string& key) {
    auto it = map_.find(key);
    if (it == map_.end()) return std::nullopt;

    if (std::chrono::system_clock::now() - it->second->second.timestamp_ > ttl_) {
        list_.erase(it->second);
        map_.erase(it);
        return std::nullopt;
    }

    list_.splice(list_.begin(), list_, it->second);
    return it->second->second.response_;
}

void RouteCache::Put(const std::string& key, const std::string& value) {
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