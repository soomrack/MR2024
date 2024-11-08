cmake_minimum_required(VERSION 3.25)

project(task2)

add_executable(task2 main.c)

target_include_directories(task2 PRIVATE matrix)

add_library(matrix STATIC matrix/matrix.c)

target_link_libraries(task2 matrix)