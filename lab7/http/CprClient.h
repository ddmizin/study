#pragma once

#include "http/IHttpClient.h"

class CprClient : public IHttpClient {
public:
    std::expected<std::string, std::string> Get(const std::string& url, const StringMap& params) override;
};