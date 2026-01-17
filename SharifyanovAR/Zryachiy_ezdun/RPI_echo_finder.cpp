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
#define SERIAL_PORT "/dev/ttyUSB0"
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
        std::vector<std::string> possible_ports = {
            "/dev/ttyUSB0",
            "/dev/ttyUSB1",
            "/dev/ttyACM0",
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
        
        struct termios tty;
        memset(&tty, 0, sizeof(tty));
        
        if (tcgetattr(serial_fd, &tty) != 0) {
            std::cerr << "Ошибка получения параметров порта: " << strerror(errno) << std::endl;
            close(serial_fd);
            connected = false;
            return false;
        }
        
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
        
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_cflag |= CREAD | CLOCAL;
        
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
        
        tty.c_oflag &= ~OPOST;
        tty.c_oflag &= ~ONLCR;
        
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 10;
        
        if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
            std::cerr << "Ошибка установки параметров порта: " << strerror(errno) << std::endl;
            close(serial_fd);
            connected = false;
            return false;
        }
        
        tcflush(serial_fd, TCIOFLUSH);
        
        usleep(2000000);
        
        std::cout << "Настройка порта завершена" << std::endl;
        connected = true;
        
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
                data_buffer.append(buffer, n);
                
                size_t pos;
                while ((pos = data_buffer.find('\n')) != std::string::npos) {
                    std::string line = data_buffer.substr(0, pos);
                    data_buffer.erase(0, pos + 1);
                    
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
            
            usleep(10000);
        }
        
        std::cout << "Поток чтения с Arduino завершен" << std::endl;
    }
    
    void processLine(const std::string& line) {
        std::cout << "Получено от Arduino: " << line << std::endl;
        
        {
            std::lock_guard<std::mutex> lock(data_mutex);
            
            // Сохраняем данные сенсора (добавлены DISTANCE: и WARN:)
            if (line.find("SENSOR:") == 0 || line.find("S:") == 0 || 
                line.find("CMD:") == 0 || line.find("READY") == 0 ||
                line.find("EXIT") == 0 || line.find("DISTANCE:") == 0 ||
                line.find("WARN:") == 0) {
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
            case 'w':
                std::cout << "Движение ВПЕРЕД" << std::endl;
                arduino.sendToArduino("w");
                break;
            case 's':
                std::cout << "Движение НАЗАД" << std::endl;
                arduino.sendToArduino("s");
                break;
            case 'a':
                std::cout << "Поворот ВЛЕВО" << std::endl;
                arduino.sendToArduino("a");
                break;
            case 'd':
                std::cout << "Поворот ВПРАВО" << std::endl;
                arduino.sendToArduino("d");
                break;
            case ' ':
                std::cout << "СТОП" << std::endl;
                arduino.sendToArduino(" ");
                break;
            case 'q':
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
            arduino.sendToArduino(" ");
        }
        arduino.disconnect();
        running = false;
    }
};

void handleClient(int clientSocket, RobotController& robot) {
    char buffer[BUFFER_SIZE];
    
    robot.setClientSocket(clientSocket);
    
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
        
        usleep(100000);
    }
    
    std::cout << "Завершение обработки клиента" << std::endl;
    close(clientSocket);
    robot.setClientSocket(-1);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        std::cerr << "Ошибка создания сокета: " << strerror(errno) << std::endl;
        return 1;
    }
    
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Ошибка установки SO_REUSEADDR/SO_REUSEPORT: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PC_PORT);
    
    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка привязки сокета: " << strerror(errno) << std::endl;
        std::cerr << "Порт " << PC_PORT << " занят. Подождите 10 секунд или убейте процесс." << std::endl;
        close(serverSocket);
        
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
    } else {
        std::cout << "Arduino успешно подключена" << std::endl;
    }
    
    std::cout << "Ожидание подключения клиента..." << std::endl;
    std::cout << "Для подключения используйте: telnet <ip_малины> " << PC_PORT << std::endl;
    
    while(robot.isRunning()) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(clientSocket < 0) {
            if (errno != EINTR) {
                std::cerr << "Ошибка принятия подключения: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        std::cout << "Клиент подключен! IP: " << inet_ntoa(clientAddr.sin_addr) 
                  << " Порт: " << ntohs(clientAddr.sin_port) << std::endl;
        
        handleClient(clientSocket, robot);
        
        std::cout << "Ожидание нового подключения..." << std::endl;
    }
    
    std::cout << "Сервер завершает работу..." << std::endl;
    close(serverSocket);
    
    return 0;
}
