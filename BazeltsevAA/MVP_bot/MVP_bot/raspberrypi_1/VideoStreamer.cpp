#include "VideoStreamer.h"

#include <vector>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

static constexpr int MAX_UDP_PAYLOAD = 1400;

struct ChunkHeader {
    uint32_t frame_id;
    uint16_t chunk_id;
    uint16_t chunk_count;
};

VideoStreamer::VideoStreamer(uint16_t remote_port,
                             int camera_index,
                             const std::string& bufferPath)
    : running(false),
      sendingEnabled(false),
      hasOperator(false),
      udpPort(remote_port),
      frame_id(0),
      bufferPath(bufferPath),
      cameraIndex(camera_index),
      currentResolutionIndex(0)
{
    // Список доступных разрешений
    availableResolutions = {
        {640, 480, "640x480"},
        {352, 288, "352x288"},
        {320, 240, "320x240"},
        {176, 144, "176x144"},
        {160, 120, "160x120"}
    };

    cap.open(camera_index);
    if (!cap.isOpened()) {
        std::cerr << "[VID] Camera open failed\n";
        return;
    }

    applyResolution();

    udp.bindSocket(0);

    initVideoBuffer();
}

VideoStreamer::~VideoStreamer() {
    stop();
}

void VideoStreamer::initVideoBuffer()
{
    try {
        if (fs::exists(bufferPath)) {
            fs::remove_all(bufferPath);
        }
        fs::create_directories(bufferPath);
        std::cout << "[VID] Video buffer initialized at "
                  << bufferPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[VID] Failed to init video buffer: "
                  << e.what() << std::endl;
    }
}

void VideoStreamer::applyResolution()
{
    std::lock_guard<std::mutex> lock(cameraMutex);
    
    if (!cap.isOpened()) {
        cap.open(cameraIndex);
        if (!cap.isOpened()) {
            std::cerr << "[VID] Failed to reopen camera\n";
            return;
        }
    }

    const Resolution& res = availableResolutions[currentResolutionIndex];
    
    // Устанавка разрешения
    cap.set(cv::CAP_PROP_FRAME_WIDTH, res.width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, res.height);
    
    int actualWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int actualHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    std::cout << "[VID] Resolution set to " << res.name 
              << " (actual: " << actualWidth << "x" << actualHeight << ")" << std::endl;
}

void VideoStreamer::increaseResolution()
{
    if (currentResolutionIndex > 0) {
        currentResolutionIndex--;
        applyResolution();
        std::cout << "[VID] Resolution increased to: " 
                  << availableResolutions[currentResolutionIndex].name << std::endl;
    } else {
        std::cout << "[VID] Already at maximum resolution" << std::endl;
    }
}

void VideoStreamer::decreaseResolution()
{
    if (currentResolutionIndex < availableResolutions.size() - 1) {
        currentResolutionIndex++;
        applyResolution();
        std::cout << "[VID] Resolution decreased to: " 
                  << availableResolutions[currentResolutionIndex].name << std::endl;
    } else {
        std::cout << "[VID] Already at minimum resolution" << std::endl;
    }
}

std::string VideoStreamer::getCurrentResolution() const
{
    if (currentResolutionIndex < availableResolutions.size()) {
        return availableResolutions[currentResolutionIndex].name;
    }
    return "Unknown";
}

void VideoStreamer::saveFrameToDisk(const cv::Mat& frame)
{
    std::ostringstream name;
    name << bufferPath << "/frame_"
         << std::setw(8) << std::setfill('0')
         << frame_id << ".jpg";

    cv::imwrite(name.str(), frame);
}

void VideoStreamer::start() {
    running = true;
    worker = std::thread(&VideoStreamer::loop, this);
}

void VideoStreamer::stop() {
    running = false;
    if (worker.joinable())
        worker.join();
}

void VideoStreamer::setOperator(const std::string& ip) {
    udp.setRemote(ip, udpPort);
    hasOperator = true;

    std::cout << "[VID] Video target set to "
              << ip << ":" << udpPort << std::endl;
}

void VideoStreamer::clearOperator() {
    hasOperator = false;
}

void VideoStreamer::setSendingEnabled(bool enabled) {
    sendingEnabled = enabled;
}

void VideoStreamer::loop()
{
    while (running) {
        cv::Mat frame;
        
        {
            std::lock_guard<std::mutex> lock(cameraMutex);
            cap >> frame;
        }

        if (frame.empty()) {
            usleep(10 * 1000);
            continue;
        }

        cv::Mat rotated_frame;
        cv::rotate(frame, rotated_frame, cv::ROTATE_90_CLOCKWISE);

        saveFrameToDisk(rotated_frame);

        if (sendingEnabled && hasOperator) {
            std::vector<uchar> encoded;
            std::vector<int> params = {
                cv::IMWRITE_JPEG_QUALITY, 60
            };

            cv::imencode(".jpg", rotated_frame, encoded, params);

            size_t total = encoded.size();
            uint16_t chunks =
                (total + MAX_UDP_PAYLOAD - 1) / MAX_UDP_PAYLOAD;

            for (uint16_t i = 0; i < chunks; ++i) {
                ChunkHeader hdr;
                hdr.frame_id = frame_id;
                hdr.chunk_id = i;
                hdr.chunk_count = chunks;

                size_t offset = i * MAX_UDP_PAYLOAD;
                size_t size =
                    std::min(MAX_UDP_PAYLOAD,
                             (int)(total - offset));

                std::vector<uint8_t> packet(sizeof(hdr) + size);
                memcpy(packet.data(), &hdr, sizeof(hdr));
                memcpy(packet.data() + sizeof(hdr),
                       encoded.data() + offset, size);

                udp.sendData(packet.data(), packet.size());
                usleep(100);
            }
        }

        frame_id++;
    }
}
