#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

namespace Config
{
inline const QString RASPBERRY_IP = "10.175.207.101";

inline constexpr int COMMAND_PORT = 8888;
inline constexpr int DATA_PORT_UDP = 5601;
inline constexpr int VIDEO_PORT = 5600;

inline const QString GSTREAMER_PATH =
    "C:/gstreamer/1.28/mingw_x86_64/bin";
}

#endif // CONFIG_H
