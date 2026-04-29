#pragma once

#include <string>
#include <memory>
#include <expected>
#include "cache/RouteCache.h"
#include "http/IHttpClient.h"

class YandexRaspApi {
    std::string api_key_;
    RouteCache cache_;
    std::shared_ptr<IHttpClient> http_client_;

public:
    YandexRaspApi(std::string key, std::shared_ptr<IHttpClient> client, const std::string& cache_file = "yandex_rasp_cache.json");

    std::expected<std::string, std::string> GetRoutes(const std::string& from, const std::string& to, const std::string& date);
};