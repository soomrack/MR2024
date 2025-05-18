#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

using namespace cv;
using namespace std;
using namespace zbar;

const string ARDUINO_PORT = "/dev/ttyACM0";

int setup_serial() {
    int fd = open(ARDUINO_PORT.c_str(), O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        cerr << "ERROR: Can't open Arduino port" << endl;
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B9600);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tcsetattr(fd, TCSANOW, &tty);

    return fd;
}

void send_command(int fd, const string& cmd) {
    if (fd < 0) return;
    write(fd, (cmd + "\n").c_str(), cmd.size() + 1);
    fsync(fd);
}

int main() {
    // Инициализация камеры
    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CAP_PROP_FPS, 30);
    
    if (!cap.isOpened()) {
        cerr << "ERROR: Camera not found!" << endl;
        return 1;
    }

    // Инициализация ZBar
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    // Подключение к Arduino
    int arduino_fd = setup_serial();
    if (arduino_fd < 0) return 1;

    cout << "SYSTEM READY. Show QR codes to camera..." << endl;

    Mat frame, gray;
    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // Обработка изображения
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Image image(gray.cols, gray.rows, "Y800", gray.data, gray.cols * gray.rows);
        
        if (scanner.scan(image) > 0) {
            for (auto symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
                string qr_data = symbol->get_data();
                cout << "QR DETECTED: " << qr_data << endl;
                
                // Отправка команды
                send_command(arduino_fd, qr_data);
                
                // Визуализация
                vector<Point> poly;
                for(int i=0; i < symbol->get_location_size(); i++) {
                    poly.emplace_back(
                        symbol->get_location_x(i),
                        symbol->get_location_y(i)
                    );
                }
                polylines(frame, poly, true, Scalar(0,255,0), 2);
            }
        }

        if (waitKey(1) == 27) break;
    }

    close(arduino_fd);
    return 0;
}
