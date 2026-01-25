#include "Heartbeat.h"

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

static constexpr char PING_MSG[] = "PING";
static constexpr char PONG_MSG[] = "PONG";

Heartbeat::Heartbeat(int socket_fd, int timeout_sec)
    : sockfd(socket_fd),
      timeout(timeout_sec),
      running(false),
      connected(true)
{
    last_ping = std::chrono::steady_clock::now();
}

Heartbeat::~Heartbeat() {
    stop();
}

void Heartbeat::start() {
    running = true;
    worker = std::thread(&Heartbeat::loop, this);
}

void Heartbeat::stop() {
    running = false;
    if (worker.joinable())
        worker.join();
}

bool Heartbeat::isOperatorConnected() const {
    return connected;
}

void Heartbeat::loop()
{
    char buffer[16];

    while (running) {
        ssize_t r = recv(sockfd,
                          buffer,
                          sizeof(buffer) - 1,
                          MSG_DONTWAIT);

        if (r > 0) {
            buffer[r] = '\0';

            if (std::strncmp(buffer, PING_MSG, 4) == 0) {
                last_ping = std::chrono::steady_clock::now();

                // Ответ PONG
                send(sockfd, PONG_MSG, 4, 0);
            }
        }
        else if (r == 0) {
            // TCP закрыт клиентом
            std::cout << "[CNT] Operator disconnected (TCP closed)\n";
            connected = false;
            running = false;
            break;
        }

        // Проверка таймаута
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(
                        now - last_ping).count();

        if (diff > timeout) {
            std::cout << "[CNT] Operator heartbeat timeout\n";
            connected = false;
            running = false;
            break;
        }

        usleep(100 * 1000); // 100 мс
    }
}
