#include "connection.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cerrno>

static std::string now_readable()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm_info, "%H:%M:%S %d-%m-%Y");
    return oss.str();
}

ConnectionInfo wait_op_connection(TcpSocket& server, uint16_t port, const std::string& baseDir)
{
    static bool initialized = false;

    static bool operator_connected = false;
    static int  active_client_fd   = -1;
    static std::string active_ip;
    static uint16_t active_port = 0;

    // Инициализация TCP-сервера
    if (!initialized) {
        if (!server.bindAndListen(port)) {
            std::cerr << "[ERR] Failed to bind TCP server on port "
                      << port << std::endl;
            std::exit(1);
        }
        initialized = true;
        std::cout << "[CNT] TCP server listening on port "
                  << port << std::endl;
    }

    while (true) {

        if (operator_connected && active_client_fd >= 0) {

            char probe;
            ssize_t r = recv(active_client_fd, &probe, 1,
                             MSG_PEEK | MSG_DONTWAIT);

            if (r == 0) {
                // Клиент корректно закрыл соединение
                std::cout << "[CNT] Operator disconnected ("
                          << active_ip << ":" << active_port
                          << ") at " << now_readable() << std::endl;

                close(active_client_fd);
                operator_connected = false;
                active_client_fd   = -1;
                active_ip.clear();
                active_port = 0;
            }
            else if (r < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    // Ошибка сокета — считаем, что оператор пропал
                    std::cout << "[CNT] Operator connection lost ("
                              << active_ip << ":" << active_port
                              << ") at " << now_readable() << std::endl;

                    close(active_client_fd);
                    operator_connected = false;
                    active_client_fd   = -1;
                    active_ip.clear();
                    active_port = 0;
                }
            }
        }


        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);

        int clientFd = accept(server.getFd(),
                              (sockaddr*)&clientAddr,
                              &addrLen);

        if (clientFd < 0) {
            usleep(10000); // 10 мс
            continue;
        }

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET,
                  &clientAddr.sin_addr,
                  ipStr,
                  sizeof(ipStr));

        uint16_t clientPort = ntohs(clientAddr.sin_port);

        if (operator_connected) {
            std::cout << "[CNT] Rejecting extra connection from "
                      << ipStr << ":" << clientPort << std::endl;
            close(clientFd);
            continue;
        }

        // Возврат оператора
        operator_connected = true;
        active_client_fd   = clientFd;
        active_ip          = ipStr;
        active_port        = clientPort;

        std::string connect_time = now_readable();

        std::cout << "[CNT] Operator connected from "
                  << ipStr << ":" << clientPort
                  << " at " << connect_time << std::endl;

        // Записываем файл сессии
        std::ofstream sessionFile(
            baseDir + "/current_session.txt",
            std::ios::out | std::ios::trunc);

        if (sessionFile.is_open()) {
            sessionFile << "operator_ip=" << ipStr << "\n";
            sessionFile << "operator_port=" << clientPort << "\n";
            sessionFile << "connect_time=" << connect_time << "\n";
            sessionFile.close();
        }

        ConnectionInfo info;
        info.socket_fd     = clientFd;
        info.operator_ip   = ipStr;
        info.operator_port = clientPort;
        info.connect_time  =
            std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());

        return info;
    }
}
