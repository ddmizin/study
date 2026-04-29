#pragma once

#include <gmock/gmock.h>
#include "http/IHttpClient.h"

class MockHttpClient : public IHttpClient {
public:
    MOCK_METHOD((std::expected<std::string, std::string>), Get, (const std::string& url, const StringMap& params), (override));
};