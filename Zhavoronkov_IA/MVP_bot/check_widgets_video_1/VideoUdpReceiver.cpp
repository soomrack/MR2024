#include "VideoUdpReceiver.h"
#include <opencv2/opencv.hpp>

VideoUdpReceiver::VideoUdpReceiver(QObject* parent)
    : QThread(parent) {}

void VideoUdpReceiver::run() {
    UdpSocket udp;
    udp.bindSocket(6000);

    while (running) {

        std::map<uint32_t, std::vector<std::vector<uint8_t>>> frames;

        uint8_t buffer[1500];

        while (true) {
            ssize_t len = udp.receiveData(buffer, sizeof(buffer));
            if (len <= (ssize_t)sizeof(ChunkHeader)) continue;

            ChunkHeader hdr;
            memcpy(&hdr, buffer, sizeof(hdr));

            auto& chunks = frames[hdr.frame_id];
            if (chunks.empty())
                chunks.resize(hdr.chunk_count);

            chunks[hdr.chunk_id] =
                    std::vector<uint8_t>(buffer + sizeof(hdr),
                                         buffer + len);

            bool complete = true;
            for (auto& c : chunks)
                if (c.empty()) complete = false;

            if (complete) {
                std::vector<uint8_t> jpg;
                for (auto& c : chunks)
                    jpg.insert(jpg.end(), c.begin(), c.end());

                cv::Mat img = cv::imdecode(jpg, cv::IMREAD_COLOR);
                if (!img.empty()) {
                    QImage qimg(img.data, img.cols, img.rows,
                                img.step, QImage::Format_BGR888);
                    emit frameReady(qimg.copy());
                }

                frames.erase(hdr.frame_id);
            }
        }
    }
}

void VideoUdpReceiver::stop() {
    running = false;
}

