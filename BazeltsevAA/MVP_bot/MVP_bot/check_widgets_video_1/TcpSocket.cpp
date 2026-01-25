#include "TcpSocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

TcpSocket::TcpSocket()
    : sockfd(-1), isServer(false) {}

TcpSocket::~TcpSocket() {
    closeSocket();
}

bool TcpSocket::bindAndListen(uint16_t port, int backlog) {
    isServer = true;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return false;

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
        return false;

    if (listen(sockfd, backlog) < 0)
        return false;

    return true;
}

int TcpSocket::acceptClient() {
    if (!isServer) return -1;

    sockaddr_in clientAddr{};
    socklen_t len = sizeof(clientAddr);
    return accept(sockfd, (sockaddr*)&clientAddr, &len);
}

bool TcpSocket::connectTo(const std::string& ip, uint16_t port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
        return false;

    if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
        return false;

    return true;
}

bool TcpSocket::sendData(const void* data, size_t size) {
    return send(sockfd, data, size, 0) == (ssize_t)size;
}

ssize_t TcpSocket::receiveData(void* buffer, size_t size) {
    return recv(sockfd, buffer, size, 0);
}

void TcpSocket::closeSocket() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

int TcpSocket::getFd() const {
    return sockfd;
}
