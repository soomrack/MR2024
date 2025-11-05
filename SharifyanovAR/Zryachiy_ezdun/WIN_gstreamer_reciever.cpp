#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <conio.h>

std::atomic<bool> running(true);
std::string raspberry_ip = "10.249.15.101";

// Глобальный флаг для предотвращения накопления команд
std::atomic<bool> sending_command(false);

// Функция отправки команд через PowerShell с ОДНИМ постоянным соединением
void sendCommandToRaspberry(const std::string& command) {
    // Если уже отправляется команда - игнорируем новую (как в играх)
    if (sending_command.exchange(true)) {
        return;
    }

    std::string powerShellCmd =
        "powershell -Command \""
        "try {"
        "    $tcp = New-Object System.Net.Sockets.TcpClient('" + raspberry_ip + "', 8888); "
        "    $stream = $tcp.GetStream(); "
        "    $data = [System.Text.Encoding]::ASCII.GetBytes('" + command + "'); "
        "    $stream.Write($data, 0, $data.Length); "
        "    $stream.Flush(); "
        "    Start-Sleep -Milliseconds 50; "  // Даем время на отправку
        "    $tcp.Close();"
        "} catch {"
        "    exit 1"
        "}\"";

    // Запускаем в отдельном процессе
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(NULL, (LPSTR)powerShellCmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        // Ждем завершения процесса отправки (но не долго)
        WaitForSingleObject(pi.hProcess, 1000); // Максимум 1 секунда ожидания
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // Разрешаем следующую команду
    sending_command = false;
}

// Быстрая функция обработки ввода - как в видеоиграх
void commandInputHandler() {
    std::cout << "Command controls activated!" << std::endl;
    std::cout << "W - Forward, A - Left, S - Backward, D - Right" << std::endl;
    std::cout << "SPACE - Stop, Q - Quit" << std::endl;
    std::cout << "Press keys to control the robot..." << std::endl;

    // Очищаем буфер клавиатуры перед началом
    while (_kbhit()) _getch();

    while (running) {
        // Быстрая проверка клавиш без накопления
        if (_kbhit()) {
            char ch = _getch();

            // Игнорируем служебные коды
            if (ch != 0 && ch != 0xE0) {
                bool valid_command = true;
                std::string command_str;

                switch (ch) {
                case 'w': case 'W':
                    std::cout << ">>> FORWARD" << std::endl;
                    command_str = "w";
                    break;
                case 'a': case 'A':
                    std::cout << ">>> LEFT" << std::endl;
                    command_str = "a";
                    break;
                case 's': case 'S':
                    std::cout << ">>> BACKWARD" << std::endl;
                    command_str = "s";
                    break;
                case 'd': case 'D':
                    std::cout << ">>> RIGHT" << std::endl;
                    command_str = "d";
                    break;
                case ' ':
                    std::cout << ">>> STOP" << std::endl;
                    command_str = " ";
                    break;
                case 'q': case 'Q':
                    std::cout << ">>> QUIT" << std::endl;
                    command_str = "q";
                    running = false;
                    break;
                default:
                    valid_command = false;
                    break;
                }

                if (valid_command && !command_str.empty()) {
                    // Запускаем отправку команды в отдельном потоке
                    std::thread sendThread(sendCommandToRaspberry, command_str);
                    sendThread.detach(); // Отсоединяем поток

                    // Короткая пауза для предотвращения спама
                    Sleep(30);
                }
            }
            else {
                // Поглощаем служебные коды
                if (_kbhit()) _getch();
            }
        }

        // Очень короткая пауза для снижения нагрузки на CPU
        Sleep(5);
    }
}

// Функция для запуска GStreamer (без изменений)
void startGStreamer() {
    std::cout << "Starting GStreamer pipeline..." << std::endl;
    std::string gstreamer_path = "C:\\gstreamer\\1.0\\mingw_x86\\bin";
    std::string command = "gst-launch-1.0 udpsrc port=5600 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink";

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    std::string full_command = "cmd.exe /c \"cd /d " + gstreamer_path + " && " + command + "\"";

    if (CreateProcessA(NULL, (LPSTR)full_command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        std::cout << "GStreamer started with PID: " << pi.dwProcessId << std::endl;

        while (running) {
            DWORD exit_code;
            if (GetExitCodeProcess(pi.hProcess, &exit_code) && exit_code != STILL_ACTIVE) {
                break;
            }
            Sleep(100);
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    std::cout << "GStreamer stopped" << std::endl;
}

int main() {
    std::cout << "=== Robot Control Center ===" << std::endl;
    std::cout << "Connecting to Raspberry Pi at: " << raspberry_ip << std::endl;
    std::cout << "Using PowerShell for commands" << std::endl;

    // Запускаем потоки
    std::thread gstreamerThread(startGStreamer);
    Sleep(3000);
    commandInputHandler();

    if (gstreamerThread.joinable()) {
        gstreamerThread.join();
    }

    std::cout << "Program finished. Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}
