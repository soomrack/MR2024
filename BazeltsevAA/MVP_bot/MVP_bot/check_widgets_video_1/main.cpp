#include <QApplication>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QSpinBox>
#include <QDateTime>
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QWidget>
#include <QKeyEvent>
#include <QSharedMemory>
#include <QImageWriter>
#include <QProcess>
#include <QSocketNotifier>

//#include <unordered_map>

#include <thread>
#include <atomic>


#include "TcpSocket.h"
//#include "UdpSocket.h"
#include "VideoUdpReceiver.h"
#include "KeyWindow.h"


// Соответствие кнопок интерфейса и отправляемых к RP символов
struct ButtonCommand {
    QPushButton* button;
    std::string command;
};


// Соответствие сохранённого кадра и времени сохранения
struct FrameData {
    QString filename;
    qint64 timestamp;
};


int main(int argc, char *argv[])
{

    std::atomic<bool> sensorThreadRunning{true};


    QApplication a(argc, argv);

    QSharedMemory sharedMemory("RobotControlPanelSingleton");

    if (!sharedMemory.create(1)) {
        sharedMemory.attach();
        sharedMemory.detach();

        QThread::msleep(200);

        if (!sharedMemory.create(1)) {
            qCritical() << "Приложение уже было запущено. Закройте все окна и попробуйте ещё раз";
            return 1;
        }
    }


    QString videoSavePath = QDir::homePath() + "/MVP_data/saved_video_frames";

    QDir saveDir(videoSavePath);
    if (!saveDir.exists()) {
        // создание всех несуществующих директории в пути
        if (saveDir.mkpath(".")) {
            qDebug() << "Created directory:" << videoSavePath;
        } else {
            qDebug() << "Failed to create directory:" << videoSavePath;
        }
    }


    // Создание папки для скриншотов
    QString screenshotPath = QDir::homePath() + "/MVP_data/screenshots";
    QDir screenshotDir(screenshotPath);
    if (!screenshotDir.exists()) {
        if (screenshotDir.mkpath(".")) {
            qDebug() << "Created screenshot directory:" << screenshotPath;
        } else {
            qDebug() << "Failed to create screenshot directory:" << screenshotPath;
        }
    }


    QString sensorLogPath = QDir::homePath() + "/MVP_data/sensor_data.txt";
    QFile sensorLogFile(sensorLogPath);

    if (!sensorLogFile.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Failed to open sensor log file";
    }



    KeyWindow window;
    window.setWindowTitle("Robot control panel");
    window.setWindowFlags(window.windowFlags() | Qt::WindowStaysOnTopHint);


    // Block Video
    auto* gb_video = new QGroupBox("Video Stream", &window);

    auto* video_display = new QLabel("Video goes here", gb_video);
    //video_display->setStyleSheet("background-color: green; min-height: 200px;");
    video_display->setAlignment(Qt::AlignCenter);

    QPushButton* btn_screenshot = new QPushButton("Screenshot",&window);
    QPushButton* btn_all_data = new QPushButton("Save all data",&window);
    QPushButton* btn_all_video = new QPushButton("Save all video",&window);
    
    QPushButton* btn_res_down = new QPushButton("Res Down (x)", &window);
    QPushButton* btn_res_up = new QPushButton("Res Up (c)", &window);
    
    QCheckBox* cb_quality = new QCheckBox("High Quality", &window);
    QLabel* connection_status_label = new QLabel("FPS: ", &window);

    auto* videoButtons = new QHBoxLayout;
    videoButtons->addWidget(btn_screenshot);
    videoButtons->addWidget(btn_all_data);
    videoButtons->addWidget(btn_all_video);
    
    videoButtons->addWidget(btn_res_down);
    videoButtons->addWidget(btn_res_up);

    auto* videoControls = new QHBoxLayout;
    videoControls->addWidget(cb_quality);
    videoControls->addWidget(connection_status_label);
    videoControls->addStretch();

    auto* videoLayout = new QGridLayout;
    videoLayout->addWidget(video_display, 0, 0);
    videoLayout->addLayout(videoButtons, 1, 0);
    videoLayout->addLayout(videoControls, 2, 0);

    videoLayout->setRowStretch(0, 100);
    videoLayout->setRowStretch(1, 0);
    videoLayout->setRowStretch(2, 0);

    gb_video->setLayout(videoLayout);



    // Block connection
    auto* gb_connect = new QGroupBox("Connection", &window);

    QLineEdit* edit_ip = new QLineEdit("192.168.1.237", gb_connect);
    //edit_ip->setText("192.168.1.254");
    QPushButton* btn_connect = new QPushButton("Conect", gb_connect);
    QLabel* status_label = new QLabel("Disconnected", gb_connect);
    status_label->setStyleSheet("color: red; font-weight: bold;");

    auto* connectButtons = new QHBoxLayout;
    connectButtons->addWidget(btn_connect);
    connectButtons->addWidget(status_label);

    auto* connectLayout = new QVBoxLayout;
    connectLayout->addWidget(edit_ip);
    connectLayout->addLayout(connectButtons);

    gb_connect->setLayout(connectLayout);



    // Block control
    auto* gb_control = new QGroupBox("Movement Control", &window);

    QPushButton* btn_up = new QPushButton("↑", gb_control);
    QPushButton* btn_down= new QPushButton("↓", gb_control);
    QPushButton* btn_left= new QPushButton("←", gb_control);
    QPushButton* btn_right= new QPushButton("→", gb_control);
    QPushButton* btn_stop= new QPushButton("STOP", gb_control);
    QPushButton* btn_rotate_cw= new QPushButton("CW ↻", gb_control);
    QPushButton* btn_rotate_ccw= new QPushButton("CCW ↺", gb_control);
    QPushButton* cam_up= new QPushButton("C↑", gb_control);
    QPushButton* cam_down= new QPushButton("C↓", gb_control);
    QPushButton* cam_left= new QPushButton("C←", gb_control);
    QPushButton* cam_right= new QPushButton("C→", gb_control);
    QPushButton* cam_stop= new QPushButton("Cam STOP", gb_control);
    QSlider* sld_speed = new QSlider(Qt::Horizontal, &window);
    sld_speed->setRange(10,100);
    sld_speed->setValue(20);
    QSpinBox* sb_speed = new QSpinBox(&window);
    sb_speed->setRange(sld_speed->minimum(),sld_speed->maximum());
    sb_speed->setValue(sld_speed->value());

    QObject::connect(sld_speed, &QSlider::valueChanged, sb_speed, &QSpinBox::setValue);
    QObject::connect(sb_speed, QOverload<int>::of(&QSpinBox::valueChanged), sld_speed, &QSlider::setValue);

    auto* controlLayout = new QGridLayout;

    controlLayout->addWidget(btn_rotate_cw,0,2);
    controlLayout->addWidget(btn_rotate_ccw,0,0);
    controlLayout->addWidget(btn_up,1,1);
    controlLayout->addWidget(btn_left,2,0);
    controlLayout->addWidget(btn_stop,2,1);
    controlLayout->addWidget(btn_right,2,2);
    controlLayout->addWidget(btn_down,3,1);

    controlLayout->addWidget(cam_up,4,0);
    controlLayout->addWidget(cam_down,5,0);
    controlLayout->addWidget(cam_left,4,2);
    controlLayout->addWidget(cam_right,5,2);
    controlLayout->addWidget(cam_stop,5,1);

    controlLayout->addWidget(new QLabel("Speed:"),6,0);
    controlLayout->addWidget(sb_speed,6,1);
    controlLayout->addWidget(sld_speed,6,2);

    controlLayout->setColumnStretch(0,1);
    controlLayout->setColumnStretch(1,1);
    controlLayout->setColumnStretch(2,1);

    gb_control->setLayout(controlLayout);



    // Block package command
    auto* gb_packet = new QGroupBox("Package commands", &window);

    QCheckBox* cb_enable = new QCheckBox("Power on", gb_packet);
    QLineEdit* edit_time = new QLineEdit("1000", gb_packet);
    QPushButton* btn_clear= new QPushButton("Clear text", gb_packet);
    QPushButton* btn_send = new QPushButton("Send package", gb_packet);
    QTextEdit* cmd_output = new QTextEdit(gb_packet);

    auto* packetLayout = new QVBoxLayout;

    packetLayout->addWidget(cb_enable);
    packetLayout->addWidget(new QLabel("Command Time (ms):"));
    packetLayout->addWidget(edit_time);
    packetLayout->addWidget(new QLabel("Commands List:"));
    packetLayout->addWidget(cmd_output);
    packetLayout->addWidget(btn_clear);
    packetLayout->addWidget(btn_send);

    gb_packet->setLayout(packetLayout);



    // Block sensors

    auto* gb_sensors = new QGroupBox("Sensor Data", &window);

    QTableWidget* sensor_table = new QTableWidget(2,5, gb_sensors);
    sensor_table->setVerticalHeaderLabels({"Sensor","Value"});
    sensor_table->setItem(0,0, new QTableWidgetItem("IF_forw"));
    sensor_table->setItem(0,1, new QTableWidgetItem("IF_back"));
    sensor_table->setItem(0,2, new QTableWidgetItem("IF_left"));
    sensor_table->setItem(0,3, new QTableWidgetItem("IF_right"));
    sensor_table->setItem(0,4, new QTableWidgetItem("Gyroscope"));
    sensor_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* sensorLayout = new QVBoxLayout;
    sensorLayout->addWidget(sensor_table);

    gb_sensors->setLayout(sensorLayout);



    // Main Layout
    auto* mainLayout = new QGridLayout;

    mainLayout->addWidget(gb_video,0,0,4,3);
    mainLayout->addWidget(gb_connect,0,3);
    mainLayout->addWidget(gb_control,1,3,2,1);
    mainLayout->addWidget(gb_packet,3,3,3,1);
    mainLayout->addWidget(gb_sensors,4,0,2,3);

    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
    mainLayout->setColumnStretch(2,1);
    mainLayout->setColumnStretch(3,1);

    mainLayout->setRowStretch(0,1);
    mainLayout->setRowStretch(1,1);
    mainLayout->setRowStretch(2,2);
    mainLayout->setRowStretch(3,2);
    mainLayout->setRowStretch(4,1);
    mainLayout->setRowStretch(5,1);

    window.setLayout(mainLayout);
    window.resize(1200,900);
    window.setFocusPolicy(Qt::StrongFocus);

    window.show();

    // Video
    VideoUdpReceiver* video = new VideoUdpReceiver(&window);

    QVector<FrameData> frameDataList;
    const int MAX_FRAMES = 10000;
    int frameCounter = 0;

    auto saveVideoFrame = [&](const QImage& img) {
        if (frameDataList.size() >= MAX_FRAMES) {

            int oldestIndex = frameDataList.size() - MAX_FRAMES;
            qDebug() << "Достигнут лимит изображений";

            QFile::remove(frameDataList[oldestIndex].filename);

            int frameNumber = frameCounter % MAX_FRAMES;
            QString filename = QString("%1/frame_%2.jpg")
                    .arg(videoSavePath)
                    .arg(frameNumber, 6, 10, QChar('0'));

            QImageWriter writer(filename);
            writer.setCompression(100);

            if (writer.write(img)) {
                qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
                frameDataList.append({filename, timestamp});

                frameDataList.removeAt(oldestIndex);
            }
        } else {
            int frameNumber = frameDataList.size();
            QString filename = QString("%1/frame_%2.jpg")
                    .arg(videoSavePath)
                    .arg(frameNumber, 6, 10, QChar('0'));

            QImageWriter writer(filename);
            writer.setCompression(100);

            if (writer.write(img)) {
                qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
                frameDataList.append({filename, timestamp});
            }
        }

        frameCounter++;

        if (frameCounter % 30 == 0) {
            qDebug() << "Saved frame:" << frameCounter
                     << "List size:" << frameDataList.size();
        }
    };
    
  


    //QDir saveDir(videoSavePath);
    if (saveDir.exists()) {
        saveDir.removeRecursively();
    }
    saveDir.mkpath(".");

    frameDataList.clear();
    qDebug() << "Video frames will be saved to:" << videoSavePath;
    qDebug() << "Frame data list cleared";



    auto getVideoDuration = [](const QString& videoPath) -> double {
        QProcess ffprobe;
        ffprobe.start("ffprobe", QStringList()
                      << "-v" << "error"
                      << "-show_entries" << "format=duration"
                      << "-of" << "default=noprint_wrappers=1:nokey=1"
                      << videoPath);

        if (ffprobe.waitForFinished(5000)) {
            QString output = ffprobe.readAllStandardOutput();
            return output.trimmed().toDouble();
        }
        return 0.0;
    };



    QObject::connect(btn_all_video, &QPushButton::clicked, [&]() {
        qDebug() << "Creating video from saved frames with real timing...";

        if (frameDataList.isEmpty()) {
            qDebug() << "No frames to create video!";
            qDebug() << "Warning: No frames have been saved yet.";
            return;
        }

        QProcess checkProcess;
        checkProcess.start("which", QStringList() << "ffmpeg");
        checkProcess.waitForFinished();

        if (checkProcess.exitCode() != 0) {
            qDebug() << "Error: FFmpeg is not installed or not in PATH";
            qDebug() << "Please install ffmpeg: sudo apt install ffmpeg";
            return;
        }

        QString outputVideo = QString("%1/output_video_%2.mp4")
                .arg(videoSavePath)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

        QString concatFile = videoSavePath + "/concat.txt";
        QFile file(concatFile);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error: Failed to create concat file:" << file.errorString();
            return;
        }

        QTextStream stream(&file);
        qDebug() << "Creating concat file with" << frameDataList.size() << "frames";

        // Вычисление и запись длительности для каждого кадра
        for (int i = 0; i < frameDataList.size(); ++i) {
            double duration = 0.0;

            if (i < frameDataList.size() - 1) {
                qint64 timeDiff = frameDataList[i+1].timestamp - frameDataList[i].timestamp;
                duration = timeDiff / 1000.0; // в секундах

                if (duration < 0.01) duration = 0.01;
                if (duration > 10.0) duration = 10.0;
            } else {
                if (frameDataList.size() > 1) {
                    double totalDuration = 0;
                    int count = 0;


                    for (int j = 0; j < frameDataList.size() - 1; ++j) {
                        qint64 diff = frameDataList[j+1].timestamp - frameDataList[j].timestamp;
                        double d = diff / 1000.0;
                        if (d >= 0.01 && d <= 10.0) {
                            totalDuration += d;
                            count++;
                        }
                    }

                    if (count > 0) {
                        duration = totalDuration / count;
                    } else {
                        duration = 0.1;
                    }
                } else {
                    duration = 0.1;
                }
            }

            QString filePath = frameDataList[i].filename;

            stream << "file '" << filePath << "'\n";
            stream << "duration " << QString::number(duration, 'f', 3) << "\n";

            // Логирование каждых 30 кадров
            if (i % 30 == 0) {
                qDebug() << "Frame" << i << "duration:" << duration << "seconds";
            }
        }

        if (!frameDataList.isEmpty()) {
            QString lastFilePath = QDir::toNativeSeparators(frameDataList.last().filename);
            stream << "file '" << lastFilePath << "'\n";
        }

        file.close();
        qDebug() << "Concat file created:" << concatFile;

        // Статистика
        if (frameDataList.size() > 1) {
            qint64 totalTime = frameDataList.last().timestamp - frameDataList.first().timestamp;
            double avgFps = (frameDataList.size() - 1) * 1000.0 / totalTime;
            qDebug() << "Total recording time:" << totalTime / 1000.0 << "seconds";
            qDebug() << "Average FPS:" << avgFps;
        }

        // Отдельный процесс
        QProcess* process = new QProcess(&window);

        QStringList args;
        args << "-f" << "concat"
             << "-safe" << "0"
             << "-i" << concatFile
             << "-c:v" << "libx264"
             << "-pix_fmt" << "yuv420p"
             << "-vsync" << "vfr"           
             << "-y"
             << outputVideo;

        qDebug() << "Running FFmpeg command with concat demuxer";

        // Подключение обработчика завершения
        QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                         [&, process, outputVideo, concatFile](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                qDebug() << "Video created successfully with real timing!";
                qDebug() << "Video location:" << outputVideo;
                qDebug() << "Total frames:" << frameDataList.size();

                // Информация о созданном видео
                QFileInfo videoInfo(outputVideo);
                if (videoInfo.exists()) {
                    qDebug() << "Video size:" << videoInfo.size() / (1024*1024) << "MB";
                    qDebug() << "Video duration:" << getVideoDuration(outputVideo) << "seconds";
                }
            } else {
                QString error = QString(process->readAllStandardError());
                qDebug() << "FFmpeg error:" << error;
                qDebug() << "Failed to create video!";
            }

            if (QFile::exists(concatFile)) {
                QFile::remove(concatFile);
                qDebug() << "Temporary concat file removed";
            }

            process->deleteLater();
        });

        QObject::connect(process, &QProcess::readyReadStandardError, [process]() {
            QString error = process->readAllStandardError();
            if (!error.trimmed().isEmpty()) {
                qDebug() << "FFmpeg:" << error.trimmed();
            }
        });

        process->start("ffmpeg", args);

        btn_all_video->setEnabled(false);
        btn_all_video->setText("Creating video...");

        QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                         [&](int, QProcess::ExitStatus) {
            btn_all_video->setEnabled(true);
            btn_all_video->setText("Save all video");
        }
        );
    });


    QObject::connect(btn_screenshot, &QPushButton::clicked, [&]() {
        QPixmap currentPixmap = video_display->pixmap(Qt::ReturnByValue);

        if (currentPixmap.isNull()) {
            status_label->setText("No video frame!");
            QTimer::singleShot(1500, [&]() {
                if (status_label->text() == "No video frame!") status_label->setText("Connected");
            });
            return;
        }

        QString filename = QString("%1/screenshot_%2.jpg")
                .arg(screenshotPath)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz"));

        if (currentPixmap.save(filename, "JPG", 100)) {
            qDebug() << "Screenshot saved:" << filename;
        } else {
            qDebug() << "Failed to save screenshot!";
        }
    });

    TcpSocket tcpCommand;
    UdpSocket udp;
    TcpSocket tcpHeartbeat;
    TcpSocket tcpSensors;


        QObject::connect(btn_all_data, &QPushButton::clicked, [&]() {

            if (tcpCommand.getFd() < 0) {
                qDebug() << "Not connected to Raspberry Pi";
                return;
            }

            const char cmd[] = "GET_ALL_DATA\n";
            tcpCommand.sendData(cmd, strlen(cmd));
            qDebug() << "[UI] Requested all data from RP";

            QByteArray header;
            char ch;

            while (true) {
                if (tcpCommand.receiveData(&ch, 1) <= 0)
                    return;

                if (ch == '\n')
                    break;

                header.append(ch);
            }

            if (!header.startsWith("FILE:")) {
                qDebug() << "Unexpected response:" << header;
                return;
            }

            qint64 fileSize = header.mid(5).toLongLong();
            qDebug() << "Receiving archive, size =" << fileSize;

            QString savePath = QDir::homePath() + "/MVP_data";
            QDir().mkpath(savePath);

            QString filename = savePath + "/MVP_log_" +
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
                    ".tar.gz";

            QFile outFile(filename);
            if (!outFile.open(QIODevice::WriteOnly)) {
                qDebug() << "Failed to create file";
                return;
            }

            qint64 received = 0;
            char buffer[4096];

            while (received < fileSize) {
                ssize_t n = tcpCommand.receiveData(
                            buffer,
                            std::min((qint64)sizeof(buffer), fileSize - received)
                            );

                if (n <= 0)
                    break;

                outFile.write(buffer, n);
                received += n;
            }

            outFile.close();

            qDebug() << "Archive saved to:" << filename;
        });




    // Connection
    QTimer heartbeatTimer;
    heartbeatTimer.setInterval(500); // 500 мс

    QObject::connect(&heartbeatTimer, &QTimer::timeout, [&]() {
        if (tcpHeartbeat.getFd() >= 0) {
            const char ping[] = "PING";
            tcpHeartbeat.sendData(ping, sizeof(ping) - 1);
        }
    });


    QObject::connect(btn_connect, &QPushButton::clicked, [&]() {
        tcpCommand.closeSocket();
        tcpHeartbeat.closeSocket();

        std::string ip = edit_ip->text().toStdString();

        bool ok1 = tcpHeartbeat.connectTo(ip, 5000);
        bool ok2 = tcpCommand.connectTo(ip, 7000);
        bool ok3 = tcpSensors.connectTo(ip, 8000);

        int value = sld_speed->value();
        QString msg = QString("SPD:%1\n").arg(value);
        QByteArray data = msg.toUtf8();

        tcpCommand.sendData(data.constData(), data.size());

        if (ok1 && ok2 && ok3) {
            status_label->setText("Connected");
            status_label->setStyleSheet("color: green; font-weight: bold;");
            heartbeatTimer.start();
        } else {
            status_label->setText("Error");
            heartbeatTimer.stop();
        }

        QObject::connect(video, &VideoUdpReceiver::frameReady,&window,[&](const QImage& img) {
            saveVideoFrame(img);

            video_display->setPixmap(QPixmap::fromImage(img)
                                     .scaled(video_display->size(),
                                             Qt::KeepAspectRatio)); },
        Qt::UniqueConnection);

        if (!video->isRunning())
            video->start();
    });


    QObject::connect(cb_quality, &QCheckBox::stateChanged, [&]() {
        const char cmd[] = "z\n";
        tcpCommand.sendData(cmd, strlen(cmd));
    });
    
    QObject::connect(btn_res_down, &QPushButton::clicked, [&]() {
        if (tcpCommand.getFd() < 0) return;
        const char cmd[] = "x\n";
        tcpCommand.sendData(cmd, strlen(cmd));
        qDebug() << "[UI] Sent resolution down command";
    });

    QObject::connect(btn_res_up, &QPushButton::clicked, [&]() {
        if (tcpCommand.getFd() < 0) return;
        const char cmd[] = "c\n";
        tcpCommand.sendData(cmd, strlen(cmd));
        qDebug() << "[UI] Sent resolution up command";
    });

    QObject::connect(&window, &KeyWindow::resolutionDownPressed, [&]() {
        if (tcpCommand.getFd() < 0) return;
        const char cmd[] = "x\n";
        tcpCommand.sendData(cmd, strlen(cmd));
        qDebug() << "[UI] Resolution down (key x)";
    });

    QObject::connect(&window, &KeyWindow::resolutionUpPressed, [&]() {
        if (tcpCommand.getFd() < 0) return;
        const char cmd[] = "c\n";
        tcpCommand.sendData(cmd, strlen(cmd));
        qDebug() << "[UI] Resolution up (key c)";
    });


    // Control

    window.keyToButton[Qt::Key_W] = btn_up;
    window.keyToButton[Qt::Key_S] = btn_down;
    window.keyToButton[Qt::Key_A] = btn_left;
    window.keyToButton[Qt::Key_D] = btn_right;
    window.keyToButton[Qt::Key_Q] = btn_rotate_ccw;
    window.keyToButton[Qt::Key_E] = btn_rotate_cw;
    window.keyToButton[Qt::Key_X] = nullptr;
    window.keyToButton[Qt::Key_C] = nullptr;

    window.keyToButton[Qt::Key_I] = cam_up;
    window.keyToButton[Qt::Key_K] = cam_down;
    window.keyToButton[Qt::Key_J] = cam_left;
    window.keyToButton[Qt::Key_L] = cam_right;
    window.keyToButton[Qt::Key_U] = cam_stop;

    btn_stop->setEnabled(false);
    cam_stop->setEnabled(false);
    btn_stop->setVisible(false);
    cam_stop->setVisible(false);

    std::vector<ButtonCommand> buttons_control = {
        { btn_up,   "w" },
        { btn_down, "s" },
        { btn_left, "a" },
        { btn_right,"d" },
        { btn_rotate_ccw, "q" },
        { btn_rotate_cw, "e" },
    };


    std::vector<ButtonCommand> buttons_camera = {
        { cam_up, "i" },
        { cam_down, "j" },
        { cam_left, "k" },
        { cam_right, "l" }
    };


    for (auto& bc : buttons_control) {
        QObject::connect(bc.button, &QPushButton::pressed, [&, bc]() {
            if (!cb_enable->isChecked()) {
                std::string cmd = bc.command + "\n";
                tcpCommand.sendData(cmd.c_str(), cmd.size());
            } else {
                int speed = sb_speed->value();
                QString time = edit_time->text();
                QString command = QString("%1:%2:%3").arg(bc.command.c_str()).arg(speed).arg(time);
                cmd_output->append(command);
            }
        });

        QObject::connect(bc.button, &QPushButton::released, [&, bc]() {
            if (!cb_enable->isChecked()) {
                std::string stop_cmd = "n\n";
                tcpCommand.sendData(stop_cmd.c_str(), stop_cmd.size());
            }
        });
    }


