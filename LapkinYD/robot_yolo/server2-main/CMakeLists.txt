cmake_minimum_required(VERSION 3.5)
project(server)

# Установка путей для MSYS2
# set(MSYS2_PREFIX "H:/msys2/mingw64")  # или "C:/msys64/ucrt64" в зависимости от вашего окружения

# # Настройка компилятора
# set(CMAKE_C_COMPILER "${MSYS2_PREFIX}/bin/gcc.exe")
# set(CMAKE_CXX_COMPILER "${MSYS2_PREFIX}/bin/g++.exe")

# set(CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 17)

# Пути для pkg-config
# set(ENV{PKG_CONFIG_PATH} "${MSYS2_PREFIX}/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0 gstreamer-rtp-1.0 gstreamer-plugins-bad-1.0)

add_executable(heartbeat ${CMAKE_SOURCE_DIR}/heartbeat.cpp)
add_executable(robot ${CMAKE_SOURCE_DIR}/robot.cpp)
add_executable(text_server ${CMAKE_SOURCE_DIR}/text_server.cpp)
add_executable(video_sender ${CMAKE_SOURCE_DIR}/video_sender.c)

target_link_libraries(heartbeat ${GSTREAMER_LIBRARIES})
target_link_libraries(robot ${GSTREAMER_LIBRARIES})
target_link_libraries(text_server ${GSTREAMER_LIBRARIES})
target_link_libraries(video_sender ${GSTREAMER_LIBRARIES})

target_include_directories(heartbeat PRIVATE ${GSTREAMER_INCLUDE_DIRS})
target_include_directories(robot PRIVATE ${GSTREAMER_INCLUDE_DIRS})
target_include_directories(text_server PRIVATE ${GSTREAMER_INCLUDE_DIRS})
target_include_directories(video_sender PRIVATE ${GSTREAMER_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(heartbeat ws2_32)
    
    # Для MSYS2/MinGW нужен pthread
    target_link_libraries(heartbeat pthread)
endif()

# Linux специфичные настройки
if(UNIX AND NOT APPLE)
    target_link_libraries(heartbeat pthread)
endif()

target_link_libraries(robot pthread)

# target_compile_definitions(heartbeat PRIVATE ${GSTREAMER_CFLAGS_OTHER})
# target_compile_definitions(robot PRIVATE ${GSTREAMER_CFLAGS_OTHER})
# target_compile_definitions(text_server PRIVATE ${GSTREAMER_CFLAGS_OTHER})
# target_compile_definitions(video_sender PRIVATE ${GSTREAMER_CFLAGS_OTHER})