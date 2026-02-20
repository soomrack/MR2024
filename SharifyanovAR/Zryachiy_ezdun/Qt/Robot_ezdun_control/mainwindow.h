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

#include "command_sender.h"
#include "video_streamer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupConnections();

    CommandSender *sender;
    VideoStreamer *streamer;

    QTextEdit *logBox;
    QLabel *connectionStatus;
    QLabel *videoStatus;
    QLabel *instructions;
    QLabel *videoDisplay;

    QPushButton *connectButton;
    QPushButton *startVideoButton;
    QPushButton *stopVideoButton;
};

#endif // MAINWINDOW_H
