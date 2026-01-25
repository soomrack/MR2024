#pragma once

#include <atomic>
#include <thread>
#include <chrono>

class Heartbeat {
public:
    Heartbeat(int socket_fd,
              int timeout_sec = 3);

    ~Heartbeat();

    void start();
    void stop();

    bool isOperatorConnected() const;

private:
    void loop();

    int sockfd;
    int timeout;

    std::atomic<bool> running;
    std::atomic<bool> connected;

    std::thread worker;

    std::chrono::steady_clock::time_point last_ping;
};
