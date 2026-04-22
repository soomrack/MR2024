#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <csignal>
#include <chrono>
#include <thread>
#include <cctype>
#include <iomanip>
#include <ctime>
#include <mutex>
#include <deque>
#include <vector>
#include <atomic>
#include <algorithm>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define TCP_PORT 8888
#define UDP_SENSOR_PORT 5601
#define INACTIVITY_MS 500
#define SEND_INTERVAL_MS 50
#define LOST_MAX_SEC 10

const std::string PC_IP = "192.168.31.3";

std::mutex log_mutex;
volatile sig_atomic_t running = true;
void signal_handler(int) { running = false; }

std::string get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(now);
    auto ns = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto us = ns.time_since_epoch().count() % 1000000;
    std::tm tm_s;
    localtime_r(&tt, &tm_s);
    std::ostringstream oss;
    oss << std::put_time(&tm_s, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(6) << us;
    return oss.str();
}

int openSerial(const char *port)
{
    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
        return -1;
    struct termios tty;
    tcgetattr(fd, &tty);
    cfmakeraw(&tty);
    cfsetspeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    tty.c_cflag |= CS8;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;
    tcsetattr(fd, TCSANOW, &tty);
    tcflush(fd, TCIOFLUSH);
    return fd;
}

void logDistanceThread(int serial_fd)
{
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in pc_addr{};
    pc_addr.sin_family = AF_INET;
    pc_addr.sin_port = htons(UDP_SENSOR_PORT);
    inet_pton(AF_INET, PC_IP.c_str(), &pc_addr.sin_addr);

    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm_s;
    localtime_r(&tt, &tm_s);
    std::ostringstream fname;
    fname << "distance_log_" << std::put_time(&tm_s, "%Y%m%d_%H%M%S") << ".csv";
    std::ofstream logFile(fname.str(), std::ios::app);
    if (logFile.is_open())
        logFile << "timestamp,distance_cm,event_type,event_message\n";

    char buf[512];
    std::string leftover;
    while (running)
    {
        ssize_t n = read(serial_fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = '\0';
            std::string data = leftover + std::string(buf, n);
            leftover.clear();
            size_t pos = 0;
            while (pos < data.size())
            {
                size_t dp = data.find("DIST:", pos);
                size_t sp = data.find("SAFETY STOP:", pos);
                size_t bp = data.find("BLOCKED:", pos);
                size_t cp = data.find("OBSTACLE CLEARED", pos);
                ssize_t np = -1;
                char et = ' ';
                if (dp != std::string::npos && (np == -1 || (ssize_t)dp < np))
                {
                    np = dp;
                    et = 'D';
                }
                if (sp != std::string::npos && (np == -1 || (ssize_t)sp < np))
                {
                    np = sp;
                    et = 'S';
                }
                if (bp != std::string::npos && (np == -1 || (ssize_t)bp < np))
                {
                    np = bp;
                    et = 'B';
                }
                if (cp != std::string::npos && (np == -1 || (ssize_t)cp < np))
                {
                    np = cp;
                    et = 'C';
                }
                if (np == -1)
                    break;

                std::string ts = get_timestamp();
                if (et == 'D')
                {
                    size_t end = data.find(":END", np);
                    if (end != std::string::npos)
                    {
                        try
                        {
                            float d = std::stof(data.substr(np + 5, end - np - 5));
                            if (logFile.is_open())
                            {
                                logFile << ts << "," << d << ",distance,\n";
                                logFile.flush();
                            }
                            std::string msg = "DISTANCE:" + std::to_string(d) + "cm\n";
                            sendto(udp_fd, msg.c_str(), msg.size(), 0,
                                   (struct sockaddr *)&pc_addr, sizeof(pc_addr));
                        }
                        catch (...)
                        {
                        }
                        pos = end + 4;
                    }
                    else
                    {
                        leftover = data.substr(np);
                        break;
                    }
                }
                else
                {
                    size_t end = data.find('\n', np);
                    if (end != std::string::npos)
                    {
                        std::string type = (et == 'S') ? "safety_stop" : (et == 'B') ? "blocked"
                                                                                     : "cleared";
                        std::string raw = data.substr(np, end - np);
                        if (logFile.is_open())
                        {
                            logFile << ts << ",," << type << "," << raw << "\n";
                            logFile.flush();
                        }
                        bool blocked = (et == 'S' || et == 'B');
                        std::string msg = blocked ? "DISTANCE:5.0cm BLOCKED\n" : "DISTANCE:20.0cm\n";
                        sendto(udp_fd, msg.c_str(), msg.size(), 0,
                               (struct sockaddr *)&pc_addr, sizeof(pc_addr));
                        pos = end + 1;
                    }
                    else
                    {
                        leftover = data.substr(np);
                        break;
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (logFile.is_open())
    {
        logFile << get_timestamp() << ",,session,STOPPED\n";
        logFile.close();
    }
    if (udp_fd >= 0)
        close(udp_fd);
}

struct BatchCmd
{
    char cmd;
    int ms;
};

std::vector<BatchCmd> parseBatch(const std::string &input)
{
    std::vector<BatchCmd> result;
    auto mapDir = [](const std::string &d) -> char
    {
        if (d == "forward" || d == "fwd" || d == "f")
            return 'W';
        if (d == "backward" || d == "back" || d == "bck" || d == "b")
            return 'S';
        if (d == "left" || d == "l")
            return 'A';
        if (d == "right" || d == "r")
            return 'D';
        if (d == "stop")
            return 'X';
        return 0;
    };
    std::istringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ';'))
    {
        auto colon = token.find(':');
        if (colon == std::string::npos)
            continue;
        std::string dir = token.substr(0, colon);
        std::string dur = token.substr(colon + 1);
        std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);
        std::transform(dur.begin(), dur.end(), dur.begin(), ::tolower);

        dir.erase(remove_if(dir.begin(), dir.end(), ::isspace), dir.end());
        dur.erase(remove_if(dur.begin(), dur.end(), ::isspace), dur.end());
        if (!dur.empty() && dur.back() == 's')
            dur.pop_back();
        char cmd = mapDir(dir);
        if (!cmd)
            continue;
        int secs = 1;
        try
        {
            secs = std::stoi(dur);
        }
        catch (...)
        {
        }
        secs = std::max(1, std::min(30, secs));
        result.push_back({cmd, secs * 1000});
    }
    return result;
}

int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    const char *PORT = "/dev/ttyACM0";
    int serial_fd = openSerial(PORT);
    if (serial_fd == -1)
    {
        std::cerr << "WARNING: Arduino not found on " << PORT << " - running without motors\n";
    }
    else
    {
        std::cout << "Serial opened: " << PORT << "\n";
    }

    std::thread logger(logDistanceThread, serial_fd);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(TCP_PORT);
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    std::cout << "Robot Control v4.0 | TCP:" << TCP_PORT
              << " | UDP sensor→" << PC_IP << ":" << UDP_SENSOR_PORT << "\n"
              << "Commands: W/S/A/D/X/L/Q | Batch: \"Forward:2s;Left:1s\"\n"
              << std::flush;

    struct TimedCmd
    {
        char cmd;
        std::chrono::milliseconds duration;
    };
    std::deque<TimedCmd> cmdHistory;
    std::mutex historyMutex;
    std::atomic<bool> lostMode{false};
    std::atomic<bool> batchRunning{false};

    auto invertCmd = [](char c) -> char
    {
        switch (c)
        {
        case 'W':
            return 'S';
        case 'S':
            return 'W';
        case 'A':
            return 'D';
        case 'D':
            return 'A';
        default:
            return 'X';
        }
    };

    char lastCommand = 'X';
    auto lastActivity = std::chrono::steady_clock::now();
    auto lastSend = std::chrono::steady_clock::now();
    auto cmdStart = std::chrono::steady_clock::now();
    int client_fd = -1;

    while (running)
    {
        if (client_fd == -1 && !lostMode)
        {
            struct sockaddr_in caddr{};
            socklen_t clen = sizeof(caddr);
            int fd = accept(server_fd, (struct sockaddr *)&caddr, &clen);
            if (fd >= 0)
            {
                client_fd = fd;
                fcntl(client_fd, F_SETFL, O_NONBLOCK);
                batchRunning = false;
                std::cout << "Connected: " << inet_ntoa(caddr.sin_addr) << "\n"
                          << std::flush;
                lastCommand = 'X';
                lastActivity = std::chrono::steady_clock::now();
                {
                    std::lock_guard<std::mutex> lk(historyMutex);
                    cmdHistory.clear();
                }
            }
        }

        if (client_fd >= 0 && !lostMode)
        {
            char buf[256];
            ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);

            if (n > 0)
            {
                buf[n] = '\0';

                if (n > 1)
                {
                    std::string line(buf, n);
                    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                        line.pop_back();
                    std::cout << "Batch command\"" << line << "\"\n"
                              << std::flush;
                    if (!batchRunning)
                    {
                        auto cmds = parseBatch(line);
                        if (!cmds.empty())
                        {
                            batchRunning = true;
                            std::thread([&, cmds]()
                                        {
                                for (auto &bc : cmds) {
                                    if (!batchRunning || !running) break;
                                    std::cout << "Batch commmand" << bc.cmd << " " << bc.ms << "ms\n" << std::flush;
                                    write(serial_fd, &bc.cmd, 1);
                                    int steps = bc.ms / 50;
                                    for (int i = 0; i < steps && batchRunning && running; i++) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                        write(serial_fd, &bc.cmd, 1);
                                    }
                                    write(serial_fd, "X", 1);
                                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                }
                                write(serial_fd, "X", 1);
                                batchRunning = false;
                                std::cout << "Batch command is done\n" << std::flush; })
                                .detach();
                        }
                    }
                }
                else
                {
                    char ch = std::toupper(buf[0]);
                    std::cout << "\rCmd: " << ch << "   " << std::flush;

                    if (ch == 'Q')
                    {
                        write(serial_fd, "X", 1);
                        break;
                    }

                    if (ch == 'L')
                    {
                        lostMode = true;
                        batchRunning = false;
                        write(serial_fd, "X", 1);
                        std::cout << "\nLost starts\n"
                                  << std::flush;
                        std::thread([&]()
                                    {
                            std::deque<TimedCmd> copy;
                            { std::lock_guard<std::mutex> lk(historyMutex); copy = cmdHistory; }
                            auto start = std::chrono::steady_clock::now();
                            for (auto it = copy.rbegin(); it != copy.rend(); ++it) {
                                if (!running) break;
                                if (std::chrono::steady_clock::now() - start >
                                    std::chrono::seconds(LOST_MAX_SEC)) break;
                                char inv = invertCmd(it->cmd);
                                if (inv == 'X') continue;
                                write(serial_fd, &inv, 1);
                                std::cout << "[LOST] " << inv << " " << it->duration.count() << "ms\n" << std::flush;
                                std::this_thread::sleep_for(it->duration);
                                write(serial_fd, "X", 1);
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            }
                            write(serial_fd, "X", 1);
                            { std::lock_guard<std::mutex> lk(historyMutex); cmdHistory.clear(); }
                            lostMode = false;
                            std::cout << "Lost is done\n" << std::flush; })
                            .detach();
                    }
                    else if (ch == 'X')
                    {
                        if (lastCommand != 'X')
                        {
                            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - cmdStart);
                            if (dur.count() > 50)
                            {
                                std::lock_guard<std::mutex> lk(historyMutex);
                                if (!cmdHistory.empty() && cmdHistory.back().cmd == lastCommand)
                                    cmdHistory.back().duration += dur;
                                else
                                    cmdHistory.push_back({lastCommand, dur});
                                if (cmdHistory.size() > 50)
                                    cmdHistory.pop_front();
                            }
                            write(serial_fd, "X", 1);
                            lastCommand = 'X';
                        }
                    }
                    else if (ch == 'W' || ch == 'S' || ch == 'A' || ch == 'D')
                    {
                        if (ch != lastCommand)
                        {
                            write(serial_fd, &ch, 1);
                            cmdStart = std::chrono::steady_clock::now();
                        }
                        lastCommand = ch;
                        lastActivity = std::chrono::steady_clock::now();
                        lastSend = std::chrono::steady_clock::now();
                    }
                }
            }
            else if (n == 0)
            {
                std::cout << "\nDisconnection — reversing\n"
                          << std::flush;
                close(client_fd);
                client_fd = -1;
                batchRunning = false;
                for (int i = 0; i < 20; i++)
                {
                    write(serial_fd, "S", 1);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                write(serial_fd, "X", 1);
                lastCommand = 'X';
            }
            else if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                close(client_fd);
                client_fd = -1;
                batchRunning = false;
                write(serial_fd, "X", 1);
                lastCommand = 'X';
            }
        }

        if (!lostMode && !batchRunning && lastCommand != 'X' &&
            std::chrono::steady_clock::now() - lastActivity >=
                std::chrono::milliseconds(INACTIVITY_MS))
        {
            write(serial_fd, "X", 1);
            lastCommand = 'X';
            std::cout << "\rCmd: TIMEOUT   " << std::flush;
        }

        if (!lostMode && !batchRunning && lastCommand != 'X' &&
            std::chrono::steady_clock::now() - lastSend >=
                std::chrono::milliseconds(SEND_INTERVAL_MS))
        {
            write(serial_fd, &lastCommand, 1);
            lastSend = std::chrono::steady_clock::now();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    running = false;
    write(serial_fd, "X", 1);
    if (client_fd >= 0)
        close(client_fd);
    close(server_fd);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (logger.joinable())
        logger.join();
    close(serial_fd);
    std::cout << "\nSession ended.\n";
    return 0;
}
