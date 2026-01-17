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

#define COMMAND_PORT 8888
#define DATA_PORT_UDP 5601
#define BUFFER_SIZE 1024

// ===================== UTILS =====================

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
    bool connect() {
        std::vector<std::string> ports = {
            "/dev/ttyUSB0",
            "/dev/ttyACM0"
        };

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
        pc_addr.sin_addr.s_addr = inet_addr("10.133.231.183");

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

                    if (!line.empty() && line.back() == '\r')
                        line.pop_back();

                    if (line.empty()) continue;

                    std::cout << "[SENSOR] " << line << std::endl;

                    std::string out = line + "\n";
                    sendto(udp_socket, out.c_str(), out.size(), 0,
                           (sockaddr*)&pc_addr, sizeof(pc_addr));
                }
            }
        }
    }
};

// ================= ROBOT LOGIC ===================

class RobotController {
private:
    SerialCommunicator arduino;
    std::deque<TimedCommand> log;
    std::mutex log_mutex;
    std::atomic<bool> lost{false};

    std::chrono::steady_clock::time_point last_time;
    char last_cmd{' '};

public:
    RobotController() {
        arduino.connect();
        last_time = std::chrono::steady_clock::now();
    }

    void handleCommand(char c) {
        if (lost) return;

        // LOST trigger
        if (c == 'l') {
            startLostMode();
            return;
        }

        // filter allowed commands
        if (c != 'w' && c != 'a' && c != 's' && c != 'd' && c != ' ')
            return;

        auto now = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
        last_time = now;

        // clamp duration (IMPORTANT)
        if (delta > std::chrono::milliseconds(500))
            delta = std::chrono::milliseconds(500);

        {
            std::lock_guard<std::mutex> lock(log_mutex);
            if (!log.empty() && last_cmd == c) {
                log.back().duration += delta;
            } else {
                log.push_back({c, delta});
                last_cmd = c;
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
                if (std::chrono::steady_clock::now() - start >
                    std::chrono::seconds(10))
                    break;

                char inv = invertCommand(it->cmd);
                if (!inv) continue;

                std::cout << "[LOST] " << inv
                          << " for " << it->duration.count() << " ms\n";

                arduino.sendToArduino(inv);
                std::this_thread::sleep_for(it->duration);

                // IMPORTANT: STOP between commands
                arduino.sendToArduino(' ');
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            arduino.sendToArduino(' ');

            {
                std::lock_guard<std::mutex> lock(log_mutex);
                log.clear();
            }

            lost = false;
            last_cmd = ' ';
            last_time = std::chrono::steady_clock::now();

            std::cout << "[LOST MODE] END\n\n";
        }).detach();
    }
};

// ================= TCP ===========================

void handleClient(int client, RobotController& robot) {
    char buf[BUFFER_SIZE];
    while (true) {
        int n = recv(client, buf, sizeof(buf), 0);
        if (n <= 0) break;

        for (int i = 0; i < n; i++) {
            if (buf[i] != '\n' && buf[i] != '\r')
                robot.handleCommand(buf[i]);
        }
    }
    close(client);
    std::cout << "[CLIENT DISCONNECTED]\n";
}

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(COMMAND_PORT);

    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    RobotController robot;

    std::cout << "=== TCP 8888 | UDP 5601 READY ===\n";

    while (true) {
        int client = accept(server, nullptr, nullptr);
        if (client >= 0) {
            std::cout << "[CLIENT CONNECTED]\n";
            std::thread(handleClient, client, std::ref(robot)).detach();
        }
    }
}
