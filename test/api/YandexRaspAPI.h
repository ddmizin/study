#pragma once

#include <string>
#include <memory>
#include <expected>
#include "cache/RouteCache.h"
#include "http/IHttpClient.h"

class YandexRaspAPI {
    std::string api_key_;
    RouteCache cache_;
    std::shared_ptr<IHttpClient> http_client_;

public:
    YandexRaspAPI(std::string key, std::shared_ptr<IHttpClient> client);

    std::expected<std::string, std::string> get_routes(const std::string& from, const std::string& to, const std::string& date);
};