#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "command_sender.h"
#include "video_streamer.h"
#include "data_logger.h"
#include "data_receiver.h"
#include "batch_command_sender.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void sendBatchCommand(const QString& command);
    //void onBatchCompleted();
    //void updateConnectionStatus(bool connected);
    //void updateVideoStatus(bool streaming);
    //void logMessage(const QString& message);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupConnections();
    void createBatchControlPanel();
    void createStatusPanel();
    void createLogPanel();
    void createVideoPanel();

    // Основные компоненты
    CommandSender *sender;
    VideoStreamer *streamer;
    DataLogger* logger;
    DataReceiver* receiver;
    BatchCommandSender* batchSender;

    // UI элементы для отображения
    QTextEdit *logBox;
    QLabel *connectionStatus;
    QLabel *videoStatus;
    QLabel *instructions;
    QLabel *videoDisplay;

    QPushButton *connectButton;
    QPushButton *startVideoButton;
    QPushButton *stopVideoButton;

    QTextEdit *commandsLog;
    QLabel *distanceLabel;
    QLabel *temperatureValue;

    // UI элементы для пакетных команд
    QGroupBox *batchGroupBox;
    QLineEdit *batchCommandEdit;
    QPushButton *sendBatchButton;
    QPushButton *clearBatchButton;

    QComboBox *directionCombo;
    QSpinBox *durationSpinBox;
    QPushButton *addToQueueButton;
    QPushButton *sendQueueButton;
    QPushButton *clearQueueButton;

    QTextEdit *batchQueueDisplay;
    QStringList currentBatchQueue;

    QLabel *batchStatusLabel;
};

#endif // MAINWINDOW_H
