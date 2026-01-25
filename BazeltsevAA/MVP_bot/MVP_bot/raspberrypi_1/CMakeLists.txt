cmake_minimum_required(VERSION 3.22)
project(MVP_robot)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(MVP_robot
        connection.cpp
        connection.h
        ConnectionInfo.h
        main.cpp
        TcpSocket.cpp
        TcpSocket.h
        UdpSocket.cpp
        UdpSocket.h
        Heartbeat.cpp
        Heartbeat.h
        VideoStreamer.cpp
        VideoStreamer.h
        CommandProcessor.cpp
        CommandProcessor.h
        RollbackExecutor.cpp
        RollbackExecutor.h
        main.cpp)

target_include_directories(MVP_robot 
   PRIVATE ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(MVP_robot 
    ${OpenCV_LIBS}
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
)
