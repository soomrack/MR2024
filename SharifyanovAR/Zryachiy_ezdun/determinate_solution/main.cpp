#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <iostream>

#include "config.h"
#include "video_streamer.h"
#include "command_sender.h"

void showBanner() {
    std::cout << "==========================================\n";
    std::cout << "        ROBOT CONTROL CENTER v1.0\n";
    std::cout << "==========================================\n";
    std::cout << "Robot IP:     " << Config::RASPBERRY_IP << "\n";
    std::cout << "Command port: " << Config::COMMAND_PORT << "\n";
    std::cout << "Video port:   " << Config::VIDEO_PORT << "\n";
    std::cout << "==========================================\n";
}

int main() {
    showBanner();

    VideoStreamer videoStreamer;
    CommandSender commandSender;

    std::cout << "\nInitializing components...\n";

    videoStreamer.initialize();
    commandSender.initialize();

    if (!commandSender.isConnected()) {
        std::cerr << "\nERROR: Cannot connect to robot!\n";
        std::cerr << "Check Raspberry Pi connection and IP address.\n";
        std::cout << "\nPress Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "\nStarting services...\n";

    std::cout << "1. Starting video stream...\n";
    videoStreamer.start();
    Sleep(3000);

    std::cout << "\n2. Starting command controller...\n";
    commandSender.start();

    while (commandSender.isRunning()) {
        Sleep(100);
    }

    std::cout << "\nShutting down...\n";

    commandSender.stop();
    videoStreamer.stop();

    std::cout << "\n==========================================\n";
    std::cout << "        Program finished\n";
    std::cout << "==========================================\n";

    std::cout << "\nPress Enter to exit...\n";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
