cmake_minimum_required(VERSION 3.20)
project(Lab7RouteSearch CXX)

# Включаем стандарт C++23 для работы с std::expected
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Собираем зависимости статически, чтобы не было проблем с DLL
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

# Подгружаем cpr (версия >= 1.11.0, как в задании)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.11.0
)

# Подгружаем nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(cpr nlohmann_json)

# Создаем статическую библиотеку с основной логикой приложения
add_library(app_logic STATIC
    cache/RouteCache.cpp
    api/YandexRaspAPI.cpp
    utils/RoutePrinter.cpp
    http/CprClient.cpp
)
# Делаем заголовки библиотеки доступными для других таргетов (например, для lab7 и run_tests)
target_include_directories(app_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(app_logic PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

# Основное приложение
add_executable(lab7 main.cpp)
target_link_libraries(lab7 PRIVATE app_logic)

# --- Тестирование ---
enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0 # Или более новая версия
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests tests/api_test.cpp) # MockHttpClient.h будет найден автоматически
target_link_libraries(run_tests PRIVATE app_logic gmock_main)

include(GoogleTest)
gtest_discover_tests(run_tests)