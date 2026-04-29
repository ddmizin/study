#include "api/YandexRaspAPI.h"
#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <utility>
#include <chrono>
#include <expected>
#include "http/IHttpClient.h"

YandexRaspApi::YandexRaspApi(std::string key, std::shared_ptr<IHttpClient> client, const std::string& cache_file) 
    : api_key_(std::move(key)), cache_(cache_file, 50, std::chrono::minutes(30)), http_client_(std::move(client)) {}

std::expected<std::string, std::string> YandexRaspApi::GetRoutes(const std::string& from, const std::string& to, const std::string& date) {
    std::string url = "https://api.rasp.yandex.net/v3.0/search/";
    std::map<std::string, std::string> params{
        {"apikey", api_key_},
        {"from", from},
        {"to", to},
        {"date", date},
        {"transfers", "true"},
        {"limit", "1000"},
        {"format", "json"}
    };

    std::string cache_key = from + "_" + to + "_" + date;
    
    auto cached = cache_.Get(cache_key);
    if (cached) {
        std::cout << "[INFO] Успешно загружено из кэша (без запроса к API).\n";
        return *cached;
    }

    auto result = http_client_->Get(url, params);
    if (result) {
        cache_.Put(cache_key, *result);
        return result;
    } else {
        return std::unexpected(result.error());
    }
}