//    QObject::connect(btn_stop, &QPushButton::clicked, [&]() {
//        if (!cb_enable->isChecked()) {
//            const char cmd[] = "n\n";
//            tcpCommand.sendData(cmd, strlen(cmd));
//        } else {
//            const char cmd[] = "CMD:STOP";
//            cmd_output->append(cmd);
//        }
//    });


    for (auto& bc : buttons_camera) {
        QObject::connect(bc.button, &QPushButton::pressed, [&, bc]() {
            std::string cmd = bc.command + "\n";
            tcpCommand.sendData(cmd.c_str(), cmd.size());
        });

        QObject::connect(bc.button, &QPushButton::released, [&, bc]() {
            std::string stop_cmd = "u\n";
            tcpCommand.sendData(stop_cmd.c_str(), stop_cmd.size());
        });
    }


    QObject::connect(sld_speed, &QSlider::sliderReleased, [&]() {
        if(!cb_enable->isChecked()) {
            if (tcpCommand.getFd() < 0)
                return;

            int value = sld_speed->value();
            QString msg = QString("SPD:%1\n").arg(value);
            QByteArray data = msg.toUtf8();

            tcpCommand.sendData(data.constData(), data.size());
            qDebug() << "[UI] Speed sent:" << value;
        }
    });


    QObject::connect(sb_speed, &QSpinBox::editingFinished, [&]() {
        if(!cb_enable->isChecked()) {
            if (tcpCommand.getFd() < 0)
                return;

            int value = sld_speed->value();  // Получение значения при отпускании
            QString msg = QString("SPD:%1\n").arg(value);
            QByteArray data = msg.toUtf8();

            tcpCommand.sendData(data.constData(), data.size());
            qDebug() << "[UI] Speed sent:" << value;
        }
    });


    edit_time->setEnabled(false);
    cmd_output->setEnabled(false);
    btn_clear->setEnabled(false);
    btn_send->setEnabled(false);


    QObject::connect(cb_enable,&QCheckBox::toggled, [&](bool checked){
        edit_time->setEnabled(checked);
        cmd_output->setEnabled(checked);
        btn_clear->setEnabled(checked);
        btn_send->setEnabled(checked);
    });


    QObject::connect(btn_clear, &QPushButton::clicked, [&]() {
        cmd_output->clear();
    });


    QObject::connect(btn_send, &QPushButton::clicked, [&]() {
        QString commands = cmd_output->toPlainText();

        if (commands.isEmpty()) {
            qDebug() << "Нет команд для отправки";
            return;
        }

        QString tempFilename = QDir::tempPath() + "/robot_commands_" +
                QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt";

        QFile file(tempFilename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Ошибка создания файла:" << file.errorString();
            return;
        }

        QTextStream stream(&file);
        stream << commands;
        file.close();

        qDebug() << "Файл создан:" << tempFilename;

        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Не удалось открыть файл для чтения";
            return;
        }

        QByteArray fileData = file.readAll();
        file.close();

        qint64 fileSize = fileData.size();

        QString header = QString("FILE:%1\n").arg(fileSize);
        QByteArray headerData = header.toUtf8();

        if (!tcpCommand.sendData(headerData.constData(), headerData.size())) {
            qDebug() << "Ошибка отправки заголовка";
            return;
        }

        if (!tcpCommand.sendData(fileData.constData(), fileSize)) {
            qDebug() << "Ошибка отправки файла";
            return;
        }

        qDebug() << "Файл успешно отправлен. Размер:" << fileSize << "байт";

        file.remove();
    });


    // Sensors
    std::thread sensorThread([&]() {

        QFile logFile(sensorLogPath);
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            qDebug() << "Failed to open sensor log file in thread";
            return;
        }

        QTextStream logStream(&logFile);

        char buf[128];
        QByteArray buffer;

        while (sensorThreadRunning) {

            ssize_t bytes = tcpSensors.receiveData(buf, sizeof(buf));

            if (bytes <= 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            buffer.append(buf, bytes);

            while (buffer.contains('\n')) {

                int idx = buffer.indexOf('\n');
                QByteArray line = buffer.left(idx);
                buffer.remove(0, idx + 1);

                QString data = QString::fromUtf8(line).trimmed();
                if (data.length() != 7)
                    continue;

                int if_front = data[0].digitValue();
                int if_back  = data[1].digitValue();
                int if_right = data[2].digitValue();
                int if_left = data[3].digitValue();
                int gyro     = data.mid(4,3).toInt();

                logStream << QDateTime::currentDateTime()
                                 .toString("yyyy-MM-dd hh:mm:ss.zzz")
                          << " | IF: "
                          << if_front << " "
                          << if_back  << " "
                          << if_left  << " "
                          << if_right
                          << " | GYRO: "
                          << gyro
                          << "\n";
                logStream.flush();

                QMetaObject::invokeMethod(&window, [=]() {
                    sensor_table->setItem(1,0,new QTableWidgetItem(QString::number(if_front)));
                    sensor_table->setItem(1,1,new QTableWidgetItem(QString::number(if_back)));
                    sensor_table->setItem(1,2,new QTableWidgetItem(QString::number(if_left)));
                    sensor_table->setItem(1,3,new QTableWidgetItem(QString::number(if_right)));
                    sensor_table->setItem(1,4,new QTableWidgetItem(QString::number(gyro)));
                }, Qt::QueuedConnection);
            }
        }

        logFile.close();
    });



    // Main
    QObject::connect(&a, &QApplication::aboutToQuit, [&]() {
        qDebug() << "Total frames saved:" << frameCounter << "in folder:" << videoSavePath;

        video->stop();
        video->quit();
        video->wait();

        heartbeatTimer.stop();
        tcpCommand.closeSocket();
        tcpHeartbeat.closeSocket();
        tcpSensors.closeSocket();

        sensorThreadRunning = false;

        if (sensorThread.joinable())
            sensorThread.join();

    });

    return a.exec();
}
