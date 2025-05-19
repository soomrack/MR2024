
#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>

#include <opencv2/opencv.hpp>

#include <memory>
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <termios.h>

/*using namespace libcamera;
using namespace std;
using namespace cv;*/

std::shared_ptr<libcamera::Camera> gCamera;
libcamera::StreamConfiguration *gConfig;
std::atomic<bool> running{true};

const std::string ARDUINO_PORT = "/dev/ttyACM0";
int serial_fd = -1;

bool setup_serial() {
    serial_fd = open(ARDUINO_PORT.c_str(), O_WRONLY | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        perror("ERROR: Can't open Arduino port");
        return false;
    }

    struct termios tty;
    if (tcgetattr(serial_fd, &tty) != 0) {
        perror("Error from tcgetattr");
        close(serial_fd);
        return false;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK);
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);

    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        close(serial_fd);
        return false;
    }

    return true;
}

void send_to_arduino(float err_x, float err_y, float rad) {
    if (serial_fd < 0) return;

    char buffer[96];
    snprintf(buffer, sizeof(buffer), "X%.2f,Y%.2f,R%.2f\n", err_x, err_y,rad);
    //std::cout << "[SEND] " << buffer;
    write(serial_fd, buffer, strlen(buffer));
    fsync(serial_fd);
}

void *mmapBuffer(const libcamera::FrameBuffer::Plane &plane) {
    return mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);
}


void munmapBuffer(void *map, const libcamera::FrameBuffer::Plane &plane) {
    munmap(map, plane.length);
}

 cv::Mat getting_mask(cv::Mat* img)
 { 
    if (!img || img->empty()) return {};

    using namespace cv;

    Mat hsv, mask;

    cvtColor(*img, hsv, COLOR_BGR2HSV);
    //подставить значения перед проверкой
    Scalar lower(28, 76, 107);
    Scalar upper(54, 255, 255);
    inRange(hsv, lower, upper, mask);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    return mask;

 }


std::vector<cv::Point> getting_largest_countours(cv::Mat* mask){
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(*mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return{};
    auto largest_contour = *std::max_element(
            contours.begin(),
            contours.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                return cv::contourArea(a) < cv::contourArea(b);
            });
    return largest_contour;
}


std::vector<float> getting_center(std::vector<cv::Point> contour,cv::Mat* frame)
{
    cv::Point2f center;
    float radius;
    if (contour.empty()){
        return std::vector<float>{0.0f, 0.0f, 250.0f};
    }

    cv::minEnclosingCircle(contour, center, radius);
    
//изображение окружности нужно только на этапе проверки нужно передавать кадр 
    if (radius >= 20) {  // Отсеиваем мелкие объекты нужно оценить какие диаметры отсеиваем 
        cv::circle(*frame, center, (int)radius, cv::Scalar(0, 255, 0), 2);
        cv::circle(*frame, center, 3, cv::Scalar(0,0,255), -1);
    }

//можно проводить фильтрацию по радиусу, для возвращения центра
    if (radius < 20){
     return std::vector<float>{0.0f, 0.0f, 250.0f};
    }
    
    float x = center.x;
    float y = center.y;
    std::vector<float> coordinates = {x,y,radius};
    return coordinates;
}


void requestComplete(libcamera::Request *request) {

    const libcamera::FrameBuffer *buffer = request->buffers().begin()->second;
    void *data = mmapBuffer(buffer->planes()[0]);

    int width = gConfig->size.width;
    int height = gConfig->size.height;

    cv::Mat frame(height, width, CV_8UC3, data);
    cv::Mat rgb = frame.clone();
    cv::resize(rgb, rgb, cv::Size(640, 480)); // уменьшение

    // прописать возможные ошибки  
    
    cv::Mat mask =  getting_mask(& rgb);
    std::vector<cv::Point> contour =  getting_largest_countours(&mask);
    std::vector<float> coordinates = getting_center(contour, &rgb);
    // find error//
    float frame_center_x = 640 / 2.0f;
    float frame_center_y = 480 / 2.0f;

    float error_x = coordinates[0] - frame_center_x;
    float error_y = coordinates[1]- frame_center_y;
    float radius = coordinates[2];

    //std::cout << error_x << "," << error_y << "\n";
    send_to_arduino(error_x, error_y, radius);
    
    //cv::imshow("Camera", rgb);
    
    if (cv::waitKey(1) == 27)
        running = false;

    munmapBuffer(data, buffer->planes()[0]);
    request->reuse(libcamera::Request::ReuseBuffers);
    gCamera->queueRequest(request);
}


int main() {

    using namespace libcamera;

    if (!setup_serial()) return 1;

    CameraManager cm;
    cm.start();

    gCamera = cm.cameras()[0];
    gCamera->acquire();
    gCamera->requestCompleted.connect(requestComplete);

    auto config = gCamera->generateConfiguration({StreamRole::Viewfinder});
    config->at(0).pixelFormat = formats::RGB888;
    config->at(0).size.width = 1920;
    config->at(0).size.height = 1080;
    config->at(0).bufferCount = 4;
    
    gCamera->configure(config.get());
    gConfig = &config->at(0);

    //FrameBufferAllocator allocator(gCamera);
    //libcamera::Stream *stream = config->at(0).stream();

    /*поток один можно не ходить по циклу если что проверить работу в отдельной без обхода
    for (StreamConfiguration &cfg : *config)
        allocator.allocate(cfg.stream());
*/

/*
std::vector<std::unique_ptr<libcamera::Request>> requests;
for (const auto &buffer : allocator.buffers(stream)) {
    auto request = gCamera->createRequest();
    if (!request) {
        throw std::runtime_error("Failed to create request");
    }
    
    if (request->addBuffer(stream, buffer.get()) < 0) {
        throw std::runtime_error("Failed to add buffer");
    }
    
    requests.push_back(std::move(request));
} 
*/
    FrameBufferAllocator allocator(gCamera);
        for (StreamConfiguration &cfg : *config)
            allocator.allocate(cfg.stream());

        std::vector<std::unique_ptr<Request>> requests;
        for (StreamConfiguration &cfg : *config) {
            for (const auto &buffer : allocator.buffers(cfg.stream())) {
                auto request = gCamera->createRequest();
                request->addBuffer(cfg.stream(), buffer.get());
                requests.push_back(std::move(request));
            }
        }


    gCamera->start();
    for (auto &req : requests)
        gCamera->queueRequest(req.get());

    while (running)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    cv::destroyAllWindows();
    gCamera->stop();
    gCamera->release();
    cm.stop();
    
    if (serial_fd >= 0) close(serial_fd);
    return 0;
    
}
