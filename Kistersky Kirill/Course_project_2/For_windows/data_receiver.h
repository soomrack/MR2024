// data_receiver.h
#pragma once
#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <string>

#include "config.h"

#pragma comment(lib, "ws2_32.lib")

class DataReceiver {
private:
    std::atomic<bool> running;
    SOCKET udpSocket;
    std::thread recvThread;

    bool initializeWinsock() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "DataReceiver: WSAStartup failed" << std::endl;
            return false;
        }
        return true;
    }

    void receiveLoop() {
        char buffer[1024];
        sockaddr_in from{};
        int fromLen = sizeof(from);

        std::cout << "DataReceiver: UDP listening on port "
            << Config::DATA_PORT_UDP << std::endl;

        while (running) {
            int bytes = recvfrom(
                udpSocket,
                buffer,
                sizeof(buffer) - 1,
                0,
                (sockaddr*)&from,
                &fromLen
            );

            if (bytes > 0) {
                buffer[bytes] = '\0';
                std::cout << "[SENSOR] " << buffer;
            }
            else {
                Sleep(10);
            }
        }
    }

public:
    DataReceiver() : running(false), udpSocket(INVALID_SOCKET) {}

    ~DataReceiver() {
        stop();
    }

    bool start() {
        if (!initializeWinsock())
            return false;

        udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket == INVALID_SOCKET) {
            std::cerr << "DataReceiver: UDP socket creation failed" << std::endl;
            return false;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(Config::DATA_PORT_UDP);

        if (bind(udpSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cerr << "DataReceiver: bind failed" << std::endl;
            closesocket(udpSocket);
            udpSocket = INVALID_SOCKET;
            return false;
        }

        running = true;
        recvThread = std::thread(&DataReceiver::receiveLoop, this);
        return true;
    }

    void stop() {
        running = false;

        if (udpSocket != INVALID_SOCKET) {
            closesocket(udpSocket);
            udpSocket = INVALID_SOCKET;
        }

        if (recvThread.joinable())
            recvThread.join();

        WSACleanup();
        std::cout << "DataReceiver stopped" << std::endl;
    }
};

#endif // DATA_RECEIVER_H
