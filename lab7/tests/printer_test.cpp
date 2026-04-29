#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <string>
#include "utils/RoutePrinter.h"

class CoutRedirect {
public:
    explicit CoutRedirect(std::streambuf* new_buffer)
        : old_cout_(std::cout.rdbuf(new_buffer)) {}

    ~CoutRedirect() {
        std::cout.rdbuf(old_cout_);
    }

private:
    std::streambuf* old_cout_;
};

TEST(RoutePrinterTest, PrintsDirectRoute) {
    const std::string json_data = R"json({
        "segments": [
            {
                "thread": { "title": "Псков — Санкт-Петербург", "transport_type": "train" },
                "from": { "title": "Псков-Пасс." },
                "to": { "title": "Санкт-Петербург (Балтийский вокзал)" },
                "departure": "2026-03-20T06:05:00+03:00",
                "arrival": "2026-03-20T09:39:00+03:00",
                "has_transfers": false
            }
        ]
    })json";

    std::stringstream buffer;
    CoutRedirect redirect(buffer.rdbuf());

    PrintRoutes(json_data);

    std::string expected_output = "[1] 2026-03-20 06:05 - 2026-03-20 09:39 | Тип: поезд | Псков-Пасс. — Санкт-Петербург (Балтийский вокзал) (Рейс: Псков — Санкт-Петербург) | Прямой рейс\n";
    ASSERT_EQ(buffer.str(), expected_output);
}

TEST(RoutePrinterTest, PrintsRouteWithTransfer) {
    const std::string json_data = R"json({
        "segments": [
            {
                "departure": "2026-03-20T17:34:00+03:00",
                "arrival": "2026-03-20T23:36:00+03:00",
                "has_transfers": true,
                "details": [
                    {
                        "thread": { "title": "Псков-Пасс. — Луга-1", "transport_type": "suburban" },
                        "from": { "title": "Псков" }, "to": { "title": "Луга" },
                        "departure": "2026-03-20T17:34:00+03:00", "arrival": "2026-03-20T20:24:00+03:00"
                    },
                    { "is_transfer": true },
                    {
                        "thread": { "title": "Луга-1 — Санкт-Петербург (Балтийский вокзал)", "transport_type": "suburban" },
                        "from": { "title": "Луга" }, "to": { "title": "Санкт-Петербург" },
                        "departure": "2026-03-20T21:02:00+03:00", "arrival": "2026-03-20T23:36:00+03:00"
                    }
                ]
            }
        ]
    })json";

    std::stringstream buffer;
    CoutRedirect redirect(buffer.rdbuf());

    PrintRoutes(json_data);

    std::string expected_output = "[1] 2026-03-20 17:34 - 2026-03-20 23:36 | Маршрут с пересадкой (1)\n      - [Участок 1] 2026-03-20 17:34 - 2026-03-20 20:24 | Тип: электричка | Псков — Луга (Рейс: Псков-Пасс. — Луга-1)\n      - [Участок 2] 2026-03-20 21:02 - 2026-03-20 23:36 | Тип: электричка | Луга — Санкт-Петербург (Рейс: Луга-1 — Санкт-Петербург (Балтийский вокзал))\n";
    ASSERT_EQ(buffer.str(), expected_output);
}

TEST(RoutePrinterTest, HandlesEmptySegments) {
    const std::string json_data = R"json({"segments": []})json";
    std::stringstream buffer;
    CoutRedirect redirect(buffer.rdbuf());
    PrintRoutes(json_data);
    ASSERT_EQ(buffer.str(), "Маршруты не найдены.\n");
}

TEST(RoutePrinterTest, HandlesApiError) {
    const std::string json_data = R"json({"error": "Invalid API key"})json";
    std::stringstream cerr_buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buffer.rdbuf());
    PrintRoutes(json_data);
    std::cerr.rdbuf(old_cerr);
    ASSERT_EQ(cerr_buffer.str(), "Ошибка со стороны Yandex API: Invalid API key\n");
}