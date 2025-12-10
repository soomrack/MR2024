#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>  // Для _kbhit() и _getch()
#include <thread>
#include <atomic>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define SERVER_IP "10.133.231.101" // Замените на IP адрес Raspberry Pi
#define BUFFER_SIZE 1024

class InputHandler {
public:
    InputHandler(SOCKET clientSocket)
        : clientSocket(clientSocket), running(true), lastCommand(' ') {}

    void start() {
        std::cout << "Управление роботом:" << std::endl;
        std::cout << "W - Вперед" << std::endl;
        std::cout << "S - Назад" << std::endl;
        std::cout << "A - Влево" << std::endl;
        std::cout << "D - Вправо" << std::endl;
        std::cout << "Пробел - Стоп" << std::endl;
        std::cout << "Q - Выход" << std::endl;
        std::cout << "\nНачинайте управление..." << std::endl;

        while (running) {
            if (_kbhit()) {  // Проверка нажатия клавиши
                char key = _getch();

                // Преобразование в нижний регистр
                key = tolower(key);

                // Отправка команды на сервер
                sendCommand(key);

                // Выход при нажатии 'q'
                if (key == 'q') {
                    running = false;
                }
            }
            else {
                // Автостоп при отпускании клавиши
                if (lastCommand != ' ' && lastCommand != 'q') {
                    sendCommand(' ');
                }
            }

            // Небольшая задержка для уменьшения нагрузки на CPU
            Sleep(50);
        }
    }

    void stop() {
        running = false;
    }

private:
    SOCKET clientSocket;
    std::atomic<bool> running;
    char lastCommand;

    void sendCommand(char command) {
        if (command != lastCommand) {
            lastCommand = command;

            // Отправка команды на сервер
            char buffer[2] = { command, '\0' };
            int result = send(clientSocket, buffer, strlen(buffer), 0);

            if (result == SOCKET_ERROR) {
                std::cerr << "Ошибка отправки команды" << std::endl;
                running = false;
            }
            else {
                // Вывод текущей команды
                switch (command) {
                case 'w': std::cout << "Команда: ВПЕРЕД" << std::endl; break;
                case 's': std::cout << "Команда: НАЗАД" << std::endl; break;
                case 'a': std::cout << "Команда: ВЛЕВО" << std::endl; break;
                case 'd': std::cout << "Команда: ВПРАВО" << std::endl; break;
                case ' ': std::cout << "Команда: СТОП" << std::endl; break;
                case 'q': std::cout << "Команда: ВЫХОД" << std::endl; break;
                }
            }
        }
    }
};

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock" << std::endl;
        return 1;
    }

    // Создание сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    std::cout << "Подключение к роботу..." << std::endl;

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка подключения к серверу" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Успешное подключение к роботу!" << std::endl;

    // Запуск обработчика ввода
    InputHandler inputHandler(clientSocket);
    inputHandler.start();

    std::cout << "Завершение работы клиента..." << std::endl;

    // Закрытие сокета и очистка Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
