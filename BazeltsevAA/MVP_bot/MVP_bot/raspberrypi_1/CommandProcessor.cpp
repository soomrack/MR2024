#include "CommandProcessor.h"
#include "VideoStreamer.h" 

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sys/socket.h>

// Форматирования времени
static std::string getCurrentTimeMs() {
    auto now = std::chrono::system_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm_info, "%d.%m.%Y")
        << "]["
        << std::put_time(tm_info, "%H:%M:%S")
        << "."
        << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

CommandProcessor::CommandProcessor(const std::string& uartDevice,
                                   const std::string& logPath)
    : uart_fd(-1),
      uartDevicePath(uartDevice),
      logFilePath(logPath),
      currentDirection(0),
      currentSpeed(50),
      currentServoCommand(0),
      directionKeyPressed(false),
      previousSpeed(50),
      videoStreamer(nullptr)
{
    // Словарь команд
    dict.forward = {
        {'w', "FORWARD"},
        {'s', "BACKWARD"},
        {'a', "LEFT"},
        {'d', "RIGHT"},
        {'q', "TURN_LEFT"},
        {'e', "TURN_RIGHT"},
        {'n', "STOP"},
        {'i', "SERVO_UP"},
        {'k', "SERVO_DOWN"},
        {'j', "SERVO_LEFT"},
        {'l', "SERVO_RIGHT"},
        {'u', "SERVO_STOP"},
        {'+', "SPEED_INC"},
        {'-', "SPEED_DEC"},
        {'x', "RES_DOWN"},
        {'c', "RES_UP"}
    };
    
    // Словарь обратных команд
    dict.inverse = {
        {'w', 's'},
        {'s', 'w'},
        {'a', 'd'},
        {'d', 'a'},
        {'q', 'e'},
        {'e', 'q'},
        {'i', 'k'},
        {'k', 'i'},
        {'j', 'l'},
        {'l', 'j'}
    };
    
    initUart();
    openLog();
    
    std::cout << "[CMD] CommandProcessor initialized with 5-char format" << std::endl;
}

CommandProcessor::~CommandProcessor()
{
    if (uart_fd >= 0) {
        // Остановка перед выходом
        setDirection(0);
        setServoCommand(0);
        sendCurrentCommand();
        stopSensorLogging();
        close(uart_fd);
    }

    if (logFile.is_open())
        logFile.close();
    
    interruptScript();
}

bool CommandProcessor::isReady() const
{
    return uart_fd >= 0 && logFile.is_open();
}

void CommandProcessor::initUart()
{
    uart_fd = open(uartDevicePath.c_str(),
                   O_RDWR | O_NOCTTY | O_SYNC);

    if (uart_fd < 0) {
        std::cerr << "[CMD] Failed to open UART: " << uartDevicePath << std::endl;
        return;
    }

    termios tty{};
    if (tcgetattr(uart_fd, &tty) != 0) {
        std::cerr << "[CMD] Failed to get UART attributes" << std::endl;
        close(uart_fd);
        uart_fd = -1;
        return;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;      // Без четности
    tty.c_cflag &= ~CSTOPB;      // 1 стоп-бит
    tty.c_cflag &= ~CSIZE;       // Очистить размер
    tty.c_cflag |= CS8;          // 8 бит данных
    tty.c_cflag &= ~CRTSCTS;     // Без аппаратного управления потоком
    
    tty.c_lflag = 0;             // Неканонический режим
    tty.c_oflag = 0;             // Необработанный вывод

    tty.c_cc[VMIN]  = 0;         // Читать без ожидания
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        std::cerr << "[CMD] Failed to set UART attributes" << std::endl;
        close(uart_fd);
        uart_fd = -1;
        return;
    }

    std::cout << "[CMD] UART initialized on " << uartDevicePath << std::endl;
}

