cmake_minimum_required(VERSION 3.20)
project(Study)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
#     "*.cpp"
#     "Hamming_code/*.cpp"
#     "Sort/*.cpp"
# )

#add_executable(${PROJECT_NAME}
#        ${SOURCES}
#)
add_subdirectory(lab1)
add_subdirectory(lab4)