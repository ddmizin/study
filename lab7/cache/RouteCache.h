#pragma once

#include <string>
#include <chrono>
#include <list>
#include <unordered_map>
#include <optional>

class RouteCache {
    struct CacheItem {
        std::string response_;
        std::chrono::system_clock::time_point timestamp_;
    };
    size_t max_size_;
    std::chrono::seconds ttl_;
    std::list<std::pair<std::string, CacheItem>> list_;
    std::unordered_map<std::string, decltype(list_)::iterator> map_;
    std::string cache_filepath_;

    void LoadFromFile();
    void SaveToFile();

public:
    RouteCache(std::string filepath, size_t max_size, std::chrono::seconds ttl);
    ~RouteCache();

    std::optional<std::string> Get(const std::string& key);
    void Put(const std::string& key, const std::string& value);
};