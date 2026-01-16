#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <errno.h>

#define PC_PORT 8888
#define BUFFER_SIZE 1024
#define SERIAL_PORT "/dev/ttyUSB0"  // Изменил на ttyUSB0 для CH340
#define SERIAL_BAUD 115200

class SerialCommunicator {
private:
    int serial_fd;
    std::atomic<bool> running;
    std::atomic<bool> connected;
    std::thread read_thread;
    std::mutex data_mutex;
    std::string last_sensor_data;
    int client_socket;
    
public:
    SerialCommunicator() : serial_fd(-1), running(false), connected(false), client_socket(-1) {}
    
    ~SerialCommunicator() {
        disconnect();
    }
    
    bool connect(const std::string& port = SERIAL_PORT, int baudrate = SERIAL_BAUD) {
        // Попробуем разные порты
        std::vector<std::string> possible_ports = {
            "/dev/ttyUSB0",  // Для CH340 (ваш адаптер)
            "/dev/ttyUSB1",
            "/dev/ttyACM0",  // Для оригинальных Arduino
            "/dev/ttyACM1"
        };
        
        std::string found_port;
        int found_fd = -1;
        
        for (const auto& p : possible_ports) {
            std::cout << "Попытка подключения к " << p << "..." << std::endl;
            int fd = open(p.c_str(), O_RDWR | O_NOCTTY);
            if (fd >= 0) {
                std::cout << "Найден порт: " << p << std::endl;
                found_port = p;
                found_fd = fd;
                break;
            }
        }
        
        if (found_fd < 0) {
            std::cerr << "Не удалось найти Arduino. Проверьте подключение." << std::endl;
            std::cerr << "Попробуйте: ls /dev/tty*" << std::endl;
            connected = false;
            return false;
        }
        
        serial_fd = found_fd;
        std::cout << "Подключено к " << found_port << std::endl;
        
        // Настройка порта
        struct termios tty;
        memset(&tty, 0, sizeof(tty));
        
        if (tcgetattr(serial_fd, &tty) != 0) {
            std::cerr << "Ошибка получения параметров порта: " << strerror(errno) << std::endl;
            close(serial_fd);
            connected = false;
            return false;
        }
        
        // Установка скорости
        speed_t speed;
        switch(baudrate) {
            case 9600: speed = B9600; break;
            case 19200: speed = B19200; break;
            case 38400: speed = B38400; break;
            case 57600: speed = B57600; break;
            case 115200: speed = B115200; break;
            default: speed = B115200; break;
        }
        
        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);
        
        // Настройка 8N1
        tty.c_cflag &= ~PARENB;     // Без паритета
        tty.c_cflag &= ~CSTOPB;     // 1 стоп-бит
        tty.c_cflag &= ~CSIZE;      // Очищаем биты размера
        tty.c_cflag |= CS8;         // 8 бит данных
        tty.c_cflag &= ~CRTSCTS;    // Без аппаратного управления потоком
        tty.c_cflag |= CREAD | CLOCAL; // Включаем чтение, игнорируем управляющие линии
        
        // Настройка ввода
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Без программного управления потоком
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Сырой ввод
        
        // Настройка вывода
        tty.c_oflag &= ~OPOST; // Сырой вывод
        tty.c_oflag &= ~ONLCR; // Не преобразовывать перевод строки
        
        // Настройка локальных флагов
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Неканонический режим
        
        // Настройка таймаутов
        tty.c_cc[VMIN] = 0;    // Не ждать минимум символов
        tty.c_cc[VTIME] = 10;  // Таймаут 1 секунда (10 * 0.1s)
        
