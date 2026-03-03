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
#include <vector>
#include <deque>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <functional>
#include <algorithm>  // Добавил для std::transform


#define COMMAND_PORT 8888
#define DATA_PORT_UDP 5601
#define BUFFER_SIZE 1024


// ===================== LOGGING CLASS =====================


class DataLogger {
private:
    std::ofstream cmdLogFile;
    std::ofstream sensorLogFile;
    std::mutex cmdMutex;
    std::mutex sensorMutex;
    std::string basePathCommands;
    std::string basePathSensors;


    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;


        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }


public:
    DataLogger() {
        // Логи на рабочий стол
        basePathCommands = "/home/rick/Desktop/robot_telemetry/commands/";
        basePathSensors = "/home/rick/Desktop/robot_telemetry/sensors/";
        std::filesystem::create_directories(basePathCommands);
        std::filesystem::create_directories(basePathSensors);


        std::string timestamp = getTimestamp();


        cmdLogFile.open(basePathCommands + "commands_" + timestamp + ".csv");
        sensorLogFile.open(basePathSensors + "sensors_" + timestamp + ".csv");


        if (cmdLogFile.is_open()) {
            cmdLogFile << "timestamp,command,duration_ms\n";
            cmdLogFile << getTimestamp() << ",PROGRAM_START,0\n";
            std::cout << "[LOGGER] Command log: " << basePathCommands + "commands_" + timestamp + ".csv" << std::endl;
        }


        if (sensorLogFile.is_open()) {
            sensorLogFile << "timestamp,distance_cm,blocked\n";
            std::cout << "[LOGGER] Sensor log: " << basePathSensors + "sensors_" + timestamp + ".csv" << std::endl;
        }
    }


    ~DataLogger() {
        if (cmdLogFile.is_open()) cmdLogFile.close();
        if (sensorLogFile.is_open()) sensorLogFile.close();
    }


    void logCommand(char cmd, long duration_ms) {
        std::lock_guard<std::mutex> lock(cmdMutex);
        if (cmdLogFile.is_open()) {
            cmdLogFile << getTimestamp() << "," 
                       << cmd << "," 
                       << duration_ms << "\n";
            cmdLogFile.flush();
        }
    }


    void logSensorData(float distance_cm, bool blocked) {
        std::lock_guard<std::mutex> lock(sensorMutex);
        if (sensorLogFile.is_open()) {
            sensorLogFile << getTimestamp() << "," 
                         << std::fixed << std::setprecision(2) << distance_cm << "," 
                         << (blocked ? "true" : "false") << "\n";
            sensorLogFile.flush();
        }
    }


    void logLostModeStart() {
        std::lock_guard<std::mutex> lock(cmdMutex);
        if (cmdLogFile.is_open()) {
            cmdLogFile << getTimestamp() << ",LOST_MODE_START,0\n";
            cmdLogFile.flush();
        }
    }


    void logLostModeEnd() {
        std::lock_guard<std::mutex> lock(cmdMutex);
        if (cmdLogFile.is_open()) {
            cmdLogFile << getTimestamp() << ",LOST_MODE_END,0\n";
            cmdLogFile.flush();
        }
    }


    void logEmergencyStop() {
        std::lock_guard<std::mutex> lock(cmdMutex);
        if (cmdLogFile.is_open()) {
            cmdLogFile << getTimestamp() << ",EMERGENCY_STOP,0\n";
            cmdLogFile.flush();
        }
    }

    // НОВОЕ: Логирование пакетной команды
    void logBatchCommand(const std::string& batchCmd) {
        std::lock_guard<std::mutex> lock(cmdMutex);
        if (cmdLogFile.is_open()) {
            cmdLogFile << getTimestamp() << ",BATCH:" << batchCmd << ",0\n";
            cmdLogFile.flush();
        }
    }
};


// ===================== UTILS =====================


// НОВОЕ: Функция для парсинга пакетных команд
struct BatchCommand {
    char cmd;
    int seconds;
};

