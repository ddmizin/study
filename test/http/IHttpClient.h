#pragma once

#include <string>
#include <map>
#include <expected>

using StringMap = std::map<std::string, std::string>;

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual std::expected<std::string, std::string> Get(const std::string& url, const StringMap& params) = 0;
};