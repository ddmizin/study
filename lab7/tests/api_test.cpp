#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <filesystem>
#include <string>
#include "MockHttpClient.h"
#include "api/YandexRaspAPI.h"

using ::testing::_;
using ::testing::Return;

TEST(YandexRaspApiTest, CachingLogic) {
    const std::string test_cache_file = "test_cache.json";
    std::filesystem::remove(test_cache_file); // Очищаем кэш от прошлых запусков

    auto mock_http_client = std::make_shared<MockHttpClient>();
    YandexRaspApi api("test_api_key", mock_http_client, test_cache_file);

    const std::string from = "c2";
    const std::string to = "c25";
    const std::string date = "2026-01-01";
    const std::string fake_response = R"({"segments": []})";

    EXPECT_CALL(*mock_http_client, Get(_, _))
        .Times(1)
        .WillOnce(Return(fake_response));

    auto result1 = api.GetRoutes(from, to, date);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), fake_response);

    auto result2 = api.GetRoutes(from, to, date);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), fake_response);

    std::filesystem::remove(test_cache_file); // Убираем за собой
}