        if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
            std::cerr << "Ошибка установки параметров порта: " << strerror(errno) << std::endl;
            close(serial_fd);
            connected = false;
            return false;
        }
        
        // Очистка буфера
        tcflush(serial_fd, TCIOFLUSH);
        
        // Даем Arduino время на перезагрузку
        usleep(2000000); // 2 секунды
        
        std::cout << "Настройка порта завершена" << std::endl;
        connected = true;
        
        // Запуск потока чтения
        running = true;
        read_thread = std::thread(&SerialCommunicator::readLoop, this);
        
        return true;
    }
    
    bool isConnected() const {
        return connected;
    }
    
    void setClientSocket(int socket) {
        std::lock_guard<std::mutex> lock(data_mutex);
        client_socket = socket;
    }
    
    void sendToArduino(const std::string& command) {
        if (serial_fd >= 0 && !command.empty()) {
            ssize_t written = write(serial_fd, command.c_str(), command.length());
            if (written > 0) {
                std::cout << "Отправлено на Arduino: '" << command << "'" << std::endl;
            } else {
                std::cerr << "Ошибка отправки на Arduino" << std::endl;
            }
        }
    }
    
    std::string getLastSensorData() {
        std::lock_guard<std::mutex> lock(data_mutex);
        return last_sensor_data;
    }
    
    void readLoop() {
        char buffer[256];
        std::string data_buffer;
        
        std::cout << "Поток чтения с Arduino запущен" << std::endl;
        
        while (running) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = read(serial_fd, buffer, sizeof(buffer) - 1);
            
            if (n > 0) {
                // std::cout << "Получено " << n << " байт с Arduino" << std::endl;
                data_buffer.append(buffer, n);
                
                // Ищем полные строки
                size_t pos;
                while ((pos = data_buffer.find('\n')) != std::string::npos) {
                    std::string line = data_buffer.substr(0, pos);
                    data_buffer.erase(0, pos + 1);
                    
                    // Удаляем возврат каретки, если есть
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    
                    if (!line.empty()) {
                        processLine(line);
                    }
                }
            } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Ошибка чтения с Arduino: " << strerror(errno) << std::endl;
                break;
            }
            
            usleep(10000); // 10ms
        }
        
        std::cout << "Поток чтения с Arduino завершен" << std::endl;
    }
    
    void processLine(const std::string& line) {
        std::cout << "Получено от Arduino: " << line << std::endl;
        
        {
            std::lock_guard<std::mutex> lock(data_mutex);
            
            // Сохраняем данные сенсора
            if (line.find("SENSOR:") == 0 || line.find("S:") == 0 || 
                line.find("CMD:") == 0 || line.find("READY") == 0 ||
                line.find("EXIT") == 0) {
                last_sensor_data = line;
            }
            
            // Отправляем данные на ПК, если есть подключение
            if (client_socket >= 0) {
                std::string data_to_send = line + "\n";
                ssize_t sent = send(client_socket, data_to_send.c_str(), data_to_send.length(), 0);
                if (sent <= 0) {
                    std::cerr << "Ошибка отправки данных на ПК" << std::endl;
                }
            }
        }
    }
    
    void disconnect() {
        running = false;
        connected = false;
        
        if (read_thread.joinable()) {
            read_thread.join();
        }
        
        if (serial_fd >= 0) {
            close(serial_fd);
            serial_fd = -1;
        }
    }
};

class RobotController {
private:
    SerialCommunicator arduino;
    std::atomic<bool> running;
    
public:
    RobotController() : running(true) {
        std::cout << "Инициализация контроллера робота..." << std::endl;
        
        // Подключение к Arduino
        if (!arduino.connect()) {
            std::cerr << "Не удалось подключиться к Arduino" << std::endl;
            running = false;
        } else {
            std::cout << "Arduino подключен успешно" << std::endl;
        }
    }
    
    ~RobotController() {
        stop();
    }
    
    void setClientSocket(int socket) {
        arduino.setClientSocket(socket);
    }
    
    void executeCommand(char command) {
        if (!arduino.isConnected()) {
            std::cerr << "Arduino не подключен!" << std::endl;
            return;
        }
        
        switch(command) {
            case 'w': // Вперед
                std::cout << "Движение ВПЕРЕД" << std::endl;
                arduino.sendToArduino("w");
                break;
            case 's': // Назад
                std::cout << "Движение НАЗАД" << std::endl;
                arduino.sendToArduino("s");
                break;
            case 'a': // Влево
                std::cout << "Поворот ВЛЕВО" << std::endl;
                arduino.sendToArduino("a");
                break;
            case 'd': // Вправо
                std::cout << "Поворот ВПРАВО" << std::endl;
                arduino.sendToArduino("d");
                break;
            case ' ': // Стоп
                std::cout << "СТОП" << std::endl;
                arduino.sendToArduino(" ");
                break;
            case 'q': // Выход
                std::cout << "ВЫХОД" << std::endl;
                arduino.sendToArduino("q");
                running = false;
                break;
            default:
                std::cout << "Неизвестная команда: " << command << std::endl;
        }
    }
    
