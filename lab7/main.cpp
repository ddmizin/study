#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "api/YandexRaspAPI.h"
#include "utils/RoutePrinter.h"
#include "http/CprClient.h"
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

class CityCodeResolver {
    std::unordered_map<std::string, std::string> city_codes_;
    std::string api_key_;
    std::string cache_file_ = "yandex_stations_list.json";

public:
    CityCodeResolver(const std::string& api_key) : api_key_(api_key) {
        LoadOrFetch();
    }

    std::string GetCode(const std::string& city_name) const {
        auto it = city_codes_.find(city_name);
        if (it != city_codes_.end()) return it->second;
        return "";
    }

private:
    void ExtractCodes(const nlohmann::json& root) {
        std::vector<const nlohmann::json*> stack;
        stack.push_back(&root);

        while (!stack.empty()) {
            const nlohmann::json* current = stack.back();
            stack.pop_back();

            if (current->is_object()) {
                std::string title;
                std::string code;
                
                if (current->contains("title") && (*current)["title"].is_string()) {
                    title = (*current)["title"].get<std::string>();
                }
                if (current->contains("codes") && (*current)["codes"].is_object()) {
                    if ((*current)["codes"].contains("yandex_code") && (*current)["codes"]["yandex_code"].is_string()) {
                        code = (*current)["codes"]["yandex_code"].get<std::string>();
                    } else if ((*current)["codes"].contains("yandex") && (*current)["codes"]["yandex"].is_string()) {
                        code = (*current)["codes"]["yandex"].get<std::string>();
                    }
                }

                if (!title.empty() && !code.empty()) {
                    if (code[0] == 'c') {
                        city_codes_[title] = code;
                    } else if (city_codes_.find(title) == city_codes_.end()) {
                        city_codes_[title] = code; 
                    }
                }

                for (auto it = current->begin(); it != current->end(); ++it) {
                    stack.push_back(&it.value());
                }
            } else if (current->is_array()) {
                for (auto it = current->begin(); it != current->end(); ++it) {
                    stack.push_back(&it.value());
                }
            }
        }
    }

    void LoadOrFetch() {
        std::string raw_json;
        std::ifstream file(cache_file_);
        if (file.is_open()) {
            std::cout << "Чтение базы городов с диска (около 30 МБ)... Пожалуйста, подождите.\n";
            raw_json.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        } else {
            std::cout << "Первый запуск: скачивание базы городов Яндекса (около 30 МБ). Пожалуйста, подождите...\n";
            std::string url = "https://api.rasp.yandex.net/v3.0/stations_list/?apikey=" + api_key_ + "&lang=ru_RU&format=json";
            
            auto response = cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/120.0.0.0"}});
            if (response.status_code != 200) {
                std::cerr << "Ошибка при скачивании городов. Код ответа: " << response.status_code << "\n";
                if (!response.text.empty()) std::cerr << "Текст ошибки: " << response.text.substr(0, 300) << "\n";
                return;
            }
            raw_json = response.text;
            
            std::ofstream out(cache_file_);
            out << raw_json;
            std::cout << "База успешно скачана и сохранена в файл '" << cache_file_ << "'.\n";
        }

        try {
            std::cout << "Парсинг JSON и построение индекса в памяти...\n";
            auto j = nlohmann::json::parse(raw_json);

            ExtractCodes(j);

            std::cout << "Готово! Загружено " << city_codes_.size() << " локаций.\n";
            if (city_codes_.empty()) {
                std::cerr << "\n[ВНИМАНИЕ] Не найдено ни одного города. Возможно, файл содержит ошибку от API (лимит запросов).\n";
                std::cerr << "Содержимое файла 'yandex_stations_list.json' (первые 300 символов):\n" << raw_json.substr(0, 300) << "\n";
                std::cerr << "УДАЛИТЕ этот файл перед следующим запуском, чтобы скачать его заново!\n\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при разборе списка городов: " << e.what() << "\n";
        }
    }
};

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <YYYY-MM-DD>\n";
        std::cerr << "Пример: " << argv[0] << " 2024-05-15\n";
        return 1;
    }
    std::string date = argv[1];
    std::string api_key = "749f6954-d4ae-4c6e-a3e8-42b1e5e7e84c";
    
    auto http_client = std::make_shared<CprClient>();
    YandexRaspApi api(api_key, http_client);
    CityCodeResolver resolver(api_key);

    while (true) {
        std::cout << "\n=== Поиск маршрутов (Дата: " << date << ") ===\n";
        std::cout << "Введите город отправления (с большой буквы, или 0 для выхода): ";
        
        std::string from_city;
        std::getline(std::cin >> std::ws, from_city);

        if (from_city == "0" || from_city == "q" || from_city == "Q") {
            std::cout << "Завершение программы...\n";
            break;
        }

        std::cout << "Введите город назначения (с большой буквы): ";
        std::string to_city;
        std::getline(std::cin, to_city);

        while (!from_city.empty() && std::isspace(static_cast<unsigned char>(from_city.back()))) from_city.pop_back();
        while (!to_city.empty() && std::isspace(static_cast<unsigned char>(to_city.back()))) to_city.pop_back();

        std::string from = resolver.GetCode(from_city);
        std::string to = resolver.GetCode(to_city);

        if (from.empty()) {
            std::cout << "Ошибка: Город '" << from_city << "' не найден в базе Яндекса.\n";
            continue;
        }
        if (to.empty()) {
            std::cout << "Ошибка: Город '" << to_city << "' не найден в базе Яндекса.\n";
            continue;
        }

        auto result = api.GetRoutes(from, to, date);
        if (result.has_value()) {
            PrintRoutes(result.value());
        } else {
            std::string err_str = result.error();
            if (err_str.find("404") != std::string::npos) {
                std::cerr << "К сожалению, Яндекс.Расписания не могут построить маршрут между этими городами.\n";
                size_t json_pos = err_str.find("{");
                if (json_pos != std::string::npos) {
                    try {
                        auto j = nlohmann::json::parse(err_str.substr(json_pos));
                        if (j.contains("error") && j["error"].contains("text") && j["error"]["text"].is_string()) {
                            std::cerr << "Ответ от API Яндекса: " << j["error"]["text"].get<std::string>() << "\n";
                        }
                    } catch (...) {}
                }
            } else {
                std::cerr << "Сетевая ошибка: " << err_str << "\n";
            }
        }
    }

    return 0;
}