void CommandProcessor::openLog()
{
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "[CMD] Failed to open command log: " << logFilePath << std::endl;
    } else {
        std::cout << "[CMD] Log file opened: " << logFilePath << std::endl;
    }
}

void CommandProcessor::writeLog(const std::string& text)
{
    if (!logFile.is_open()) return;
    
    logFile << "[" << getCurrentTimeMs() << "] ";

    if (rollbackMode)
        logFile << "RB:";

    logFile << text << std::endl;
    logFile.flush(); // Немедленная запись на диск
}

void CommandProcessor::sendFullCommand(int direction, int speed, int servoCommand)
{
    if (uart_fd < 0) {
        std::cerr << "[UART] UART not initialized, cannot send command" << std::endl;
        return;
    }
    
    // Формирование строки с командой
    char command[6]; // 5 цифр + завершающий ноль
    
    // Направление (1 цифра) (0-6)
    direction = std::max(0, std::min(6, direction));
    command[0] = '0' + direction;
    
    // Скорость (3 цифры, от 010 до 100)
    int spd = speed;
    if (spd < 10) spd = 10;
    if (spd > 100) spd = 100;
    
    if (spd >= 100) {
        command[1] = '1';
        command[2] = '0';
        command[3] = '0';
    } else if (spd >= 10) {
        command[1] = '0';
        command[2] = '0' + (spd / 10);
        command[3] = '0' + (spd % 10);
    } else {
        command[1] = '0';
        command[2] = '1';
        command[3] = '0';
    }
    
    // Команда сервопривода (1 цифра, 0-4)
    servoCommand = std::max(0, std::min(4, servoCommand));
    command[4] = '0' + servoCommand;
    command[5] = '\0';
    
    // Отправка команды
    ssize_t written = write(uart_fd, command, 5);
    
    if (written != 5) {
        std::cerr << "[UART] Failed to write command to UART, written: " << written << std::endl;
    } else {
        std::cout << "[UART] TX: " << command 
                  << " (dir: " << direction 
                  << ", spd: " << spd 
                  << "%, servo: " << servoCommand << ")" << std::endl;
    }
}

void CommandProcessor::setDirection(int direction)
{
    if (direction < 0 || direction > 6) {
        std::cerr << "[CMD] Invalid direction: " << direction << std::endl;
        return;
    }
    
    int oldDirection = currentDirection;
    currentDirection = direction;
    
    if (direction == 0) {
        directionKeyPressed = false;
    } else {
        directionKeyPressed = true;
    }
    
    // Логирование направления
    if (oldDirection != direction) {
        std::string dirNames[] = {"STOP", "FORWARD", "BACKWARD", "RIGHT", "LEFT", "TURN_RIGHT", "TURN_LEFT"};
        std::string dirName = (direction >= 0 && direction <= 6) ? dirNames[direction] : "UNKNOWN";
        writeLog("DIR:" + std::to_string(oldDirection) + "->" + std::to_string(direction) + "(" + dirName + ")");
    }
    
    sendCurrentCommand();
}

void CommandProcessor::setServoCommand(int servoCommand)
{
    if (servoCommand < 0 || servoCommand > 4) {
        std::cerr << "[CMD] Invalid servo command: " << servoCommand << std::endl;
        return;
    }
    
    int oldServo = currentServoCommand;
    currentServoCommand = servoCommand;
    
    // Логирование ск=ерво команд
    if (oldServo != servoCommand) {
        std::string servoNames[] = {"SERVO_STOP", "SERVO_RIGHT", "SERVO_UP", "SERVO_DOWN", "SERVO_LEFT"};
        std::string servoName = (servoCommand >= 0 && servoCommand <= 4) ? servoNames[servoCommand] : "UNKNOWN";
        writeLog("SERVO:" + std::to_string(oldServo) + "->" + std::to_string(servoCommand) + "(" + servoName + ")");
    }
    
    sendCurrentCommand();
}

void CommandProcessor::sendCurrentCommand()
{
    sendFullCommand(currentDirection, currentSpeed, currentServoCommand);
}

