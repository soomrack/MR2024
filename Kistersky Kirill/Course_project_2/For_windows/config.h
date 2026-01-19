// config.h
#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Configuration flags - set to 0 to disable components
#define ENABLE_VIDEO_STREAM   1  // Set to 0 to disable video
#define ENABLE_COMMAND_SENDER 1  // Set to 0 to disable command sending
#define ENABLE_DATA_RECEIVER  0  // Set to 0 to disable data receiving


namespace Config {
    // IP адрес Raspberry Pi
    const std::string RASPBERRY_IP = "10.133.231.101";

    // Порт для команд
    const int COMMAND_PORT = 8888;
    const int DATA_PORT_UDP = 5601;     // Для получения данных

    // Порт для видео
    const int VIDEO_PORT = 5600;

    // Путь к GStreamer
    const std::string GSTREAMER_PATH = "C:\\gstreamer\\1.0\\mingw_x86\\bin";

    // Путь к папке с видео
    const std::string RECORDING_PATH = "C:\\Telemetry_recorded";
}

#endif // CONFIG_H