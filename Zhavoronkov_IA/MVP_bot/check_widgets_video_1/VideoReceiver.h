#pragma once

#include <QThread>
#include <QImage>

class VideoReceiver : public QThread {
    Q_OBJECT

public:
    explicit VideoReceiver(const QString& url, QObject* parent = nullptr);
    void stop();

signals:
    void frameReady(const QImage& frame);

protected:
    void run() override;

private:
    QString streamUrl;
    bool running;
};