void CommandProcessor::processCommand(char cmd)
{
    // Обработка клавиш управления скоростью
    if (cmd == '+' || cmd == '=') {
        int newSpeed = currentSpeed + 10;
        if (newSpeed > 100) newSpeed = 100;
        processSpeed(newSpeed);
        return;
    }
    else if (cmd == '-' || cmd == '_') {
        int newSpeed = currentSpeed - 10;
        if (newSpeed < 10) newSpeed = 10;
        processSpeed(newSpeed);
        return;
    }
    
    // Обработка команд разрешения видео
    if (cmd == 'x') {
        if (videoStreamer) {
            videoStreamer->decreaseResolution();
            std::cout << "[CMD] Decreasing video resolution" << std::endl;
            writeLog("VIDEO:RES_DOWN");
        } else {
            std::cerr << "[CMD] VideoStreamer not set!" << std::endl;
        }
        return;
    }
    else if (cmd == 'c') {
        if (videoStreamer) {
            videoStreamer->increaseResolution();
            std::cout << "[CMD] Increasing video resolution" << std::endl;
            writeLog("VIDEO:RES_UP");
        } else {
            std::cerr << "[CMD] VideoStreamer not set!" << std::endl;
        }
        return;
    }
    
    if (!dict.forward.count(cmd)) {
        std::cerr << "[CMD] Unknown command: " << cmd << std::endl;
        return;
    }
    
    // Обновление состояния на основе команды
    switch (cmd) {
        case 'w': setDirection(1); break;    // Вперед
        case 's': setDirection(2); break;    // Назад
        case 'd': setDirection(3); break;    // Вправо
        case 'a': setDirection(4); break;    // Влево
        case 'q': setDirection(6); break;    // Поворот влево
        case 'e': setDirection(5); break;    // Поворот вправо
        case 'n': setDirection(0); break;    // Стоп
        case ' ': setDirection(0); break;    // Стоп
        case '0': setDirection(0); break;    // Стоп
        
        case 'i': setServoCommand(2); break; // Серво вверх
        case 'k': setServoCommand(3); break; // Серво вниз
        case 'j': setServoCommand(4); break; // Серво влево
        case 'l': setServoCommand(1); break; // Серво вправо
        case 'u': setServoCommand(0); break; // Серво стоп
        
        default:
            std::cerr << "[CMD] Unhandled command: " << cmd << std::endl;
            return;
    }
    
    std::cout << "[CMD] Executed: " << dict.forward[cmd] 
              << " (dir=" << currentDirection 
              << ", spd=" << currentSpeed 
              << ", servo=" << currentServoCommand << ")" << std::endl;
}


void CommandProcessor::processSpeed(int speed)
{
    if (speed < 0 || speed > 100) {
        std::cerr << "[CMD] Invalid speed: " << speed << std::endl;
        return;
    }

    if (speed == currentSpeed) {
        return;
    }

    previousSpeed = currentSpeed;
    currentSpeed = speed;

    // Логирование скорости
    writeLog("SPD:" + std::to_string(previousSpeed) + "->" + std::to_string(speed));
    std::cout << "[CMD] Speed set to " << speed << std::endl;

    if (directionKeyPressed) {
        sendCurrentCommand();
    }
}

