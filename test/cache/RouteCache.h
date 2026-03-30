#pragma once

#include <string>
#include <chrono>
#include <list>
#include <unordered_map>
#include <optional>

class RouteCache {
    struct CacheItem {
        std::string response;
        std::chrono::system_clock::time_point timestamp;
    };
    size_t max_size_;
    std::chrono::seconds ttl_;
    std::list<std::pair<std::string, CacheItem>> list_;
    std::unordered_map<std::string, decltype(list_)::iterator> map_;
    std::string cache_filepath_;

    void load_from_file();
    void save_to_file();

public:
    RouteCache(std::string filepath, size_t max_size, std::chrono::seconds ttl);
    ~RouteCache();

    std::optional<std::string> get(const std::string& key);
    void put(const std::string& key, const std::string& value);
};