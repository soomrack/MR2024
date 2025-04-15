
cmake_minimum_required(VERSION 3.10)

project(Matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
        message(FATAL_ERROR "Требуется GCC версии 7 или выше для C++17")
    endif()
endif()

add_definitions(-DMATRIX_LOG_ENABLE)

set(SOURCES
    src/main.cpp
    src/matrix.cpp
)

set(HEADERS
    include/matrix.hpp
)

add_executable(matrix ${SOURCES} ${HEADERS})

target_include_directories(matrix PRIVATE include)
target_link_libraries(matrix Threads::Threads)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    target_compile_options(matrix PRIVATE -Wall -Wextra -Wpedantic)
endif()


