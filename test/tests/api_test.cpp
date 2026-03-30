#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "MockHttpClient.h"
#include "api/YandexRaspAPI.h"

using ::testing::_;
using ::testing::Return;

// Тест для проверки логики кэширования в YandexRaspAPI
TEST(YandexRaspAPITest, CachingLogic) {
    // 1. Подготовка
    auto mock_http_client = std::make_shared<MockHttpClient>();
    YandexRaspAPI api("test_api_key", mock_http_client);

    const std::string from = "c2";
    const std::string to = "c25";
    const std::string date = "2026-01-01";
    const std::string fake_response = R"({"segments": []})";

    // 2. Настройка мока: ожидаем, что метод Get будет вызван РОВНО 1 раз.
    // При вызове он должен вернуть наш тестовый JSON.
    EXPECT_CALL(*mock_http_client, Get(_, _))
        .Times(1)
        .WillOnce(Return(fake_response));

    // 3. Первый вызов: кэш пуст, должен произойти вызов API (нашего мока)
    auto result1 = api.get_routes(from, to, date);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), fake_response);

    // 4. Второй вызов с теми же параметрами: данные должны взяться из кэша.
    // Метод Get у мока вызван НЕ будет, т.к. мы настроили .Times(1).
    // Если бы произошел второй вызов, gmock провалил бы тест.
    auto result2 = api.get_routes(from, to, date);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), fake_response);
}