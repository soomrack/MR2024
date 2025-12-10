// config.h
#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    // IP адрес Raspberry Pi
    const std::string RASPBERRY_IP = "10.133.231.101";

    // Порт для команд
    const int COMMAND_PORT = 8888;

    // Порт для видео
    const int VIDEO_PORT = 5600;

    // Путь к GStreamer
    const std::string GSTREAMER_PATH = "C:\\gstreamer\\1.0\\mingw_x86\\bin";
}

#endif // CONFIG_H