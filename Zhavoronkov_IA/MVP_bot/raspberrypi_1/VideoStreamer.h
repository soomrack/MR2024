#pragma once

#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "UdpSocket.h"

class VideoStreamer {
public:
    VideoStreamer(uint16_t remote_port,
                  int camera_index,
                  const std::string& bufferPath);

    ~VideoStreamer();

    void start();
    void stop();

    void setSendingEnabled(bool enabled);

    void setOperator(const std::string& ip);
    void clearOperator();

    void increaseResolution();
    void decreaseResolution();
    std::string getCurrentResolution() const;

private:
    void loop();
    void initVideoBuffer();
    void saveFrameToDisk(const cv::Mat& frame);
    void applyResolution(); // Применение текущего разрешенния

    // Структура для хранения информации о разрешении
    struct Resolution {
        int width;
        int height;
        std::string name;
    };

    std::atomic<bool> running;
    std::atomic<bool> sendingEnabled;
    std::atomic<bool> hasOperator;

    std::thread worker;
    std::mutex cameraMutex;

    cv::VideoCapture cap;
    UdpSocket udp;

    uint16_t udpPort;
    uint32_t frame_id;

    std::string bufferPath;
    int cameraIndex;

    // Управление разрешением
    std::vector<Resolution> availableResolutions;
    size_t currentResolutionIndex;
};
