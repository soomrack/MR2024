cmake_minimum_required(VERSION 3.10)
project(b_plus_tree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
list(APPEND SOURCES "main.cpp")

add_executable(b_plus_tree_app ${SOURCES})
