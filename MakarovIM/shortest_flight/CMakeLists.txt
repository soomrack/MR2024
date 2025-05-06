cmake_minimum_required(VERSION 3.10)

project(shortest_flight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(shortest_flight src/main.cpp src/flight_graph.cpp src/logger.cpp src/csv_flight_parser.cpp src/utils.cpp)

target_include_directories(shortest_flight PUBLIC include)