    bool isRunning() const { 
        return running; 
    }
    
    bool isArduinoConnected() const {
        return arduino.isConnected();
    }
    
    void stop() {
        if (arduino.isConnected()) {
            arduino.sendToArduino(" "); // Отправляем команду остановки
        }
        arduino.disconnect();
        running = false;
    }
};

void handleClient(int clientSocket, RobotController& robot) {
    char buffer[BUFFER_SIZE];
    
    // Устанавливаем сокет для отправки данных с Arduino на ПК
    robot.setClientSocket(clientSocket);
    
    // Отправляем приветственное сообщение клиенту
    std::string welcome = "Подключено к Raspberry Pi. Ожидание команд...\n";
    send(clientSocket, welcome.c_str(), welcome.length(), 0);
    
    std::cout << "Начата обработка клиента" << std::endl;
    
    while(robot.isRunning()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(clientSocket, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(clientSocket + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity < 0) {
            std::cerr << "Ошибка select: " << strerror(errno) << std::endl;
            break;
        }
        
        if (activity > 0 && FD_ISSET(clientSocket, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            
            // Чтение команды от клиента
            int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if(bytesRead <= 0) {
                std::cout << "Клиент отключился или ошибка чтения: " << strerror(errno) << std::endl;
                break;
            }
            
            if(buffer[0] != '\0') {
                std::cout << "Получена команда от ПК: '" << buffer[0] << "'" << std::endl;
                robot.executeCommand(buffer[0]);
            }
        }
        
        // Проверяем состояние каждые 100ms
        usleep(100000);
    }
    
    std::cout << "Завершение обработки клиента" << std::endl;
    close(clientSocket);
    robot.setClientSocket(-1); // Сбрасываем сокет
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        std::cerr << "Ошибка создания сокета: " << strerror(errno) << std::endl;
        return 1;
    }
    
    // Настройка опций сокета для повторного использования адреса
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Ошибка установки SO_REUSEADDR/SO_REUSEPORT: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }
    
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PC_PORT);
    
    // Привязка сокета
    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка привязки сокета: " << strerror(errno) << std::endl;
        std::cerr << "Порт " << PC_PORT << " занят. Подождите 10 секунд или убейте процесс." << std::endl;
        close(serverSocket);
        
        // Подождем и попробуем еще раз
        sleep(10);
        
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Ошибка повторного создания сокета" << std::endl;
            return 1;
        }
        
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        
        if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Повторная привязка также не удалась" << std::endl;
            close(serverSocket);
            return 1;
        }
    }
    
    // Прослушивание
    if(listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка прослушивания: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }
    
    std::cout << "Сервер робота запущен на порту " << PC_PORT << std::endl;
    std::cout << "Подключение к Arduino..." << std::endl;
    
    RobotController robot;
    
    if (!robot.isArduinoConnected()) {
        std::cerr << "Предупреждение: Arduino не подключен, некоторые функции будут недоступны" << std::endl;
        std::cout << "Попробуйте выполнить команду: ls /dev/tty*" << std::endl;
        std::cout << "Убедитесь, что Arduino подключена и драйвер CH340 установлен" << std::endl;
    } else {
        std::cout << "Arduino успешно подключена" << std::endl;
    }
    
    std::cout << "Ожидание подключения клиента..." << std::endl;
    std::cout << "Для подключения используйте: telnet <ip_малины> " << PC_PORT << std::endl;
    
    while(robot.isRunning()) {
        // Принятие подключения
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(clientSocket < 0) {
            if (errno != EINTR) {
                std::cerr << "Ошибка принятия подключения: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        std::cout << "Клиент подключен! IP: " << inet_ntoa(clientAddr.sin_addr) 
                  << " Порт: " << ntohs(clientAddr.sin_port) << std::endl;
        
        // Обработка клиента
        handleClient(clientSocket, robot);
        
        std::cout << "Ожидание нового подключения..." << std::endl;
    }
    
    std::cout << "Сервер завершает работу..." << std::endl;
    close(serverSocket);
    
    return 0;
}
