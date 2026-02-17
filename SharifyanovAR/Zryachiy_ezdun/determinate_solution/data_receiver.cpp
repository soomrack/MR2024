#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>

#include "../Video_streamer/config.h"
#include "data_logger.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

SOCKET g_socket = INVALID_SOCKET;
DataLogger g_logger;
bool g_running = true;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        std::cout << "\nCtrl+C detected. Shutting down...\n";
        g_running = false;
        return TRUE;
    }
    return FALSE;
}

void cleanup() {
    if (g_socket != INVALID_SOCKET) {
        closesocket(g_socket);
        g_socket = INVALID_SOCKET;
    }

    g_logger.close();
    WSACleanup();

    std::cout << "Resources released. Safe to restart.\n";
}

int main(int argc, char* argv[]) {

    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    std::cout << "=====================================\n";
    std::cout << "   SENSOR DATA RECEIVER (UDP)\n";
    std::cout << "=====================================\n";

    if (argc > 1)
        std::cout << "Robot IP: " << argv[1] << "\n";

    std::cout << "Listening on port " << Config::DATA_PORT_UDP << "\n";

    if (!g_logger.initialize()) {
        std::cerr << "Failed to initialize logger\n";
        return 1;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    g_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: "
            << WSAGetLastError() << "\n";
        cleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(g_socket, SOL_SOCKET, SO_REUSEADDR,
        (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(Config::DATA_PORT_UDP);

    if (bind(g_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: "
            << WSAGetLastError() << "\n";
        cleanup();
        return 1;
    }

    std::cout << "Waiting for sensor data...\n";

    char buffer[BUFFER_SIZE];
    sockaddr_in sender{};
    int sender_len = sizeof(sender);

    while (g_running) {

        int len = recvfrom(g_socket, buffer,
            BUFFER_SIZE - 1, 0,
            (sockaddr*)&sender,
            &sender_len);

        if (len > 0) {
            buffer[len] = '\0';

            SYSTEMTIME st;
            GetLocalTime(&st);

            printf("[%02d:%02d:%02d.%03d] %s",
                st.wHour, st.wMinute,
                st.wSecond, st.wMilliseconds,
                buffer);

            g_logger.log(buffer);
        }

        Sleep(5);
    }

    cleanup();
    return 0;
}
