#include "TcpSocket.h"
#include "connection.h"
#include "Heartbeat.h"
#include "VideoStreamer.h"
#include "CommandProcessor.h"
#include "RollbackExecutor.h"
#include <filesystem>

#include <iostream>
#include <unistd.h>

#include <atomic>
#include <thread>

std::atomic<bool> operatorConnected{false};
std::atomic<bool> cancelRollback{false};

int main() {
    std::string baseDir = std::getenv("HOME") + std::string("/MVP_log");

    std::filesystem::create_directory(baseDir);
    std::filesystem::create_directory(baseDir + "/video_buffer");

    TcpSocket controlServer;
    TcpSocket commandServer;
    TcpSocket sensorServer;

    if (!commandServer.bindAndListen(7000)) {
        std::cerr << "[CMD] Failed to bind command server\n";
        return 1;
    }

    if (!sensorServer.bindAndListen(8000)) {
        std::cerr << "[SNS] Failed to bind sensor server\n";
        return 1;
    }

    VideoStreamer video(6000, 0, baseDir + "/video_buffer");
    video.start();

    CommandProcessor cmd(
        "/dev/ttyACM0",
        baseDir + "/commands.log"
    );
    
    cmd.setVideoStreamer(&video);
    
    cmd.startSensorLogging(baseDir + "/sensors.log");

    std::thread rollbackThread;

    while (true) {
        ConnectionInfo conn = wait_op_connection(controlServer, 5000, baseDir);

        operatorConnected = true;
        cancelRollback = true;   // отмена rollback

        if (rollbackThread.joinable())
            rollbackThread.join();

        // heartbeat
        Heartbeat hb(conn.socket_fd, 3);
        hb.start();
        std::cout << "[CNT] Heartbeat started\n";

        cmd.logSystemEvent("OPERATOR_CONNECTED");

        video.setOperator(conn.operator_ip);
        video.setSendingEnabled(true);

        std::cout << "[CMD] Waiting for command connection...\n";
        int commandFd = commandServer.acceptClient();

        std::cout << "[SNS] Waiting for sensor connection...\n";
        int sensorFd = sensorServer.acceptClient();

        if (sensorFd < 0) {
            std::cerr << "[SNS] Sensor accept failed\n";
            close(commandFd);
            close(conn.socket_fd);
            continue;
        }

        std::cout << "[SNS] Sensor client connected\n";

        cmd.setSensorSocket(sensorFd);

        if (commandFd < 0) {
            std::cerr << "[CMD] Command accept failed\n";
            close(conn.socket_fd);
            continue;
        }

        char buffer[32];

        // Состояние приёма файла
        bool receivingFile = false;
        size_t expectedFileSize = 0;
        std::string fileBuffer;
        std::string recvBuffer;

        // Работа с оператором
        while (hb.isOperatorConnected()) {
            ssize_t len = recv(commandFd, buffer, sizeof(buffer), MSG_DONTWAIT);

            if (len > 0) {
                recvBuffer.append(buffer, len);

                while (true) {
                    if (!receivingFile) {
                        size_t pos = recvBuffer.find('\n');
                        if (pos == std::string::npos)
                            break;

                        std::string line = recvBuffer.substr(0, pos);
                        recvBuffer.erase(0, pos + 1);

                        // Обработка команды FILE
                        if (line.rfind("FILE:", 0) == 0) {
                            expectedFileSize = std::stoul(line.substr(5));
                            fileBuffer.clear();
                            receivingFile = true;
                            std::cout << "[CMD] Receiving script, size = "
                                    << expectedFileSize << " bytes\n";
                        } else {
                            cmd.interruptScript();
                            cmd.processLine(line);
                        }
                    }
                    // Полцчение тела файла
                    else {
                        size_t need = expectedFileSize - fileBuffer.size();
                        size_t take = std::min(need, recvBuffer.size());

                        fileBuffer.append(recvBuffer.substr(0, take));
                        recvBuffer.erase(0, take);

                        if (fileBuffer.size() == expectedFileSize) {
                            std::cout << "[CMD] Script received\n";
                            std::string scriptPath = "/tmp/robot_script.txt";
                            std::ofstream f(scriptPath);
                            f << fileBuffer;
                            f.close();

                            cmd.startScriptFromFile(scriptPath);

                            receivingFile = false;
                        }

                        if (recvBuffer.empty())
                            break;
                    }
                }
            }

            usleep(1000);
        }

        // Потеря связи
        operatorConnected = false;
        cancelRollback = false;

        cmd.interruptScript();
        cmd.processCommand('n'); // остановка
        cmd.logSystemEvent("OPERATOR_LOST");

        video.setSendingEnabled(false);
        video.clearOperator();

        close(commandFd);
        hb.stop();
        close(conn.socket_fd);

        cmd.setSensorSocket(-1);
        close(sensorFd);

        std::cout << "[CNT] Operator lost\n";

        rollbackThread = std::thread([&] {
            auto lostTime = std::chrono::steady_clock::now();

            while (true) {
                usleep(1000 * 1000); // 1 сек

                if (operatorConnected.load()) {
                    std::cout << "[CNT] Operator reconnected, rollback cancelled\n";
                    return;
                }

                auto now = std::chrono::steady_clock::now();
                auto sec = std::chrono::duration_cast<std::chrono::seconds>(
                    now - lostTime).count();

                std::cout << "[CNT] Without operator " << sec << " sec\n";

                if (sec >= 10) {
                    std::cout << "[CNT] Starting rollback\n";

                    RollbackExecutor rb(
                        cmd,
                        baseDir + "/commands.log",
                        cancelRollback
                    );

                    rb.executeRollback();
                    return;
                }
            }
        });
    }
    
    return 0;
}
