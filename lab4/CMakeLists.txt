cmake_minimum_required(VERSION 3.20)
project(HamarcProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        hamarc
        main.cpp
        utils.cpp
        hamming.cpp
        cli.cpp
        archive.cpp
)