#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <string>
#include <fstream>
#include <atomic>
#include <thread>
#include <unordered_map>

class VideoStreamer;

class CommandProcessor {
public:
    CommandProcessor(const std::string& uartDevice,
                     const std::string& logPath);
    ~CommandProcessor();

    bool isReady() const;
    
    // Основные методы управления
    void sendFullCommand(int direction, int speed, int servoCommand);
    void setDirection(int direction);
    void setServoCommand(int servoCommand);
    void sendCurrentCommand();
    
    // Обработка команд
    void processCommand(char cmd);
    void processSpeed(int speed);
    void processLine(const std::string& line);
    
    // Управление скриптами
    void interruptScript();
    void startScriptFromFile(const std::string& path);
    
    // Rollback и системные события
    void setRollbackMode(bool enabled);
    void logSystemEvent(const std::string& text);
    void setSpeedDirect(int speed);
    
    // Работа с сенсорами
    void startSensorLogging(const std::string& sensorLogPath);
    void stopSensorLogging();
    
    // Вспомогательные методы
    bool inverseCommand(char cmd, char& outInv) const;
    void setSensorSocket(int fd);
    
    // Новый метод для установки видеопотока
    void setVideoStreamer(VideoStreamer* vs);

private:
    // Состояние системы
    int currentDirection;
    int currentSpeed;
    int currentServoCommand;
    bool directionKeyPressed;
    int previousSpeed;
    int sensorSocketFd = -1;

    // Аппаратные интерфейсы
    int uart_fd;
    std::string uartDevicePath;
    std::string logFilePath;
    std::ofstream logFile;
    
    // Словари команд
    struct {
        std::unordered_map<char, std::string> forward;
        std::unordered_map<char, char> inverse;
    } dict;
    
    // Управление потоками
    std::thread scriptThread;
    std::atomic<bool> scriptRunning{false};
    std::atomic<bool> scriptInterrupt{false};
    std::atomic<bool> rollbackMode{false};
    
    std::thread sensorThread;
    std::atomic<bool> sensorRunning{false};
    std::ofstream sensorLog;

    VideoStreamer* videoStreamer;

    // Приватные методы
    void initUart();
    void openLog();
    void writeLog(const std::string& text);
    void scriptThreadFunc(const std::string& path);
    void sensorThreadFunc();
};

#endif
