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
#include <algorithm>
#include <signal.h>
#include <errno.h>
#include <regex>

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
                now.time_since_epoch())
              % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
  }

public:
  DataLogger() {
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

  void logBatchCommand(const std::string& batchCmd) {
    std::lock_guard<std::mutex> lock(cmdMutex);
    if (cmdLogFile.is_open()) {
      cmdLogFile << getTimestamp() << ",BATCH:" << batchCmd << ",0\n";
      cmdLogFile.flush();
    }
  }

  void logBufferClear() {
    std::lock_guard<std::mutex> lock(cmdMutex);
    if (cmdLogFile.is_open()) {
      cmdLogFile << getTimestamp() << ",BUFFER_CLEARED,0\n";
      cmdLogFile.flush();
    }
  }
  
  void logInvalidCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(cmdMutex);
    if (cmdLogFile.is_open()) {
      cmdLogFile << getTimestamp() << ",INVALID_COMMAND:" << command << ",0\n";
      cmdLogFile.flush();
    }
  }
};

// ===================== UTILS =====================

struct BatchCommand {
  char cmd;
  int seconds;
};

// НОВАЯ ФУНКЦИЯ: Проверка формата batch команды
bool isValidBatchFormat(const std::string& token) {
    // Проверяем наличие двоеточия
    size_t colonPos = token.find(':');
    if (colonPos == std::string::npos) return false;
    
    // Проверяем, что после двоеточия есть цифры и 's'
    std::string durationPart = token.substr(colonPos + 1);
    if (durationPart.empty()) return false;
    
    // Проверяем формат "Xs" или "X" (где X - число)
    bool hasS = (durationPart.back() == 's');
    std::string numberPart = hasS ? durationPart.substr(0, durationPart.length() - 1) : durationPart;
    
    // Проверяем, что все символы - цифры
    for (char c : numberPart) {
        if (!isdigit(c)) return false;
    }
    
    // Проверяем длину числа (не более 2 цифр)
    if (numberPart.length() > 2) return false;
    
    return true;
}

// НОВАЯ ФУНКЦИЯ: Проверка допустимых направлений
bool isValidDirection(const std::string& direction) {
    std::string lowerDir = direction;
    std::transform(lowerDir.begin(), lowerDir.end(), lowerDir.begin(), ::tolower);
    
    static const std::vector<std::string> validDirections = {
        "forward", "fwd", "fw", "f",
        "backward", "bck", "bk", "b",
        "left", "l",
        "right", "r",
        "stop"
    };
    
    return std::find(validDirections.begin(), validDirections.end(), lowerDir) != validDirections.end();
}

// НОВАЯ ФУНКЦИЯ: Проверка отдельных WASD команд
bool isValidWASDCommand(char c) {
    return (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == ' ' || c == 'l');
}

