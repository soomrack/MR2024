cmake_minimum_required(VERSION 3.10)
project(client)

set(MSYS2_PREFIX "C:/msys64/ucrt64")
set(CMAKE_C_COMPILER   "${MSYS2_PREFIX}/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "${MSYS2_PREFIX}/bin/g++.exe")
set(CMAKE_GENERATOR "MSYS Makefiles" CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt5 COMPONENTS Core Widgets OpenGL Network REQUIRED)

set(ENV{PKG_CONFIG_PATH} "${MSYS2_PREFIX}/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED
    gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0
    gstreamer-rtp-1.0 gstreamer-plugins-bad-1.0)

set(OpenCV_DIR "${MSYS2_PREFIX}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED core dnn imgproc)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)

add_executable(heartbeat ${CMAKE_SOURCE_DIR}/heartbeat.cpp)

add_executable(client
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/src/client.cpp
    ${CMAKE_SOURCE_DIR}/src/video_widget.cpp
    ${CMAKE_SOURCE_DIR}/src/main_window.cpp
    ${CMAKE_SOURCE_DIR}/src/keyboard_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/command_sender.cpp
    ${CMAKE_SOURCE_DIR}/src/yolo_detector.cpp
    ${CMAKE_SOURCE_DIR}/src/ocr_detector.cpp
    ${CMAKE_SOURCE_DIR}/src/sensor_receiver.cpp
    ${CMAKE_SOURCE_DIR}/include/video_widget.h
    ${CMAKE_SOURCE_DIR}/include/keyboard_handler.h
    ${CMAKE_SOURCE_DIR}/include/main_window.h
    ${CMAKE_SOURCE_DIR}/include/command_sender.h
    ${CMAKE_SOURCE_DIR}/include/client.h
    ${CMAKE_SOURCE_DIR}/include/yolo_detector.h
    ${CMAKE_SOURCE_DIR}/include/sensor_receiver.h
    ${CMAKE_SOURCE_DIR}/include/ocr_detector.h
)

target_link_libraries(client PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Network
    ${GSTREAMER_LIBRARIES}
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
)

target_link_libraries(heartbeat ${GSTREAMER_LIBRARIES})

if(WIN32)
    target_link_libraries(heartbeat ws2_32 pthread)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(heartbeat pthread)
endif()

target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GSTREAMER_INCLUDE_DIRS}
    ${Qt5_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TESSERACT_INCLUDE_DIRS}
)

target_include_directories(heartbeat PRIVATE ${GSTREAMER_INCLUDE_DIRS})
target_compile_definitions(heartbeat PRIVATE ${GSTREAMER_CFLAGS_OTHER})
target_compile_options(client PRIVATE ${GSTREAMER_CFLAGS_OTHER})
