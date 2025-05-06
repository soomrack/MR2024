cmake_minimum_required(VERSION 3.10)
project(FlightPathPlanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
        message(FATAL_ERROR "Требуется GCC версии 7 или выше для C++17")
    endif()
endif()


set(SOURCES
    src/flightpathplanner.cpp
    src/main.cpp
)

set(HEADERS
    include/flightpathplanner.hpp
)

add_executable(planner ${SOURCES} ${HEADERS})

target_include_directories(planner PRIVATE include)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    target_compile_options(planner PRIVATE -Wall -Wextra -pedantic)
endif()