std::vector<BatchCommand> parseBatchCommands(const std::string& input) {
    std::vector<BatchCommand> commands;
    
    std::string workStr = input;
    std::transform(workStr.begin(), workStr.end(), workStr.begin(), ::tolower);
    
    // Разбиваем по точке с запятой или запятой для множественных команд
    std::stringstream ss(workStr);
    std::string token;
    
    while (std::getline(ss, token, ';')) {
        // Удаляем пробелы
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        
        if (token.empty()) continue;
        
        size_t colonPos = token.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string direction = token.substr(0, colonPos);
        std::string durationStr = token.substr(colonPos + 1);
        
        // Парсим направление
        char cmd = 0;
        if (direction.find("forward") != std::string::npos || direction.find("w") != std::string::npos) {
            cmd = 'w';
        } else if (direction.find("back") != std::string::npos || direction.find("backward") != std::string::npos || direction.find("s") != std::string::npos) {
            cmd = 's';
        } else if (direction.find("left") != std::string::npos || direction.find("a") != std::string::npos) {
            cmd = 'a';
        } else if (direction.find("right") != std::string::npos || direction.find("d") != std::string::npos) {
            cmd = 'd';
        } else if (direction.find("stop") != std::string::npos) {
            cmd = ' ';
        }
        
        if (cmd == 0) continue;
        
        // Парсим длительность
        int seconds = 1; // по умолчанию 1 секунда
        try {
            seconds = std::stoi(durationStr);
            if (seconds < 1) seconds = 1;
            if (seconds > 30) seconds = 30; // лимит 30 секунд
        } catch (...) {
            seconds = 1;
        }
        
        commands.push_back({cmd, seconds});
    }
    
    return commands;
}


char invertCommand(char c) {
    switch (c) {
        case 'w': return 's';
        case 's': return 'w';
        case 'a': return 'd';
        case 'd': return 'a';
        case ' ': return ' ';
        default:  return 0;
    }
}


struct TimedCommand {
    char cmd;
    std::chrono::milliseconds duration;
};


// ================= SERIAL + UDP ==================


class SerialCommunicator {
private:
    int serial_fd{-1};
    int udp_socket{-1};
    sockaddr_in pc_addr{};
    std::thread read_thread;
    std::atomic<bool> running{false};


public:
    std::function<void(const std::string&)> onSensorData;


    bool connect(const std::string& pc_ip) {
        std::vector<std::string> ports = {"/dev/ttyUSB0","/dev/ttyACM0"};
        for (auto& p : ports) {
            serial_fd = open(p.c_str(), O_RDWR | O_NOCTTY);
            if (serial_fd >= 0) {
                std::cout << "[SERIAL] Arduino on " << p << std::endl;
                break;
            }
        }
        if (serial_fd < 0) {
            std::cerr << "[SERIAL] Arduino not found\n";
            return false;
        }


        termios tty{};
        tcgetattr(serial_fd, &tty);
        cfsetospeed(&tty, B115200);
        cfsetispeed(&tty, B115200);
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_cflag |= CREAD | CLOCAL;
        tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);
        tty.c_lflag &= ~(ICANON | ECHO | ISIG);
        tty.c_oflag &= ~OPOST;
        tcsetattr(serial_fd, TCSANOW, &tty);


        udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        pc_addr.sin_family = AF_INET;
        pc_addr.sin_port = htons(DATA_PORT_UDP);
        pc_addr.sin_addr.s_addr = inet_addr(pc_ip.c_str());


