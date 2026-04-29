#include <gtest/gtest.h>
#include <filesystem>
#include <thread>
#include <fstream>
#include <chrono>
#include <nlohmann/json.hpp>
#include "cache/RouteCache.h"

class RouteCacheTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::filesystem::remove("lru_cache.json");
        std::filesystem::remove("ttl_cache.json");
        std::filesystem::remove("file_io_cache.json");
    }
};

TEST_F(RouteCacheTest, LruEviction) {
    RouteCache cache("lru_cache.json", 2, std::chrono::minutes(10));
    cache.Put("key1", "value1");
    cache.Put("key2", "value2");

    // key2 is now the least recently used
    ASSERT_TRUE(cache.Get("key1").has_value()); // Access key1 to make it most recent
    
    cache.Put("key3", "value3"); // This should evict key2

    EXPECT_FALSE(cache.Get("key2").has_value());
    EXPECT_TRUE(cache.Get("key1").has_value());
    EXPECT_TRUE(cache.Get("key3").has_value());
}

TEST_F(RouteCacheTest, TtlExpiration) {
    RouteCache cache("ttl_cache.json", 5, std::chrono::seconds(1));
    cache.Put("key1", "value1");

    ASSERT_TRUE(cache.Get("key1").has_value());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_FALSE(cache.Get("key1").has_value());
}

TEST_F(RouteCacheTest, FilePersistence) {
    const std::string cache_file = "file_io_cache.json";
    {
        RouteCache cache(cache_file, 5, std::chrono::minutes(10));
        cache.Put("persistent_key", "persistent_value");
    } // cache destructor is called here, saving to file

    {
        RouteCache cache(cache_file, 5, std::chrono::minutes(10));
        auto value = cache.Get("persistent_key");
        ASSERT_TRUE(value.has_value());
        EXPECT_EQ(value.value(), "persistent_value");
    }
}

TEST_F(RouteCacheTest, FileLoadExpired) {
    const std::string cache_file = "file_io_cache.json";
    
    nlohmann::json j;
    j["old_key"] = {{"response", "old_value"}, {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::hours(2))}};
    std::ofstream(cache_file) << j.dump(4);

    RouteCache cache(cache_file, 5, std::chrono::minutes(30));
    EXPECT_FALSE(cache.Get("old_key").has_value());
}