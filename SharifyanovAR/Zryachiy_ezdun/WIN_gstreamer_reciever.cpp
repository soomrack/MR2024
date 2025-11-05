//     ###Это код для windows##


#include <iostream>
#include <windows.h>
#include <string>

int main() {
    std::cout << "Starting GStreamer pipeline on Windows..." << std::endl;

    // Путь к директории GStreamer
    std::string gstreamer_path = "C:\\gstreamer\\1.0\\mingw_x86\\bin";

    // Команда для выполнения
    std::string command =
        "gst-launch-1.0 udpsrc port=5600 ! "
        "application/x-rtp,encoding-name=JPEG,payload=26 ! "
        "rtpjpegdepay ! jpegdec ! autovideosink";

    // Создаем процесс
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Подготавливаем командную строку
    std::string full_command = "cmd.exe /c \"cd /d " + gstreamer_path + " && " + command + "\"";

    std::cout << "Executing: " << full_command << std::endl;

    // Запускаем процесс
    if (!CreateProcessA(
        NULL,                   // Имя приложения (используем командную строку)
        (LPSTR)full_command.c_str(), // Командная строка
        NULL,                   // Атрибуты безопасности процесса
        NULL,                   // Атрибуты безопасности потока
        FALSE,                  // Наследование дескрипторов
        0,                      // Флаги создания
        NULL,                   // Окружение
        NULL,                   // Текущая директория
        &si,                    // STARTUPINFO
        &pi))                   // PROCESS_INFORMATION
    {
        std::cerr << "ERROR: Failed to create process! Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "GStreamer started with PID: " << pi.dwProcessId << std::endl;
    std::cout << "Press Enter to stop the stream..." << std::endl;

    // Ждем нажатия Enter для завершения
    std::cin.get();

    // Завершаем процесс
    TerminateProcess(pi.hProcess, 0);

    // Закрываем дескрипторы
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "GStreamer stopped" << std::endl;

    return 0;
}