        running = true;
        read_thread = std::thread(&SerialCommunicator::readLoop, this);
        return true;
    }


    void sendToArduino(char c) {
        if (serial_fd < 0) return;
        char buf[2] = { c, '\n' };
        write(serial_fd, buf, 2);
        std::cout << "[CMD → ARDUINO] " << c << std::endl;
    }


    void readLoop() {
        char buf[256];
        std::string buffer;


        while (running) {
            int n = read(serial_fd, buf, sizeof(buf));
            if (n > 0) {
                buffer.append(buf, n);
                size_t pos;
                while ((pos = buffer.find('\n')) != std::string::npos) {
                    std::string line = buffer.substr(0, pos);
                    buffer.erase(0, pos + 1);
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    if (line.empty()) continue;


                    std::cout << "[SENSOR] " << line << std::endl;


                    // UDP
                    std::string out = line + "\n";
                    sendto(udp_socket, out.c_str(), out.size(), 0,
                           (sockaddr*)&pc_addr, sizeof(pc_addr));


                    // Callback для RobotController
                    if (onSensorData) {
                        onSensorData(line);
                    }
                }
            } else {
                usleep(10000); // 10 ms
            }
        }
    }


    void stop() {
        running = false;
        if (read_thread.joinable())
            read_thread.join();


        if (serial_fd >= 0) { close(serial_fd); serial_fd = -1; }
        if (udp_socket >= 0) { close(udp_socket); udp_socket = -1; }
    }
};


// ================= ROBOT LOGIC ===================


class RobotController {
private:
    SerialCommunicator arduino;
    std::deque<TimedCommand> log;
    std::mutex log_mutex;
    std::atomic<bool> lost{false};
    DataLogger dataLogger;


    std::chrono::steady_clock::time_point last_time;
    char last_cmd{' '};


    // Для логирования сенсоров
    std::thread sensorLogThread;
    std::atomic<bool> sensorLogging{true};
    std::chrono::milliseconds sensorLogInterval{100};
    std::string lastSensorData;
    std::mutex sensorDataMutex;

    // НОВОЕ: Флаг для блокировки пакетных команд (чтобы WASD не мешали)
    std::atomic<bool> batchMode{false};
    std::mutex batchMutex;


    void parseAndLogSensorData(const std::string& data) {
        float distance = 0.0;
        bool blocked = false;


        size_t distPos = data.find("DISTANCE:");
        if (distPos != std::string::npos) {
            std::string distStr = data.substr(distPos + 9);
            size_t cmPos = distStr.find("cm");
            if (cmPos != std::string::npos) {
                try {
                    distance = std::stof(distStr.substr(0, cmPos));
                    blocked = (data.find("BLOCKED") != std::string::npos);
                    dataLogger.logSensorData(distance, blocked);


                    if (blocked && distance < 20.0) {
                        dataLogger.logEmergencyStop();
                    }
                } catch (...) {
                    std::cerr << "[LOGGER] Failed to parse sensor data: " << data << std::endl;
                }
            }
        }
    }


    void sensorLoggingLoop() {
        while (sensorLogging) {
            {
                std::lock_guard<std::mutex> lock(sensorDataMutex);
                if (!lastSensorData.empty()) {
                    parseAndLogSensorData(lastSensorData);
                }
            }
            std::this_thread::sleep_for(sensorLogInterval);
        }
    }


    // НОВОЕ: Выполнение пакетной команды
    void executeBatchCommand(const std::vector<BatchCommand>& batch) {
        {
            std::lock_guard<std::mutex> lock(batchMutex);
            batchMode = true;
        }
        
        dataLogger.logBatchCommand("START_BATCH");
        
        for (const auto& bc : batch) {
            std::cout << "[BATCH] Executing: " << bc.cmd << " for " << bc.seconds << "s" << std::endl;
            
            // Отправляем команду
            arduino.sendToArduino(bc.cmd);
            
            // Ждем указанное время
            std::this_thread::sleep_for(std::chrono::seconds(bc.seconds));
            
            // Логируем
            dataLogger.logCommand(bc.cmd, bc.seconds * 1000);
        }
        
        // Стоп
        std::cout << "[BATCH] Batch complete, stopping..." << std::endl;
        arduino.sendToArduino(' ');
        dataLogger.logCommand(' ', 100);
        
        dataLogger.logBatchCommand("END_BATCH");
        
        {
            std::lock_guard<std::mutex> lock(batchMutex);
            batchMode = false;
        }
    }


public:
    RobotController(const std::string& pc_ip) : dataLogger() {
        arduino.onSensorData = [this](const std::string& data) {
            this->updateSensorData(data);
        };


        if (arduino.connect(pc_ip)) {
            last_time = std::chrono::steady_clock::now();
            sensorLogThread = std::thread(&RobotController::sensorLoggingLoop, this);
        } else {
            std::cerr << "[ROBOT] Failed to connect to Arduino" << std::endl;
        }
    }


