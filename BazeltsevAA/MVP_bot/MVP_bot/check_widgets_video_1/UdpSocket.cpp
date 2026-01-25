#include "UdpSocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

UdpSocket::UdpSocket()
    : sockfd(-1), hasRemote(false) {}

UdpSocket::~UdpSocket() {
    closeSocket();
}

bool UdpSocket::bindSocket(uint16_t port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    return bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == 0;
}

bool UdpSocket::setRemote(const std::string& ip, uint16_t port) {
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &remoteAddr.sin_addr) <= 0)
        return false;

    hasRemote = true;
    return true;
}

bool UdpSocket::sendData(const void* data, size_t size) {
    if (!hasRemote) return false;
    return sendto(sockfd, data, size, 0,
                  (sockaddr*)&remoteAddr,
                  sizeof(remoteAddr)) == (ssize_t)size;
}

ssize_t UdpSocket::receiveData(void* buffer, size_t size) {
    sockaddr_in sender{};
    socklen_t len = sizeof(sender);
    return recvfrom(sockfd, buffer, size, 0,
                    (sockaddr*)&sender, &len);
}

void UdpSocket::closeSocket() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}
