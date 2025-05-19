cmake_minimum_required(VERSION 3.10)
project(capture)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)
find_package(OpenCV REQUIRED)

include_directories(
    ${LIBCAMERA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

link_directories(
    ${LIBCAMERA_LIBRARY_DIRS}
)

add_executable(capture cam_with_trackbar.cpp)
target_link_libraries(capture
    ${LIBCAMERA_LIBRARIES}
    ${OpenCV_LIBS}
)