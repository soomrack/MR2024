cmake_minimum_required(VERSION 3.25)

project(task2_cpp)

add_executable(task2_cpp main.cpp)

target_include_directories(task2_cpp PRIVATE matrix)

add_library(matrix STATIC matrix/matrix.cpp)

target_link_libraries(task2_cpp matrix)
