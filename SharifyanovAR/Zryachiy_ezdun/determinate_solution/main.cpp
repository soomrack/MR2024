// main.cpp
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <iostream>

#include "config.h"
#include "video_streamer.h"
#include "command_sender.h"

void showBanner() {
    std::cout << "==========================================" << std::endl;
    std::cout << "        ROBOT CONTROL CENTER v1.0         " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Robot IP: " << Config::RASPBERRY_IP << std::endl;
    std::cout << "Command port: " << Config::COMMAND_PORT << std::endl;
    std::cout << "Video port: " << Config::VIDEO_PORT << std::endl;
    std::cout << "==========================================" << std::endl;
}

int main() {
    showBanner();

    // Инициализируем компоненты
    VideoStreamer videoStreamer;
    CommandSender commandSender;

    std::cout << "\nInitializing components..." << std::endl;

    // Инициализация
    videoStreamer.initialize();
    commandSender.initialize();

    // Проверяем подключение к роботу
    if (!commandSender.isConnected()) {
        std::cerr << "\nERROR: Cannot connect to robot!" << std::endl;
        std::cerr << "Make sure:" << std::endl;
        std::cerr << "1. Raspberry Pi is on and connected to WiFi" << std::endl;
        std::cerr << "2. Server is running on Raspberry Pi" << std::endl;
        std::cerr << "3. IP address is correct: " << Config::RASPBERRY_IP << std::endl;

        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "\nStarting all services..." << std::endl;
    std::cout << "=============================" << std::endl;

    // 1. Запускаем видео поток
    std::cout << "1. Starting video stream..." << std::endl;
    videoStreamer.start();
    std::cout << "   Waiting 3 seconds for GStreamer..." << std::endl;
    Sleep(3000);

    // 2. Запускаем управление командами
    std::cout << "\n2. Starting command controller..." << std::endl;
    std::cout << "   Ready for input..." << std::endl;

    commandSender.start();

    // 3. Ждем завершения работы с командами
    while (commandSender.isRunning()) {
        Sleep(100);
    }

    // 4. Останавливаем все
    std::cout << "\n3. Shutting down services..." << std::endl;
    std::cout << "   Stopping command controller..." << std::endl;
    commandSender.stop();

    std::cout << "   Stopping video stream..." << std::endl;
    videoStreamer.stop();

    std::cout << "\n==========================================" << std::endl;
    std::cout << "        All services stopped              " << std::endl;
    std::cout << "        Program completed                 " << std::endl;
    std::cout << "==========================================" << std::endl;

    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    return 0;
}