std::vector<BatchCommand> parseBatchCommands(const std::string& input) {
    std::vector<BatchCommand> commands;
    
    std::string workStr = input;
    
    workStr.erase(0, workStr.find_first_not_of(" \t\n\r"));
    workStr.erase(workStr.find_last_not_of(" \t\n\r") + 1);
    
    std::transform(workStr.begin(), workStr.end(), workStr.begin(), ::tolower);
    
    std::cout << "[BATCH PARSER] Parsing: '" << workStr << "'" << std::endl;
    
    // Проверка на явно некорректные символы
    if (workStr.length() > 0) {
        bool hasValidChars = true;
        for (char c : workStr) {
            if (!isalnum(c) && c != ':' && c != ';' && c != ' ') {
                std::cout << "[BATCH PARSER] Invalid character in command: '" << c << "'" << std::endl;
                return commands;  // Возвращаем пустой вектор
            }
        }
    }
    
    std::stringstream ss(workStr);
    std::string token;
    
    while (std::getline(ss, token, ';')) {
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        
        if (token.empty()) continue;
        
        std::cout << "[BATCH PARSER] Token: '" << token << "'" << std::endl;
        
        // НОВОЕ: Проверка формата
        if (!isValidBatchFormat(token)) {
            std::cout << "[BATCH PARSER] Invalid format, skipping" << std::endl;
            continue;
        }
        
        size_t colonPos = token.find(':');
        std::string direction = token.substr(0, colonPos);
        std::string durationStr = token.substr(colonPos + 1);
        
        if (!durationStr.empty() && durationStr.back() == 's') {
            durationStr.pop_back();
        }
        
        std::cout << "[BATCH PARSER] Direction: '" << direction << "', Duration: '" << durationStr << "'" << std::endl;
        
        // НОВОЕ: Проверка допустимого направления
        if (!isValidDirection(direction)) {
            std::cout << "[BATCH PARSER] Invalid direction: " << direction << std::endl;
            continue;
        }
        
        char cmd = 0;
        
        if (direction == "forward" || direction == "fwd" || direction == "fw" || direction == "f") {
            cmd = 'w';
            std::cout << "[BATCH PARSER] Matched FORWARD -> w" << std::endl;
        } else if (direction == "backward" || direction == "bck" || direction == "bk" || direction == "b") {
            cmd = 's';
            std::cout << "[BATCH PARSER] Matched BACKWARD -> s" << std::endl;
        } else if (direction == "left" || direction == "l") {
            cmd = 'a';
            std::cout << "[BATCH PARSER] Matched LEFT -> a" << std::endl;
        } else if (direction == "right" || direction == "r") {
            cmd = 'd';
            std::cout << "[BATCH PARSER] Matched RIGHT -> d" << std::endl;
        } else if (direction == "stop") {
            cmd = ' ';
            std::cout << "[BATCH PARSER] Matched STOP -> space" << std::endl;
        }
        
        if (cmd == 0) continue;
        
        int seconds = 1;
        try {
            seconds = std::stoi(durationStr);
            if (seconds < 1) seconds = 1;
            if (seconds > 30) seconds = 30;
        } catch (...) {
            std::cout << "[BATCH PARSER] Invalid duration, using default 1s" << std::endl;
            seconds = 1;
        }
        
        commands.push_back({ cmd, seconds });
        std::cout << "[BATCH PARSER] Added command: " << cmd << " for " << seconds << "s" << std::endl;
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
    default: return 0;
  }
}

struct TimedCommand {
  char cmd;
  std::chrono::milliseconds duration;
};

// ================= SERIAL + UDP ==================

class SerialCommunicator {
private:
  int serial_fd{ -1 };
  int udp_socket{ -1 };
  sockaddr_in pc_addr{};
  std::thread read_thread;
  std::atomic<bool> running{ false };

public:
  std::function<void(const std::string&)> onSensorData;

  bool connect(const std::string& pc_ip) {
    std::vector<std::string> ports = { "/dev/ttyUSB0", "/dev/ttyACM0" };
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

          std::string out = line + "\n";
          sendto(udp_socket, out.c_str(), out.size(), 0,
                 (sockaddr*)&pc_addr, sizeof(pc_addr));

          if (onSensorData) {
            onSensorData(line);
          }
        }
      } else {
        usleep(10000);
      }
    }
  }

  void stop() {
    running = false;
    if (read_thread.joinable())
      read_thread.join();

    if (serial_fd >= 0) {
      close(serial_fd);
      serial_fd = -1;
    }
    if (udp_socket >= 0) {
      close(udp_socket);
      udp_socket = -1;
    }
  }
};

// ================= ROBOT LOGIC ===================

class RobotController {
private:
  SerialCommunicator arduino;
  std::deque<TimedCommand> log;
  std::mutex log_mutex;
  std::atomic<bool> lost{ false };
  std::atomic<bool> clientConnected{ false };
  DataLogger dataLogger;

