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
#include <errno.h>

#define COMMAND_PORT 8888
#define DATA_PORT_UDP 5601
#define BUFFER_SIZE 1024
#define SERIAL_BAUD 115200

class SerialCommunicator {
private:
    int serial_fd;
    int udp_socket;

    std::atomic<bool> running;
    std::atomic<bool> connected;

    std::thread read_thread;
    std::mutex data_mutex;
    std::string last_sensor_data;

    sockaddr_in pc_addr{};
    sockaddr_in udp_bind_addr{};

public:
    SerialCommunicator()
        : serial_fd(-1), udp_socket(-1), running(false), connected(false) {}

    ~SerialCommunicator() {
        disconnect();
    }

    bool connect(int baudrate = SERIAL_BAUD) {
        std::vector<std::string> ports = {
            "/dev/ttyUSB0",
            "/dev/ttyUSB1",
            "/dev/ttyACM0",
            "/dev/ttyACM1"
        };

        for (auto &p : ports) {
            serial_fd = open(p.c_str(), O_RDWR | O_NOCTTY);
            if (serial_fd >= 0) {
                std::cout << "Connected to Arduino on " << p << std::endl;
                break;
            }
        }

        if (serial_fd < 0) {
            std::cerr << "Arduino not found" << std::endl;
            return false;
        }

        termios tty{};
        tcgetattr(serial_fd, &tty);

        cfsetospeed(&tty, B115200);
        cfsetispeed(&tty, B115200);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_cflag |= CREAD | CLOCAL;
        tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);

        tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        tty.c_oflag &= ~OPOST;

        tty.c_cc[VMIN]  = 0;
        tty.c_cc[VTIME] = 5;

        tcsetattr(serial_fd, TCSANOW, &tty);
        tcflush(serial_fd, TCIOFLUSH);

        // ===== UDP =====
        udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket < 0) {
            std::cerr << "UDP socket create error: " << strerror(errno) << std::endl;
            return false;
        }

        int reuse = 1;
        setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        udp_bind_addr.sin_family = AF_INET;
        udp_bind_addr.sin_addr.s_addr = INADDR_ANY;
        udp_bind_addr.sin_port = 0;

        bind(udp_socket, (sockaddr*)&udp_bind_addr, sizeof(udp_bind_addr));

        pc_addr.sin_family = AF_INET;
        pc_addr.sin_port = htons(DATA_PORT_UDP);
        pc_addr.sin_addr.s_addr = inet_addr("10.133.231.183");

        running = true;
        connected = true;
        read_thread = std::thread(&SerialCommunicator::readLoop, this);

        return true;
    }

    bool isConnected() const {
        return connected;
    }

    void sendToArduino(const std::string &cmd) {
        if (serial_fd >= 0) {
            write(serial_fd, cmd.c_str(), cmd.size());
        }
    }

    void readLoop() {
        char buf[256];
        std::string buffer;

        while (running) {
            ssize_t n = read(serial_fd, buf, sizeof(buf));

            if (n > 0) {
                buffer.append(buf, n);
                size_t pos;
                while ((pos = buffer.find('\n')) != std::string::npos) {
                    std::string line = buffer.substr(0, pos);
                    buffer.erase(0, pos + 1);
                    if (!line.empty() && line.back() == '\r')
                        line.pop_back();
                    processLine(line);
                }
            } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Serial read error: " << strerror(errno) << std::endl;
                break;
            }

            usleep(5000);
        }

        std::cout << "Serial read thread stopped" << std::endl;
    }

    void processLine(const std::string &line) {
        std::cout << "Arduino: " << line << std::endl;
        sendToPC(line);
    }

    void sendToPC(const std::string &msg) {
        std::string out = msg + "\n";
        ssize_t sent = sendto(
            udp_socket,
            out.c_str(),
            out.size(),
            0,
            (sockaddr*)&pc_addr,
            sizeof(pc_addr)
        );

        if (sent < 0) {
            std::cerr << "UDP send error: " << strerror(errno) << std::endl;
        }
    }

    void disconnect() {
        running = false;
        connected = false;

        if (read_thread.joinable())
            read_thread.join();

        if (serial_fd >= 0)
            close(serial_fd);

        if (udp_socket >= 0)
            close(udp_socket);
    }
};

class RobotController {
private:
    SerialCommunicator arduino;
    std::atomic<bool> running;

public:
    RobotController() : running(true) {
        if (!arduino.connect()) {
            running = false;
        }
    }

    bool isRunning() const {
        return running;
    }

    void executeCommand(char c) {
        switch (c) {
            case 'w': arduino.sendToArduino("w\n"); break;
            case 's': arduino.sendToArduino("s\n"); break;
            case 'a': arduino.sendToArduino("a\n"); break;
            case 'd': arduino.sendToArduino("d\n"); break;
            case ' ': arduino.sendToArduino(" \n"); break;
            case 'q':
                arduino.sendToArduino("q\n");
                running = false;
                break;
            default: break;
        }
    }
};

void handleCommandClient(int client, RobotController &robot) {
    char buf[BUFFER_SIZE];
    while (robot.isRunning()) {
        int n = recv(client, buf, sizeof(buf), 0);
        if (n <= 0) break;

        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n' || buf[i] == '\r') continue;
            robot.executeCommand(buf[i]);
        }
    }
    close(client);
}

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(COMMAND_PORT);

    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    std::cout << "TCP command server on port " << COMMAND_PORT << std::endl;
    std::cout << "UDP sensor sender on port " << DATA_PORT_UDP << std::endl;

    RobotController robot;

    while (robot.isRunning()) {
        int client = accept(server, nullptr, nullptr);
        if (client >= 0) {
            std::thread(handleCommandClient, client, std::ref(robot)).detach();
        }
    }

    close(server);
    return 0;
}
