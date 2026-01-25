#pragma once

#include <string>
#include <cstdint>
#include <arpa/inet.h>

class UdpSocket {
public:
    UdpSocket();
    ~UdpSocket();

    bool bindSocket(uint16_t port);
    bool setRemote(const std::string& ip, uint16_t port);

    bool sendData(const void* data, size_t size);
    ssize_t receiveData(void* buffer, size_t size);

    void closeSocket();

private:
    int sockfd;
    sockaddr_in remoteAddr;
    bool hasRemote;
};
