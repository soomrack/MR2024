#pragma once

#include <QThread>
#include <QImage>
#include <map>
#include <vector>
#include "UdpSocket.h"

struct ChunkHeader {
    uint32_t frame_id;
    uint16_t chunk_id;
    uint16_t chunk_count;
};

class VideoUdpReceiver : public QThread {
    Q_OBJECT
public:
    explicit VideoUdpReceiver(QObject* parent = nullptr);
    void stop();

signals:
    void frameReady(const QImage&);

protected:
    void run() override;

private:
    std::atomic<bool> running{true};
};
