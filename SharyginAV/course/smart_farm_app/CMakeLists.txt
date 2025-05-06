cmake_minimum_required(VERSION 3.16)

project(smart_farm_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick SerialPort)

qt_standard_project_setup()

qt_add_executable(smart_farm_app
  main.cpp
)

qt_add_qml_module(smart_farm_app
    URI path
    VERSION 1.0
    QML_FILES main.qml
    SOURCES sensors_and_leds_handler.h sensors_and_leds_handler.cpp
)

target_link_libraries(smart_farm_app PRIVATE Qt6::Core Qt6::Quick Qt6::SerialPort)

include(GNUInstallDirs)
install(TARGETS smart_farm_app
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
