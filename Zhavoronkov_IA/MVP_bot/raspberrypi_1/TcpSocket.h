#pragma once

#include <string>
#include <cstdint>

class TcpSocket {
public:
    TcpSocket();
    ~TcpSocket();

    bool bindAndListen(uint16_t port, int backlog = 1);
    int  acceptClient();

    bool connectTo(const std::string& ip, uint16_t port);

    bool sendData(const void* data, size_t size);
    ssize_t receiveData(void* buffer, size_t size);

    void closeSocket();
    int  getFd() const;

private:
    int sockfd;
    bool isServer;
};

