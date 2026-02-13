// data_receiver.cpp - ПРОЕКТ B
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include "../Video_streamer/config.h" // ВАЖНО: подключаем общий конфиг

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

int main() {
    // Загружаем IP из файла (нужен только для информации)
    std::cout << "=====================================\n";
    std::cout << "   SENSOR DATA RECEIVER (UDP)\n";
    std::cout << "=====================================\n";
    std::cout << "Loaded robot IP: " << Config::RASPBERRY_IP << "\n";
    std::cout << "Listening on port " << Config::DATA_PORT_UDP << "\n";
    std::cout << "Waiting for sensor data...\n";
    std::cout << "=====================================\n\n";

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Разрешаем повторное использование адреса
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(Config::DATA_PORT_UDP);

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed on port " << Config::DATA_PORT_UDP
            << " error: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char buffer[BUFFER_SIZE];
    sockaddr_in sender{};
    int sender_len = sizeof(sender);

    while (true) {
        int len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
            (sockaddr*)&sender, &sender_len);

        if (len > 0) {
            buffer[len] = '\0';

            SYSTEMTIME st;
            GetLocalTime(&st);

            printf("[%02d:%02d:%02d.%03d] %s",
                st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
                buffer);
        }
        Sleep(10);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}