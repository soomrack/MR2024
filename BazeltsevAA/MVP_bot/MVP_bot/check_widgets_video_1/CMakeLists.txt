cmake_minimum_required(VERSION 3.5)

project(check_widgets VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


find_package(PkgConfig REQUIRED)

find_package(OpenCV REQUIRED) 

pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)


include_directories(
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWSCALE_INCLUDE_DIRS}
)

#

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        TcpSocket.h
        TcpSocket.cpp
        UdpSocket.h
        UdpSocket.cpp
        VideoUdpReceiver.h VideoUdpReceiver.cpp
        KeyWindow.h KeyWindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(check_widgets
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(check_widgets SHARED
            ${PROJECT_SOURCES}
        )

    else()
        add_executable(check_widgets
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(check_widgets
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${OpenCV_LIBS} 
        ${AVFORMAT_LIBRARIES}
        ${AVCODEC_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${SWSCALE_LIBRARIES}
)

target_include_directories(check_widgets PRIVATE ${OpenCV_INCLUDE_DIRS})


set_target_properties(check_widgets PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(check_widgets)
endif()
