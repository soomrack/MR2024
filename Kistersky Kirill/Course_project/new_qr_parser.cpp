#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <chrono>
#include <sys/ioctl.h>
#include <thread>  // Добавлен этот заголовочный файл

using namespace cv;
using namespace std;
using namespace zbar;

const string ARDUINO_PORT = "/dev/ttyACM0";
const int QR_RESEND_DELAY_MS = 10000;
const int RECONNECT_DELAY_MS = 2000;

int arduino_fd = -1;
string last_sent_qr;
chrono::steady_clock::time_point last_send_time;
bool arduino_ready = false;

bool is_arduino_connected(int fd) {
    if (fd < 0) return false;
    int status;
    return (ioctl(fd, TIOCMGET, &status) != -1);
}

int setup_serial() {
    int fd = open(ARDUINO_PORT.c_str(), O_WRONLY | O_NOCTTY);
    if (fd < 0) return -1;

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B9600);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;
    tcsetattr(fd, TCSANOW, &tty);

    // Исправленная строка с использованием std::
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return fd;
}

void send_command(const string& cmd) {
    if (!arduino_ready) return;

    int bytes_written = write(arduino_fd, (cmd + "\n").c_str(), cmd.size() + 1);
    if (bytes_written < 0) {
        cerr << "Write error, trying to reconnect..." << endl;
        arduino_ready = false;
        close(arduino_fd);
        arduino_fd = -1;
    } else {
        fsync(arduino_fd);
    }
}

bool should_send_qr(const string& qr_data) {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - last_send_time).count();
    
    if (qr_data != last_sent_qr || elapsed >= QR_RESEND_DELAY_MS) {
        last_sent_qr = qr_data;
        last_send_time = now;
        return true;
    }
    return false;
}

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "ERROR: Camera not found!" << endl;
        return 1;
    }
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    cout << "Initializing..." << endl;

    arduino_fd = setup_serial();
    if (arduino_fd >= 0) {
        arduino_ready = true;
        cout << "Arduino connected successfully!" << endl;
    } else {
        cerr << "Failed to connect to Arduino" << endl;
    }

    Mat frame, gray;
    while (true) {
        static auto last_check = chrono::steady_clock::now();
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(now - last_check).count() >= RECONNECT_DELAY_MS) {
            if (!arduino_ready) {
                arduino_fd = setup_serial();
                if (arduino_fd >= 0) {
                    arduino_ready = true;
                    cout << "Arduino reconnected!" << endl;
                }
            }
            last_check = now;
        }

        cap >> frame;
        if (frame.empty()) continue;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Image image(gray.cols, gray.rows, "Y800", gray.data, gray.cols * gray.rows);
        
        if (scanner.scan(image) > 0) {
            for (auto symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
                string qr_data = symbol->get_data();
                cout << "QR detected: " << qr_data << endl;

                if (should_send_qr(qr_data) && arduino_ready) {
                    send_command(qr_data);
                    cout << "Sent to Arduino: " << qr_data << endl;
                }
            }
        }

        if (waitKey(1) == 27) break;
    }

    if (arduino_fd >= 0) close(arduino_fd);
    return 0;
}