void CommandProcessor::processLine(const std::string& line)
{
    std::cout << "[CMD] Processing line: " << line << std::endl;
    
    // Обработка команд скорости
    if (line.rfind("SPD:", 0) == 0) {
        try {
            int speed = std::stoi(line.substr(4));
            processSpeed(speed);
        } catch (const std::exception& e) {
            std::cerr << "[CMD] Invalid speed format: " << line << std::endl;
        }
        return;
    }
    
    // Обработка команд направления
    if (line.rfind("DIR:", 0) == 0) {
        try {
            int direction = std::stoi(line.substr(4));
            setDirection(direction);
        } catch (const std::exception& e) {
            std::cerr << "[CMD] Invalid direction format: " << line << std::endl;
        }
        return;
    }
    
    // Обработка команд сервопривода
    if (line.rfind("SERVO:", 0) == 0) {
        try {
            int servo = std::stoi(line.substr(6));
            setServoCommand(servo);
        } catch (const std::exception& e) {
            std::cerr << "[CMD] Invalid servo format: " << line << std::endl;
        }
        return;
    }

    // Обработка одиночных символьных команд
    if (line.size() == 1) {
        processCommand(line[0]);
        return;
    }
    
    // Обработка скриптовых команд
    if (line.rfind("SCRIPT:", 0) == 0) {
        std::string scriptContent = line.substr(7);
        std::string tempFile = "/tmp/robot_script_" + std::to_string(time(nullptr)) + ".txt";
        std::ofstream f(tempFile);
        if (f.is_open()) {
            f << scriptContent;
            f.close();
            startScriptFromFile(tempFile);
        }
        return;
    }

    std::cerr << "[CMD] Unknown line format: " << line << std::endl;
}

void CommandProcessor::setSpeedDirect(int speed)
{
    if (speed < 0 || speed > 100) {
        std::cerr << "[CMD] setSpeedDirect: Invalid speed: " << speed << std::endl;
        return;
    }

    int oldSpeed = currentSpeed;
    currentSpeed = speed;
    
    // Отправка команды скорости при движении
    if (directionKeyPressed) {
        sendCurrentCommand();
    }
    
    // Логирование режиме rollback
    if (rollbackMode) {
        writeLog("SPD:" + std::to_string(oldSpeed) + "->" + std::to_string(speed));
    }
    
    std::cout << "[CMD] Speed direct set to " << speed << std::endl;
}

void CommandProcessor::interruptScript()
{
    if (scriptRunning) {
        std::cout << "[CMD] Interrupting script..." << std::endl;
        scriptInterrupt = true;

        if (scriptThread.joinable()) {
            scriptThread.join();
            std::cout << "[CMD] Script thread joined" << std::endl;
        }

        scriptRunning = false;
        scriptInterrupt = false;
        
        // Останавка при прерывании
        setDirection(0);
    }
}

void CommandProcessor::scriptThreadFunc(const std::string& path)
{
    std::cout << "[CMD] Starting script from: " << path << std::endl;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[CMD] Failed to open script file: " << path << std::endl;
        scriptRunning = false;
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
            std::cout << "[CMD] Script line: " << line << std::endl;
        }
    }
    file.close();

    for (size_t i = 0; i < lines.size(); ++i) {
        if (scriptInterrupt) {
            std::cout << "[CMD] Script interrupted at line " << i << std::endl;
            break;
        }

        const std::string& l = lines[i];

        // Парсинг
        std::istringstream ss(l);
        std::string token;
        std::vector<std::string> parts;
        
        while (std::getline(ss, token, ':')) {
            parts.push_back(token);
        }
        
        if (parts.size() != 3) {
            std::cerr << "[CMD] Invalid script line format: " << l 
                      << " (expected cmd:speed:duration)" << std::endl;
            continue;
        }
        
        if (parts[0].length() != 1) {
            std::cerr << "[CMD] Invalid command in script: " << parts[0] << std::endl;
            continue;
        }
        
        char cmd = parts[0][0];
        int speed, duration_ms;
        
        try {
            speed = std::stoi(parts[1]);
            duration_ms = std::stoi(parts[2]);
        } catch (const std::exception& e) {
            std::cerr << "[CMD] Invalid number in script line: " << l << std::endl;
            continue;
        }
        
        // Установка скорости
        processSpeed(speed);
        
        processCommand(cmd);
        
        std::cout << "[CMD] Waiting " << duration_ms << " ms..." << std::endl;
        int slept = 0;
        while (slept < duration_ms && !scriptInterrupt) {
            usleep(10 * 1000); // 10 ms шаг
            slept += 10;
        }
        
        if (!scriptInterrupt) {
            // Останавка
            processCommand('n');
            usleep(50 * 1000); // 50ms
        }
    }

    // Удаление временного файл
    if (std::remove(path.c_str()) != 0) {
        std::cerr << "[CMD] Failed to remove temp script file: " << path << std::endl;
    }
    
    std::cout << "[CMD] Script finished" << std::endl;
    scriptRunning = false;
}

