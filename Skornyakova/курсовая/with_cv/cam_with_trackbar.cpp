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

using namespace libcamera;
using namespace std;
using namespace cv;

shared_ptr<Camera> gCamera;
StreamConfiguration *gConfig;
atomic<bool> running{true};

// HSV threshold values
int hMin = 0, sMin = 0, vMin = 0;
int hMax = 179, sMax = 255, vMax = 255;

void *mmapBuffer(const FrameBuffer::Plane &plane) {
    return mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);
}

void munmapBuffer(void *map, const FrameBuffer::Plane &plane) {
    munmap(map, plane.length);
}

void createTrackbars() {
    namedWindow("HSV Controls", WINDOW_NORMAL);
    createTrackbar("Hue Min", "HSV Controls", &hMin, 179);
    createTrackbar("Hue Max", "HSV Controls", &hMax, 179);
    createTrackbar("Sat Min", "HSV Controls", &sMin, 255);
    createTrackbar("Sat Max", "HSV Controls", &sMax, 255);
    createTrackbar("Val Min", "HSV Controls", &vMin, 255);
    createTrackbar("Val Max", "HSV Controls", &vMax, 255);
}

void requestComplete(Request *request) {
    createTrackbars();
    const FrameBuffer *buffer = request->buffers().begin()->second;
    void *data = mmapBuffer(buffer->planes()[0]);

    int width = gConfig->size.width;
    int height = gConfig->size.height;

    Mat frame(height, width, CV_8UC3, data);
    Mat rgb = frame.clone();
    
    // Convert to HSV and apply threshold
    Mat hsv, mask;
    cvtColor(rgb, hsv, COLOR_BGR2HSV);
    
    Scalar lower(hMin, sMin, vMin);
    Scalar upper(hMax, sMax, vMax);
    inRange(hsv, lower, upper, mask);
    
    // Apply mask to original image
    Mat result;
    bitwise_and(rgb, rgb, result, mask);
    
    // Show original and processed images
    imshow("Camera", rgb);
    imshow("Mask", mask);
    imshow("Result", result);
    
    if (waitKey(1) == 27)
        running = false;

    munmapBuffer(data, buffer->planes()[0]);

    request->reuse(Request::ReuseBuffers);
    gCamera->queueRequest(request);
}

int main() {
    // Create trackbars window
    //createTrackbars();
    
    CameraManager cm;
    cm.start();

    gCamera = cm.cameras()[0];
    gCamera->acquire();
    gCamera->requestCompleted.connect(requestComplete);

    auto config = gCamera->generateConfiguration({StreamRole::Viewfinder});
    config->at(0).pixelFormat = formats::RGB888;
    config->at(0).size.width = 640;
    config->at(0).size.height = 480;
    config->at(0).bufferCount = 4;
    gCamera->configure(config.get());
    gConfig = &config->at(0);

    FrameBufferAllocator allocator(gCamera);
    for (StreamConfiguration &cfg : *config)
        allocator.allocate(cfg.stream());

    vector<unique_ptr<Request>> requests;
    for (StreamConfiguration &cfg : *config) {
        for (const auto &buffer : allocator.buffers(cfg.stream())) {
            auto request = gCamera->createRequest();
            request->addBuffer(cfg.stream(), buffer.get());
            requests.push_back(move(request));
        }
    }

    gCamera->start();
    for (auto &req : requests)
        gCamera->queueRequest(req.get());

    while (running)
        this_thread::sleep_for(chrono::milliseconds(10));

    gCamera->stop();
    gCamera->release();
    cm.stop();
    
    destroyAllWindows();
}
