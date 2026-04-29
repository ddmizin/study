cmake_minimum_required(VERSION 3.20)
project(Lab7RouteSearch CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.11.0
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(cpr nlohmann_json)

add_library(app_logic STATIC
    cache/RouteCache.cpp
    api/YandexRaspAPI.cpp
    utils/RoutePrinter.cpp
    http/CprClient.cpp
)
target_include_directories(app_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(app_logic PUBLIC cpr::cpr nlohmann_json::nlohmann_json)

add_executable(lab7 main.cpp)
target_link_libraries(lab7 PRIVATE app_logic)

enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
    tests/api_test.cpp
    tests/cache_test.cpp
    tests/printer_test.cpp
)
target_link_libraries(run_tests PRIVATE app_logic gmock_main nlohmann_json::nlohmann_json)

include(GoogleTest)
gtest_discover_tests(run_tests)
include(tests/coverage.cmake)