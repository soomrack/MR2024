#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define DATA_PORT_UDP 5601
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(DATA_PORT_UDP);

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "=====================================\n";
    std::cout << "   SENSOR DATA RECEIVER (UDP)\n";
    std::cout << "   Listening on port " << DATA_PORT_UDP << "\n";
    std::cout << "=====================================\n\n";

    char buffer[BUFFER_SIZE];

    while (true) {
        int len = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (len > 0) {
            buffer[len] = '\0';
            std::cout << buffer;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