    ~RobotController() {
        sensorLogging = false;
        if (sensorLogThread.joinable())
            sensorLogThread.join();
        arduino.stop();
    }


    void updateSensorData(const std::string& sensorData) {
        std::lock_guard<std::mutex> lock(sensorDataMutex);
        lastSensorData = sensorData;
    }


    // НОВОЕ: Обработка строковых пакетных команд
    void handleBatchCommand(const std::string& batchText) {
        if (lost) {
            std::cout << "[BATCH] Lost mode active, ignoring batch" << std::endl;
            return;
        }
        
        std::cout << "[BATCH] Received: " << batchText << std::endl;
        
        auto commands = parseBatchCommands(batchText);
        if (commands.empty()) {
            std::cout << "[BATCH] No valid commands parsed" << std::endl;
            return;
        }
        
        // Запускаем пакетное выполнение в отдельном потоке
        std::thread([this, commands]() {
            this->executeBatchCommand(commands);
        }).detach();
    }


    void handleCommand(char c) {
        // НОВОЕ: Пропускаем WASD команды во время пакетного режима
        bool isBatchActive = false;
        {
            std::lock_guard<std::mutex> lock(batchMutex);
            isBatchActive = batchMode;
        }
        
        if (isBatchActive) {
            return; // WASD игнорируются во время пакетного режима
        }
        
        if (lost) return;


        if (c == 'l') {
            dataLogger.logLostModeStart();
            startLostMode();
            return;
        }


        if (c != 'w' && c != 'a' && c != 's' && c != 'd' && c != ' ')
            return;


        auto now = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
        last_time = now;


        if (delta > std::chrono::milliseconds(500))
            delta = std::chrono::milliseconds(500);


        {
            std::lock_guard<std::mutex> lock(log_mutex);
            if (!log.empty() && last_cmd == c) {
                log.back().duration += delta;
            } else {
                log.push_back({c, delta});
                last_cmd = c;
                dataLogger.logCommand(c, delta.count());
            }
        }


        arduino.sendToArduino(c);
    }


    void startLostMode() {
        if (lost) return;
        lost = true;


        std::thread([this]() {
            std::cout << "\n[LOST MODE] START\n";


            std::deque<TimedCommand> copy;
            {
                std::lock_guard<std::mutex> lock(log_mutex);
                copy = log;
            }


            auto start = std::chrono::steady_clock::now();


            for (auto it = copy.rbegin(); it != copy.rend(); ++it) {
                if (std::chrono::steady_clock::now() - start > std::chrono::seconds(10))
                    break;


                char inv = invertCommand(it->cmd);
                if (!inv) continue;


                std::cout << "[LOST] " << inv
                          << " for " << it->duration.count() << " ms\n";


                arduino.sendToArduino(inv);
                std::this_thread::sleep_for(it->duration);


                dataLogger.logCommand(inv, it->duration.count());


                arduino.sendToArduino(' ');
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }


            arduino.sendToArduino(' ');


            {
                std::lock_guard<std::mutex> lock(log_mutex);
                log.clear();
            }


            dataLogger.logLostModeEnd();
            lost = false;
            last_cmd = ' ';
            last_time = std::chrono::steady_clock::now();


            std::cout << "[LOST MODE] END\n\n";
        }).detach();
    }
};


// ================= TCP SERVER ====================


// Добавь эту переменную в начало функции handleClient
std::string lineBuffer;  // Глобальный буфер для пакетных команд