  std::chrono::steady_clock::time_point last_time;
  char last_cmd{ ' ' };

  std::thread sensorLogThread;
  std::atomic<bool> sensorLogging{ true };
  std::chrono::milliseconds sensorLogInterval{ 100 };
  std::string lastSensorData;
  std::mutex sensorDataMutex;

  std::atomic<bool> batchMode{ false };
  std::mutex batchMutex;
  std::atomic<bool> batchActive{ false };
  std::thread batchThread;

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

  void executeBatchCommand(const std::vector<BatchCommand>& batch) {
    {
        std::lock_guard<std::mutex> lock(batchMutex);
        if (batchMode) {
            std::cout << "[BATCH] Already running, canceling previous..." << std::endl;
            batchActive = false;
            if (batchThread.joinable()) {
                batchThread.join();
            }
        }
        batchMode = true;
        std::cout << "[BATCH] Mode ACTIVE" << std::endl;
    }
   
    batchActive = true;
    dataLogger.logBatchCommand("START_BATCH");

    for (const auto& bc : batch) {
        if (!batchActive || !clientConnected) {
            std::cout << "[BATCH] Interrupted\n";
            break;
        }
        
        std::cout << "[BATCH] Executing: " << bc.cmd << " for " << bc.seconds << "s" << std::endl;
        arduino.sendToArduino(bc.cmd);

        int steps = bc.seconds * 10;
        for (int i = 0; i < steps; ++i) {
            if (!batchActive || !clientConnected) {
                std::cout << "[BATCH] Interrupted during execution" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!batchActive || !clientConnected) break;

        std::cout << "[BATCH] Stopping after " << bc.seconds << "s" << std::endl;
        arduino.sendToArduino(' ');
        dataLogger.logCommand(bc.cmd, bc.seconds * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (batchActive && clientConnected) {
        std::cout << "[BATCH] Batch complete, sending FINAL STOP" << std::endl;
        arduino.sendToArduino(' ');
        dataLogger.logCommand(' ', 100);
    }
    
    dataLogger.logBatchCommand("END_BATCH");

    {
        std::lock_guard<std::mutex> lock(batchMutex);
        batchMode = false;
        std::cout << "[BATCH] Mode INACTIVE" << std::endl;
    }
    batchActive = false;
  }

  void stopCurrentBatch() {
    if (!batchActive) return;

    std::cout << "[BATCH] Stopping current batch..." << std::endl;
    batchActive = false;
    arduino.sendToArduino(' ');

    if (batchThread.joinable()) {
        batchThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(batchMutex);
        batchMode = false;
    }

    std::cout << "[BATCH] Fully stopped\n";
  }

  void clearCommandBuffer() {
    std::lock_guard<std::mutex> lock(log_mutex);
    log.clear();
    last_cmd = ' ';
    last_time = std::chrono::steady_clock::now();
    dataLogger.logBufferClear();
    std::cout << "[BUFFER] Command buffer cleared" << std::endl;
  }

public:
  RobotController(const std::string& pc_ip)
    : dataLogger() {
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
      
    stopCurrentBatch();
    arduino.stop();
  }

  void updateSensorData(const std::string& sensorData) {
    std::lock_guard<std::mutex> lock(sensorDataMutex);
    lastSensorData = sensorData;
  }

  void forceStop() {
    std::cout << "[ROBOT] Force stopping all motors" << std::endl;
    
    stopCurrentBatch();
    clearCommandBuffer();
    
    for (int i = 0; i < 3; i++) {
        arduino.sendToArduino(' ');
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    lost = false;
    last_cmd = ' ';
    last_time = std::chrono::steady_clock::now();
    
    std::cout << "[ROBOT] Force stop complete" << std::endl;
  }

  void setClientConnected(bool connected) {
    clientConnected = connected;
    if (!connected) {
        std::cout << "[CLIENT] Disconnected - forcing robot stop" << std::endl;
        forceStop();
    }
  }

  void handleBatchCommand(const std::string& batchText) {
    if (lost) return;

    if (!clientConnected) {
        std::cout << "[BATCH] No client connected, ignoring" << std::endl;
        return;
    }

    auto commands = parseBatchCommands(batchText);
    if (commands.empty()) {
        std::cout << "[BATCH] No valid commands found" << std::endl;
        dataLogger.logInvalidCommand(batchText);
        return;
    }

    if (batchThread.joinable()) {
        std::cout << "[BATCH] Stopping previous batch..." << std::endl;
        batchActive = false;
        batchThread.join();
    }

    batchThread = std::thread([this, commands]() {
        this->executeBatchCommand(commands);
    });
    
    std::cout << "[BATCH] Started in background" << std::endl;
  }

  void handleCommand(char c) {
    bool isBatchActive = false;
    {
      std::lock_guard<std::mutex> lock(batchMutex);
      isBatchActive = batchMode;
    }

    if (isBatchActive) {
      std::cout << "[CMD] Ignoring WASD command - batch mode active" << std::endl;
      return;
    }

    if (lost) return;

    if (!clientConnected) {
      std::cout << "[CMD] No client connected, ignoring: " << c << std::endl;
      return;
    }

    if (c == 'l') {
      dataLogger.logLostModeStart();
      startLostMode();
      return;
    }

    if (!isValidWASDCommand(c)) {
        std::cout << "[CMD] Invalid command: " << c << std::endl;
        dataLogger.logInvalidCommand(std::string(1, c));
        return;
    }

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
        log.push_back({ c, delta });
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

// Глобальный указатель для обработчика сигналов
RobotController* globalRobotPtr = nullptr;

void signalHandler(int signum) {
    std::cout << "\n[SIGNAL] Received signal " << signum << ", stopping robot..." << std::endl;
    if (globalRobotPtr) {
        globalRobotPtr->forceStop();
    }
    exit(signum);
}

// ================= TCP SERVER ====================

void setNonBlocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void handleClient(int client, RobotController& robot) {
  char buf[BUFFER_SIZE];
  std::string lineBuffer;
  bool inBatchMode = false;
  
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  robot.setClientConnected(true);
  std::cout << "[CLIENT] Connected, buffer initialized (fd=" << client << ")" << std::endl;

  while (true) {
    memset(buf, 0, sizeof(buf));
    
    int n = recv(client, buf, sizeof(buf) - 1, 0);
    
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        } else {
            std::cout << "[CLIENT] Socket error: " << strerror(errno) << std::endl;
            break;
        }
    } else if (n == 0) {
        std::cout << "[CLIENT] Connection closed by client" << std::endl;
        break;
    }

    std::cout << "[TCP] Received " << n << " bytes: '";
    for (int i = 0; i < n; i++) {
        if (buf[i] >= 32 && buf[i] <= 126) {
            std::cout << buf[i];
        } else {
            std::cout << "[" << (int)buf[i] << "]";
        }
    }
    std::cout << "'" << std::endl;

    for (int i = 0; i < n; i++) {
      char c = buf[i];
      
      if (c == '\n' || c == '\r') {
        if (!lineBuffer.empty()) {
          std::cout << "[TCP] Received complete batch: '" << lineBuffer << "'" << std::endl;
          
          // НОВОЕ: Проверка длины batch команды
          if (lineBuffer.length() > 50) {
              std::cout << "[TCP] Batch command too long, ignoring" << std::endl;
              lineBuffer.clear();
              inBatchMode = false;
              continue;
          }
          
          robot.handleBatchCommand(lineBuffer);
          lineBuffer.clear();
          inBatchMode = false;
        }
        continue;
      }
      
      // Проверяем, является ли символ командой WASD
      if (isValidWASDCommand(c) && !inBatchMode) {
        if (!lineBuffer.empty()) {
          std::cout << "[TCP] Warning: lineBuffer not empty when receiving WASD: '" << lineBuffer << "'" << std::endl;
          lineBuffer.clear();
        }
        std::cout << "[TCP] WASD command: '" << c << "'" << std::endl;
        robot.handleCommand(c);
      } else {
        // НОВОЕ: Проверка на недопустимые символы в batch режиме
        if (inBatchMode && !isalnum(c) && c != ':' && c != ';') {
            std::cout << "[TCP] Invalid character in batch mode: '" << c << "', clearing buffer" << std::endl;
            lineBuffer.clear();
            inBatchMode = false;
        } else {
            lineBuffer += c;
            inBatchMode = true;
            
            // НОВОЕ: Защита от слишком длинных команд
            if (lineBuffer.length() > 50) {
                std::cout << "[TCP] Batch buffer overflow, clearing" << std::endl;
                lineBuffer.clear();
                inBatchMode = false;
            }
        }
      }
    }
  }

  std::cout << "[CLIENT] Disconnected - cleaning up..." << std::endl;
  robot.setClientConnected(false);
  
  shutdown(client, SHUT_RDWR);
  close(client);
  
  std::cout << "[CLIENT] Connection closed (fd=" << client << ")" << std::endl;
}

// ================= MAIN ===================

std::string selectIpAddress() {
  std::string default_ip = "10.209.227.183";
  std::string choice;
  std::string ip_address;

  std::cout << "=====================================\n";
  std::cout << "     ВЫБОР IP АДРЕСА\n";
  std::cout << "=====================================\n";
  std::cout << "1. Использовать IP из кода: " << default_ip << "\n";
  std::cout << "2. Ввести IP адрес вручную\n";
  std::cout << "=====================================\n";
  std::cout << "Ваш выбор (1 или 2): ";

  std::getline(std::cin, choice);

  if (choice == "2") {
    std::cout << "Введите IP адрес для отправки данных: ";
    std::getline(std::cin, ip_address);

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
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  
  std::string pc_ip = selectIpAddress();

  int server = socket(AF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    std::cerr << "[SERVER] Failed to create socket\n";
    return 1;
  }

  int opt = 1;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(COMMAND_PORT);

  if (bind(server, (sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cerr << "[SERVER] Bind failed\n";
    close(server);
    return 1;
  }

  if (listen(server, 5) < 0) {
    std::cerr << "[SERVER] Listen failed\n";
    close(server);
    return 1;
  }

  RobotController robot(pc_ip);
  globalRobotPtr = &robot;

  std::cout << "=====================================\n";
  std::cout << "  ROBOT CONTROL SERVER (SECURE)\n";
  std::cout << "  TCP Port: " << COMMAND_PORT << "\n";
  std::cout << "  UDP Data Port: " << DATA_PORT_UDP << "\n";
  std::cout << "  UDP Destination IP: " << pc_ip << "\n";
  std::cout << "  Logs: /home/rick/Desktop/robot_telemetry\n";
  std::cout << "  Commands: WASD + 'l'(lost)\n";
  std::cout << "  Batch: \"Forward:2s;Left:1s\"\n";
  std::cout << "  Multiple commands supported!\n";
  std::cout << "  SECURE MODE: Invalid commands are ignored\n";
  std::cout << "=====================================\n\n";

  std::vector<std::thread> clientThreads;

  while (true) {
    int client = accept(server, nullptr, nullptr);
    
    if (client >= 0) {
      std::cout << "\n[CLIENT CONNECTED] New client connected (fd=" << client << ")" << std::endl;
      
      clientThreads.push_back(std::thread([client, &robot]() {
          handleClient(client, std::ref(robot));
      }));
      
      clientThreads.back().detach();
      
    } else {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::cerr << "[SERVER] Accept failed: " << strerror(errno) << std::endl;
      }
      usleep(100000);
    }
  }

  close(server);
  return 0;
}
