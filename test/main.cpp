#include <iostream>
#include <string>
#include <memory>
#include "api/YandexRaspAPI.h"
#include "utils/RoutePrinter.h"
#include "http/CprClient.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <YYYY-MM-DD>\n";
        std::cerr << "Пример: " << argv[0] << " 2024-05-15\n";
        return 1;
    }
    std::string date = argv[1];
    std::string api_key = "749f6954-d4ae-4c6e-a3e8-42b1e5e7e84c";
    
    auto http_client = std::make_shared<CprClient>();
    YandexRaspAPI api(api_key, http_client);

    // Коды городов в системе Яндекс: c2 = Санкт-Петербург, c25 = Псков
    std::string spb_code = "c2";
    std::string pskov_code = "c25";

    while (true) {
        std::cout << "\n=== Поиск маршрутов (Дата: " << date << ") ===\n";
        std::cout << "1. Санкт-Петербург -> Псков\n";
        std::cout << "2. Псков -> Санкт-Петербург\n";
        std::cout << "0. Выход\n";
        std::cout << "Ваш выбор: ";
        
        std::string input;
        if (!(std::cin >> input)) break; // Обработка EOF

        if (input == "0" || input == "q" || input == "Q") {
            std::cout << "Завершение программы...\n";
            break;
        }

        std::string from, to;
        if (input == "1") {
            from = spb_code;
            to = pskov_code;
        } else if (input == "2") {
            from = pskov_code;
            to = spb_code;
        } else {
            std::cout << "Неверный выбор. Введите 1, 2 или 0.\n";
            continue;
        }

        // Возвращается std::expected, проверяем на наличие значения
        auto result = api.get_routes(from, to, date);
        if (result.has_value()) {
            print_routes(result.value());
        } else {
            std::cerr << "Сетевая ошибка: " << result.error() << "\n";
        }
    }

    return 0;
}