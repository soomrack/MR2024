cmake_minimum_required(VERSION 4.0)
project(matrix-cpp)

add_executable(matrix-cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/matrix-cpp.cpp
)