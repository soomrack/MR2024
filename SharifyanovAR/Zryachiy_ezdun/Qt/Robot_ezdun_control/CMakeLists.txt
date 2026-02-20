cmake_minimum_required(VERSION 3.19)
project(Robot_ezdun_control LANGUAGES CXX)

# Путь к GStreamer (твой новый путь)
set(GSTREAMER_ROOT "C:/gstreamer/1.28/mingw_x86_64")

# Проверяем существование пути
if(NOT EXISTS ${GSTREAMER_ROOT})
    message(WARNING "GStreamer not found at ${GSTREAMER_ROOT}. Video streaming will not work!")
    message(WARNING "Please install GStreamer from: https://gstreamer.freedesktop.org/download/")
    set(GSTREAMER_FOUND FALSE)
else()
    set(GSTREAMER_FOUND TRUE)
    message(STATUS "GStreamer found at ${GSTREAMER_ROOT}")

    # Пути к заголовкам GStreamer
    set(GSTREAMER_INCLUDE_DIRS
        ${GSTREAMER_ROOT}/include
        ${GSTREAMER_ROOT}/include/gstreamer-1.0
        ${GSTREAMER_ROOT}/include/glib-2.0
        ${GSTREAMER_ROOT}/lib/glib-2.0/include
    )

    # Пути к библиотекам GStreamer
    set(GSTREAMER_LIBRARY_DIRS
        ${GSTREAMER_ROOT}/lib
    )

    # Список библиотек GStreamer для линковки
    set(GSTREAMER_LIBRARIES
        gstreamer-1.0
        gstapp-1.0
        gobject-2.0
        glib-2.0
        intl
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
)

target_include_directories(Robot_ezdun_control PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Добавляем GStreamer только если найден
if(GSTREAMER_FOUND)
    target_include_directories(Robot_ezdun_control PRIVATE ${GSTREAMER_INCLUDE_DIRS})
    target_link_directories(Robot_ezdun_control PRIVATE ${GSTREAMER_LIBRARY_DIRS})

    # Линкуем библиотеки GStreamer
    target_link_libraries(Robot_ezdun_control PRIVATE
        ${GSTREAMER_LIBRARY_DIRS}/libgstreamer-1.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libgstapp-1.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libgobject-2.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libglib-2.0.dll.a
        ${GSTREAMER_LIBRARY_DIRS}/libintl.dll.a
    )

    target_compile_definitions(Robot_ezdun_control PRIVATE HAVE_GSTREAMER=1)
    message(STATUS "GStreamer libraries linked successfully")
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