void handleClient(int client, RobotController& robot) {
    char buf[BUFFER_SIZE];
    std::string lineBuffer;  // Локальный буфер для этого клиента
    
    while (true) {
        int n = recv(client, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        
        for (int i = 0; i < n; i++) {
            char c = buf[i];
            
            // WASD, space, l - ОДИНОЧНЫЕ команды (работают как раньше)
            if (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == ' ' || c == 'l') {
                // Если есть накопленный буфер - сначала пакетная команда
                if (!lineBuffer.empty()) {
                    std::cout << "[TCP] Batch: '" << lineBuffer << "'" << std::endl;
                    robot.handleBatchCommand(lineBuffer);
                    lineBuffer.clear();
                }
                // WASD сразу
                std::cout << "[TCP] WASD: '" << c << "'" << std::endl;
                robot.handleCommand(c);
            }
            // \n или \r - конец пакетной команды
            else if (c == '\n' || c == '\r') {
                if (!lineBuffer.empty()) {
                    std::cout << "[TCP] Batch: '" << lineBuffer << "'" << std::endl;
                    robot.handleBatchCommand(lineBuffer);
                    lineBuffer.clear();
                }
            }
            // Все остальное - для пакетных команд
            else {
                lineBuffer += c;
            }
        }
    }
    
    close(client);
    std::cout << "[CLIENT DISCONNECTED]\n";
}


// ================= MAIN WITH IP SELECTION ====================


std::string selectIpAddress() {
    std::string default_ip = "10.209.227.183";
    std::string choice;
    std::string ip_address;
    
    std::cout << "=====================================\n";
    std::cout << "     ВЫБОР IP АДРЕСА ДЛЯ UDP\n";
    std::cout << "=====================================\n";
    std::cout << "1. Использовать IP из кода: " << default_ip << "\n";
    std::cout << "2. Ввести IP адрес вручную\n";
    std::cout << "=====================================\n";
    std::cout << "Ваш выбор (1 или 2): ";
    
    std::getline(std::cin, choice);
    
    if (choice == "2") {
        std::cout << "Введите IP адрес для отправки данных: ";
        std::getline(std::cin, ip_address);
        
        // Простая проверка формата IP
        struct sockaddr_in sa;
        int result = inet_pton(AF_INET, ip_address.c_str(), &(sa.sin_addr));
        if (result != 1) {
            std::cout << "Неверный формат IP адреса. Используется IP по умолчанию: " << default_ip << "\n";
            ip_address = default_ip;
        }
    } else {
        ip_address = default_ip;
        std::cout << "Используется IP по умолчанию: " << ip_address << "\n";
    }
    
    std::cout << "=====================================\n\n";
    return ip_address;
}


int main() {
    // Выбор IP адреса
    std::string pc_ip = selectIpAddress();
    
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { std::cerr << "[SERVER] Failed to create socket\n"; return 1; }


    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(COMMAND_PORT);


    if (bind(server, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[SERVER] Bind failed\n"; close(server); return 1;
    }


    if (listen(server, 5) < 0) {
        std::cerr << "[SERVER] Listen failed\n"; close(server); return 1;
    }


    RobotController robot(pc_ip);


    std::cout << "=====================================\n";
    std::cout << "  ROBOT CONTROL SERVER (BATCH READY)\n";
    std::cout << "  TCP Port: " << COMMAND_PORT << "\n";
    std::cout << "  UDP Data Port: " << DATA_PORT_UDP << "\n";
    std::cout << "  UDP Destination IP: " << pc_ip << "\n";
    std::cout << "  Logs: /home/rick/Desktop/robot_telemetry\n";
    std::cout << "  WASD + 'l'(lost) + BATCH: \"Forward:2s;Left:1s\"\n";
    std::cout << "=====================================\n\n";


    while (true) {
        int client = accept(server, nullptr, nullptr);
        if (client >= 0) {
            std::cout << "[CLIENT CONNECTED]" << std::endl;
            std::thread(handleClient, client, std::ref(robot)).detach();
        } else {
            std::cerr << "[SERVER] Accept failed\n";
            usleep(1000000);
        }
    }


    close(server);
    return 0;
}
