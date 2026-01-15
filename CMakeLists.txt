cmake_minimum_required(VERSION 3.20)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        temp_flatmap.cpp
        temp_hashmap.cpp
        temp_vector.cpp
        test.cpp
)

add_subdirectory(Hamming_code)
add_subdirectory(Sort)