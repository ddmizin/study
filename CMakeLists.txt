cmake_minimum_required(VERSION 3.20)
project(main)
# Включаем поддержку последних стандартов
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Автоматически повышаем стандарт если возможно
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    check_cxx_compiler_flag(-std=c++2c HAS_CPP2C)
    if(HAS_CPP2C)
        set(CMAKE_CXX_STANDARD 26)
        add_compile_options(-std=c++2c)
    endif()
endif()

add_subdirectory(classes)

add_executable(${PROJECT_NAME}
        commands.cpp
        data_types.cpp
        enum.cpp
        fstream.cpp
        pointer_links.cpp
        structure.cpp
        test.cpp
        test2.cpp
        useful.cpp
        vector_string.cpp
        while_for_if_switch.cpp
        classes.cpp
)
