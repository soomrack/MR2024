#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <string>

#include "config.h"
#include "video_streamer.h"
#include "command_sender.h"

PROCESS_INFORMATION g_dataReceiverProcess{};

void setupIP() {
    std::cout << "\n==========================================\n";
    std::cout << "      ROBOT CONNECTION SETUP\n";
    std::cout << "==========================================\n";
    std::cout << "Default robot IP: " << Config::RASPBERRY_IP << "\n";
    std::cout << "1 - Use default IP\n";
    std::cout << "2 - Enter different IP\n";
    std::cout << "Choose option (1/2): ";

    int option;
    std::cin >> option;

    if (option == 2) {
        std::cout << "Enter Raspberry Pi IP address: ";
        std::cin >> Config::RASPBERRY_IP;
        std::cout << "✓ Using IP: " << Config::RASPBERRY_IP << "\n";
    }
    else {
        std::cout << "✓ Using default IP: " << Config::RASPBERRY_IP << "\n";
    }

    std::cout << "==========================================\n\n";
}

bool startDataReceiverProcess() {

    // Получаем путь текущего exe
    wchar_t currentPath[MAX_PATH];
    GetModuleFileNameW(NULL, currentPath, MAX_PATH);

    std::wstring exeDir(currentPath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    // Формируем путь к data_receiver.exe в той же папке
    std::wstring exePath = exeDir + L"\\data_receiver.exe";

    // Передаем IP как аргумент
    std::wstring commandLine = L"\"" + exePath + L"\" " +
        std::wstring(Config::RASPBERRY_IP.begin(), Config::RASPBERRY_IP.end());

    STARTUPINFOW si{};
    si.cb = sizeof(si);

    ZeroMemory(&g_dataReceiverProcess, sizeof(g_dataReceiverProcess));

    BOOL success = CreateProcessW(
        NULL,
        &commandLine[0],
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        exeDir.c_str(),   // рабочая директория
        &si,
        &g_dataReceiverProcess
    );

    if (!success) {
        std::cerr << "Failed to start Data Receiver. Error: "
            << GetLastError() << "\n";
        return false;
    }

    std::cout << "✓ Data Receiver started successfully.\n";
    return true;
}

void stopDataReceiverProcess() {
    if (g_dataReceiverProcess.hProcess) {

        std::cout << "Stopping Data Receiver...\n";

        TerminateProcess(g_dataReceiverProcess.hProcess, 0);

        CloseHandle(g_dataReceiverProcess.hProcess);
        CloseHandle(g_dataReceiverProcess.hThread);

        ZeroMemory(&g_dataReceiverProcess, sizeof(g_dataReceiverProcess));
    }
}

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

    setupIP();
    showBanner();

    //Запуск второго проекта
    if (!startDataReceiverProcess()) {
        std::cerr << "Cannot continue without Data Receiver.\n";
        return 1;
    }

    VideoStreamer videoStreamer;
    CommandSender commandSender;

    std::cout << "\nInitializing components...\n";

    videoStreamer.initialize();
    commandSender.initialize();

    if (!commandSender.isConnected()) {
        std::cerr << "\nERROR: Cannot connect to robot!\n";
        stopDataReceiverProcess();
        return 1;
    }

    std::cout << "\nStarting services...\n";

    videoStreamer.start();
    Sleep(3000);

    commandSender.start();

    while (commandSender.isRunning()) {
        Sleep(100);
    }

    std::cout << "\nShutting down...\n";

    commandSender.stop();
    videoStreamer.stop();

    stopDataReceiverProcess();

    std::cout << "\n==========================================\n";
    std::cout << "        Program finished\n";
    std::cout << "==========================================\n";

    std::cout << "\nPress Enter to exit...\n";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
