// command_sender.h
#pragma once
#ifndef COMMAND_SENDER_H
#define COMMAND_SENDER_H

// ВАЖНО: сначала winsock2, потом windows.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <conio.h>
#include <thread>
#include <atomic>
#include <string>
#include "config.h"

#pragma comment(lib, "ws2_32.lib")

class CommandSender {
private:
    std::atomic<bool> running;
    std::atomic<bool> connected;
    SOCKET clientSocket;
    char lastCommand;

    bool initializeWinsock() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Winsock initialization failed" << std::endl;
            return false;
        }
        return true;
    }

    bool connectToRobot() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed" << std::endl;
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(Config::COMMAND_PORT);

        // Используем старый способ вместо inet_pton
        serverAddr.sin_addr.s_addr = inet_addr(Config::RASPBERRY_IP.c_str());

        // Устанавливаем таймауты
        int timeout = 3000; // 3 секунды
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

        std::cout << "Connecting to robot at " << Config::RASPBERRY_IP << ":" << Config::COMMAND_PORT << "..." << std::endl;

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            return false;
        }

        connected = true;
        return true;
    }

    void sendCommand(char command) {
        if (!connected || clientSocket == INVALID_SOCKET) return;

        if (command != lastCommand) {
            lastCommand = command;

            char buffer[2] = { command, '\0' };
            int result = send(clientSocket, buffer, strlen(buffer), 0);

            if (result == SOCKET_ERROR) {
                std::cerr << "Send failed, trying to reconnect..." << std::endl;
                connected = false;
                closesocket(clientSocket);

                // Пытаемся переподключиться
                if (connectToRobot()) {
                    send(clientSocket, buffer, strlen(buffer), 0);
                }
            }
            else {
                // Вывод команды
                switch (command) {
                case 'w': std::cout << ">>> FORWARD" << std::endl; break;
                case 's': std::cout << ">>> BACKWARD" << std::endl; break;
                case 'a': std::cout << ">>> LEFT" << std::endl; break;
                case 'd': std::cout << ">>> RIGHT" << std::endl; break;
                case ' ': std::cout << ">>> STOP" << std::endl; break;
                case 'q': std::cout << ">>> QUIT" << std::endl; break;
                }
            }
        }
    }

public:
    CommandSender() : running(true), connected(false),
        clientSocket(INVALID_SOCKET), lastCommand(' ') {}

    ~CommandSender() {
        stop();
    }

    void initialize() {
        std::cout << "=== Command Sender Initialized ===" << std::endl;
        std::cout << "Target: " << Config::RASPBERRY_IP << ":" << Config::COMMAND_PORT << std::endl;

        if (!initializeWinsock()) {
            running = false;
            return;
        }

        if (!connectToRobot()) {
            std::cerr << "Failed to connect to robot!" << std::endl;
            running = false;
            return;
        }

        std::cout << "Connected successfully!" << std::endl;
        std::cout << "================================" << std::endl;
    }

    void start() {
        if (!connected) {
            std::cerr << "Not connected to robot!" << std::endl;
            return;
        }

        std::cout << "\n=== ROBOT CONTROLS ===" << std::endl;
        std::cout << "W - Forward" << std::endl;
        std::cout << "A - Left" << std::endl;
        std::cout << "S - Backward" << std::endl;
        std::cout << "D - Right" << std::endl;
        std::cout << "SPACE - Stop" << std::endl;
        std::cout << "Q - Quit" << std::endl;
        std::cout << "======================" << std::endl;
        std::cout << "Ready for commands..." << std::endl;

        // Очищаем буфер клавиатуры
        while (_kbhit()) _getch();

        while (running) {
            if (_kbhit()) {
                char key = _getch();
                key = tolower(key);

                if (key == 'q') {
                    running = false;
                    break;
                }
                else if (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == ' ') {
                    sendCommand(key);
                }
            }
            else {
                // Автостоп при отпускании
                if (lastCommand != ' ' && lastCommand != 'q') {
                    sendCommand(' ');
                }
            }

            Sleep(50);
        }

        // Отправляем финальный стоп
        if (connected) {
            sendCommand(' ');
        }
    }

    void stop() {
        running = false;

        if (connected) {
            if (clientSocket != INVALID_SOCKET) {
                closesocket(clientSocket);
            }
            WSACleanup();
            connected = false;
        }

        std::cout << "Command sender stopped" << std::endl;
    }

    bool isRunning() const {
        return running;
    }

    bool isConnected() const {
        return connected;
    }
};

#endif // COMMAND_SENDER_H