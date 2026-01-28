cmake_minimum_required(VERSION 3.20)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
