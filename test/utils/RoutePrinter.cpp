#include "RoutePrinter.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Форматирование даты и времени из "2026-03-27T06:55:00+03:00" в "2026-03-27 06:55"
std::string format_datetime(const std::string& raw) {
    if (raw.length() >= 16 && raw[10] == 'T') {
        return raw.substr(0, 10) + " " + raw.substr(11, 5);
    }
    return raw;
}

// Перевод типов транспорта на русский язык
std::string translate_transport(const std::string& type) {
    if (type == "bus") return "автобус";
    if (type == "train") return "поезд";
    if (type == "suburban") return "электричка";
    if (type == "plane") return "самолёт";
    if (type == "water") return "водный транспорт";
    if (type == "helicopter") return "вертолёт";
    return type;
}

// Парсинг JSON и вывод данных в консоль
void print_routes(const std::string& json_str) {
    try {
        auto j = json::parse(json_str);
        if (j.contains("error")) {
            std::cerr << "Ошибка со стороны Yandex API: " << j["error"].get<std::string>() << "\n";
            return;
        }
        if (!j.contains("segments") || j["segments"].empty()) {
            std::cout << "Маршруты не найдены.\n";
            return;
        }

        std::vector<std::pair<std::string, std::string>> parsed_routes;
        for (const auto& segment : j["segments"]) {
            bool has_transfers = segment.value("has_transfers", false);
            std::vector<json> travel_legs;

            // Если есть пересадки, извлекаем фактические участки пути
            if (has_transfers && segment.contains("details")) {
                for (const auto& detail : segment["details"]) {
                    if (detail.contains("thread")) {
                        travel_legs.push_back(detail);
                    }
                }
            } else if (!has_transfers) {
                travel_legs.push_back(segment);
            }

            int transfer_count = travel_legs.empty() ? 0 : static_cast<int>(travel_legs.size()) - 1;
            if (transfer_count > 1) continue; 

            std::string total_dep = format_datetime(segment.value("departure", "Неизвестно"));
            std::string total_arr = format_datetime(segment.value("arrival", "Неизвестно"));

            std::ostringstream oss;

            if (transfer_count == 0) {
                std::string title = "Неизвестный маршрут";
                std::string transport = "Неизвестно";
                const auto& target = travel_legs.empty() ? segment : travel_legs.front();
                
                std::string from_station = (target.contains("from") && target["from"].is_object()) ? target["from"].value("title", "Неизвестно") : "Неизвестно";
                std::string to_station = (target.contains("to") && target["to"].is_object()) ? target["to"].value("title", "Неизвестно") : "Неизвестно";
                title = from_station + " — " + to_station;

                if (target.contains("thread")) {
                    std::string thread_title = target["thread"].value("title", "");
                    if (!thread_title.empty() && thread_title != title) {
                        title += " (Рейс: " + thread_title + ")";
                    } else if (!thread_title.empty()) {
                        title = thread_title;
                    }
                    transport = translate_transport(target["thread"].value("transport_type", "Неизвестно"));
                }
                
                oss << total_dep << " - " << total_arr << " | Тип: " << transport << " | " << title << " | Прямой рейс\n";
            } else {
                oss << total_dep << " - " << total_arr << " | Маршрут с пересадкой (" << transfer_count << ")\n";
                
                int step = 1;
                for (const auto& leg : travel_legs) {
                    std::string leg_title = "Неизвестный маршрут";
                    std::string leg_transport = "Неизвестно";
                    
                    std::string leg_from = (leg.contains("from") && leg["from"].is_object()) ? leg["from"].value("title", "Неизвестно") : "Неизвестно";
                    std::string leg_to = (leg.contains("to") && leg["to"].is_object()) ? leg["to"].value("title", "Неизвестно") : "Неизвестно";
                    leg_title = leg_from + " — " + leg_to;

                    if (leg.contains("thread")) {
                        std::string thread_title = leg["thread"].value("title", "");
                        if (!thread_title.empty() && thread_title != leg_title) {
                            leg_title += " (Рейс: " + thread_title + ")";
                        } else if (!thread_title.empty()) {
                            leg_title = thread_title;
                        }
                        leg_transport = translate_transport(leg["thread"].value("transport_type", "Неизвестно"));
                    }
                    
                    std::string leg_dep = format_datetime(leg.value("departure", "Неизвестно"));
                    std::string leg_arr = format_datetime(leg.value("arrival", "Неизвестно"));
                    
                    oss << "      - [Участок " << step++ << "] " << leg_dep << " - " << leg_arr << " | Тип: " << leg_transport << " | " << leg_title << "\n";
                }
            }
            parsed_routes.push_back({segment.value("departure", ""), oss.str()});
        }

        std::sort(parsed_routes.begin(), parsed_routes.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        std::vector<std::string> routes_output;
        int current_idx = 1;
        for (const auto& pr : parsed_routes) {
            routes_output.push_back("[" + std::to_string(current_idx++) + "] " + pr.second);
        }

        if (routes_output.empty()) {
            std::cout << "Подходящие маршруты (максимум 1 пересадка) не найдены.\n";
            return;
        }

        int total = static_cast<int>(routes_output.size());
        int displayed = 0;
        while (displayed < total) {
            int chunk = (total - displayed < 7) ? (total - displayed) : 7;
            for (int i = 0; i < chunk; ++i) { std::cout << routes_output[displayed + i]; }
            displayed += chunk;
            if (displayed < total) {
                std::cout << "\nПоказано " << displayed << " из " << total << " маршрутов.\nПоказать еще? (y - да, n - отмена): ";
                std::string ans; std::cin >> ans; if (ans == "n" || ans == "N") break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга JSON (std::exception): " << e.what() << "\n";
    }
}