void CommandProcessor::startScriptFromFile(const std::string& path)
{
    std::cout << "[CMD] Starting script from file: " << path << std::endl;
    
    interruptScript();

    scriptRunning = true;
    scriptInterrupt = false;

    scriptThread = std::thread(
        &CommandProcessor::scriptThreadFunc,
        this,
        path
    );
    
    // Отделение потока
    scriptThread.detach();
}

bool CommandProcessor::inverseCommand(char cmd, char& outInv) const
{
    if (!dict.inverse.count(cmd))
        return false;

    outInv = dict.inverse.at(cmd);
    return true;
}

void CommandProcessor::setRollbackMode(bool enabled)
{
    rollbackMode = enabled;
    std::cout << "[CMD] Rollback mode: " << (enabled ? "ON" : "OFF") << std::endl;
}

void CommandProcessor::logSystemEvent(const std::string& text)
{
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTimeMs() << "] SYS:" << text << std::endl;
        logFile.flush();
    }
    std::cout << "[SYS] " << text << std::endl;
}

void CommandProcessor::setSensorSocket(int fd)
{
    sensorSocketFd = fd;
}

void CommandProcessor::startSensorLogging(const std::string& sensorLogPath) {
    if (sensorRunning) return;
    if (uart_fd < 0) return;

    sensorLog.open(sensorLogPath, std::ios::app);
    if (!sensorLog.is_open()) return;

    sensorRunning = true;
    sensorThread = std::thread(&CommandProcessor::sensorThreadFunc, this);
    sensorThread.detach();
}

void CommandProcessor::stopSensorLogging() {
    if (!sensorRunning) return;
    sensorRunning = false;

    // Поток detached
    if (sensorLog.is_open()) {
        sensorLog.flush();
        sensorLog.close();
    }
}

void CommandProcessor::setVideoStreamer(VideoStreamer* vs)
{
    videoStreamer = vs;
    std::cout << "[CMD] VideoStreamer set" << std::endl;
}

void CommandProcessor::sensorThreadFunc() {
    std::string rx;
    char buf[128];

    while (sensorRunning) {
        ssize_t n = read(uart_fd, buf, sizeof(buf));
        if (n > 0) {
            rx.append(buf, buf + n);

            while (true) {
                size_t pos = rx.find('\n');
                if (pos == std::string::npos) break;

                std::string line = rx.substr(0, pos);
                rx.erase(0, pos + 1);

                if (!line.empty() && line.back() == '\r')
                    line.pop_back();

                // 7 цифр: 4 флага о препятствиях + угол(000-359)
                bool ok = (line.size() == 7);
                if (ok) {
                    for (char c : line) {
                        if (c < '0' || c > '9') { ok = false; break; }
                    }
                }

                if (ok) {
                    // Локальный лог
                    if (sensorLog.is_open()) {
                        sensorLog << "[" << getCurrentTimeMs() << "] " << line << "\n";
                        sensorLog.flush();
                    }

                    // Отправка оператору по TCP
                    if (sensorSocketFd >= 0) {
                        std::string out = line + "\n";
                        ssize_t sent = send(sensorSocketFd,
                                            out.c_str(),
                                            out.size(),
                                            MSG_NOSIGNAL);

                        if (sent <= 0) {
                            std::cerr << "[SNS] Failed to send sensor data\n";
                        }
                    }
                }
            }
        }

        usleep(1000);
    }
}
