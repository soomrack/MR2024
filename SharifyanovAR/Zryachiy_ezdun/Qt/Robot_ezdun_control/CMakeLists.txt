cmake_minimum_required(VERSION 3.19)
project(Robot_ezdun_control LANGUAGES CXX)

# Путь к GStreamer
set(GSTREAMER_ROOT "C:/gstreamer/1.28/mingw_x86_64")

# ===== ПУТЬ К СОБРАННОМУ OPENCV 4.9.0 =====
set(OPENCV_ROOT "C:/opencv-4.9.0/build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug/install")
set(OPENCV_INCLUDE_DIR "${OPENCV_ROOT}/include")
set(OPENCV_LIB_DIR "${OPENCV_ROOT}/x64/mingw/lib")
set(OPENCV_BIN_DIR "${OPENCV_ROOT}/x64/mingw/bin")

message(STATUS "========================================")
message(STATUS "OpenCV 4.9.0 configuration:")
message(STATUS "  Root: ${OPENCV_ROOT}")
message(STATUS "  Include: ${OPENCV_INCLUDE_DIR}")
message(STATUS "  Lib dir: ${OPENCV_LIB_DIR}")
message(STATUS "  Bin dir: ${OPENCV_BIN_DIR}")
message(STATUS "========================================")

# Проверяем существование пути GStreamer
if(NOT EXISTS ${GSTREAMER_ROOT})
    message(WARNING "GStreamer not found at ${GSTREAMER_ROOT}. Video streaming will not work!")
    set(GSTREAMER_FOUND FALSE)
else()
    set(GSTREAMER_FOUND TRUE)
    message(STATUS "GStreamer found at ${GSTREAMER_ROOT}")

    set(GSTREAMER_INCLUDE_DIRS
        ${GSTREAMER_ROOT}/include
        ${GSTREAMER_ROOT}/include/gstreamer-1.0
        ${GSTREAMER_ROOT}/include/glib-2.0
        ${GSTREAMER_ROOT}/lib/glib-2.0/include
    )

    set(GSTREAMER_LIBRARY_DIRS
        ${GSTREAMER_ROOT}/lib
    )
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Multimedia MultimediaWidgets)

qt_standard_project_setup()

qt_add_executable(Robot_ezdun_control
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    command_sender.h
    command_sender.cpp
    command_logger.h
    video_streamer.h
    video_streamer.cpp
    config.h
    data_logger.h
    data_receiver.h
    data_logger.cpp
    data_receiver.cpp
    data_logger.h
    batch_command_sender.h
    batch_command_sender.cpp
    yolo_detector.h
    yolo_detector.cpp
    dual_video_widget.h
    dual_video_widget.cpp
)

target_include_directories(Robot_ezdun_control PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ===== ПОДКЛЮЧАЕМ OPENCV 4.9.0 (WORLD) =====
if(EXISTS ${OPENCV_INCLUDE_DIR})
    message(STATUS "✅ OpenCV include found at ${OPENCV_INCLUDE_DIR}")
    target_include_directories(Robot_ezdun_control PRIVATE ${OPENCV_INCLUDE_DIR})

    # Ищем библиотеку opencv_world
    file(GLOB OPENCV_WORLD_LIB
        "${OPENCV_LIB_DIR}/libopencv_world*.a"
    )

    if(OPENCV_WORLD_LIB)
        message(STATUS "✅ OpenCV world library found: ${OPENCV_WORLD_LIB}")
        target_link_libraries(Robot_ezdun_control PRIVATE ${OPENCV_WORLD_LIB})
        target_compile_definitions(Robot_ezdun_control PRIVATE HAVE_OPENCV=1)
    else()
        message(FATAL_ERROR "❌ OpenCV world library not found in ${OPENCV_LIB_DIR}")
    endif()

    # Добавляем путь к DLL для runtime
    target_compile_definitions(Robot_ezdun_control PRIVATE
        OPENCV_BIN_DIR="${OPENCV_BIN_DIR}")
else()
    message(FATAL_ERROR "❌ OpenCV include directory not found at ${OPENCV_INCLUDE_DIR}")
endif()

# Добавляем GStreamer если найден
if(GSTREAMER_FOUND)
    target_include_directories(Robot_ezdun_control PRIVATE ${GSTREAMER_INCLUDE_DIRS})
    target_link_directories(Robot_ezdun_control PRIVATE ${GSTREAMER_LIBRARY_DIRS})

    target_link_libraries(Robot_ezdun_control PRIVATE
        ${GSTREAMER_LIBRARY_DIRS}/libgstreamer-1.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libgstapp-1.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libgobject-2.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libglib-2.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libintl.dll.a
    )

    target_compile_definitions(Robot_ezdun_control PRIVATE HAVE_GSTREAMER=1)
    message(STATUS "✅ GStreamer libraries linked successfully")
else()
    target_compile_definitions(Robot_ezdun_control PRIVATE HAVE_GSTREAMER=0)
endif()

target_link_libraries(Robot_ezdun_control PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

if(WIN32)
    target_link_libraries(Robot_ezdun_control PRIVATE
        ws2_32
        winmm
    )
endif()

include(GNUInstallDirs)

install(TARGETS Robot_ezdun_control
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Robot_ezdun_control
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
