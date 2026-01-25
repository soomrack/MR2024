#pragma once
#include <string>
#include <ctime>

struct ConnectionInfo {
    int         socket_fd;
    std::string operator_ip;
    uint16_t    operator_port;
    std::time_t connect_time;
};

