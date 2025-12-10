#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#define PORT 8888
#define BUFFER_SIZE 1024

class RobotController {
public:
    RobotController() : running(true) {
        // Инициализация моторов/сервоприводов робота
        std::cout << "Инициализация контроллера робота..." << std::endl;
    }
    
    ~RobotController() {
        stop();
    }
    
    void executeCommand(char command) {
        switch(command) {
            case 'w': // Вперед
                moveForward();
                break;
            case 's': // Назад
                moveBackward();
                break;
            case 'a': // Влево
                turnLeft();
                break;
            case 'd': // Вправо
                turnRight();
                break;
            case ' ': // Стоп
                stop();
                break;
            case 'q': // Выход
                running = false;
                stop();
                break;
            default:
                std::cout << "Неизвестная команда: " << command << std::endl;
        }
    }
    
    bool isRunning() const { return running; }
    
private:
    bool running;
    
    void moveForward() {
        std::cout << "Движение ВПЕРЕД" << std::endl;
        // Реализация управления моторами
        // Пример: setLeftMotorSpeed(100); setRightMotorSpeed(100);
    }
    
    void moveBackward() {
        std::cout << "Движение НАЗАД" << std::endl;
        // Реализация управления моторами
    }
    
    void turnLeft() {
        std::cout << "Поворот ВЛЕВО" << std::endl;
        // Реализация управления моторами
    }
    
    void turnRight() {
        std::cout << "Поворот ВПРАВО" << std::endl;
        // Реализация управления моторами
    }
    
    void stop() {
        std::cout << "СТОП" << std::endl;
        // Остановка всех моторов
        // Пример: setLeftMotorSpeed(0); setRightMotorSpeed(0);
    }
};

void handleClient(int clientSocket, RobotController& robot) {
    char buffer[BUFFER_SIZE];
    
    while(robot.isRunning()) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // Чтение команды от клиента
        int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
        if(bytesRead <= 0) {
            std::cout << "Клиент отключился или ошибка чтения" << std::endl;
            break;
        }
        
        if(buffer[0] != '\0') {
            robot.executeCommand(buffer[0]);
        }
    }
    
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return 1;
    }
    
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    // Привязка сокета
    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка привязки сокета" << std::endl;
        return 1;
    }
    
    // Прослушивание
    if(listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка прослушивания" << std::endl;
        return 1;
    }
    
    std::cout << "Сервер робота запущен на порту " << PORT << std::endl;
    std::cout << "Ожидание подключения клиента..." << std::endl;
    
    RobotController robot;
    
    // Принятие подключения
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if(clientSocket < 0) {
        std::cerr << "Ошибка принятия подключения" << std::endl;
        return 1;
    }
    
    std::cout << "Клиент подключен!" << std::endl;
    
    // Обработка клиента
    handleClient(clientSocket, robot);
    
    std::cout << "Сервер завершает работу..." << std::endl;
    close(serverSocket);
    
    return 0;
}
