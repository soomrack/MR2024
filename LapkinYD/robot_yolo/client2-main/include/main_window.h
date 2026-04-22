#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QImage>
#include <atomic>

#include "video_widget.h"
#include "keyboard_handler.h"
#include "command_sender.h"
#include "sensor_receiver.h"

extern std::atomic<int> g_displaySkip;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void updateFrame(const QImage &frame);
    CommandSender *commandSender() const { return m_cmdSender; }

public slots:
    void appendLog(const QString &msg);
    void onDistanceUpdated(float cm, bool blocked);

private slots:
    void onSkipMore();
    void onSkipLess();
    void onCommandReceived(const QString &cmd);
    void onRobotConnected();
    void onRobotDisconnected();
    void onBtnPressed(const QString &cmd);
    void onBtnReleased();

private:
    VideoWidget     *m_videoWidget;
    KeyboardHandler *m_keyboardHandler;
    CommandSender   *m_cmdSender;
    SensorReceiver  *m_sensorReceiver;
    QLabel          *m_distanceLabel;

    QLabel      *m_skipLabel;
    QLabel      *m_statusLabel;
    QLabel      *m_lastCmdLabel;
    QTextEdit   *m_logWidget;

    QPushButton *m_btnW, *m_btnA, *m_btnS, *m_btnD, *m_btnStop, *m_btnLost;

    void setupUi();
    void updateSkipLabel();
    QPushButton *makeWasdBtn(const QString &label);
};

#endif // MAIN_WINDOW_H
