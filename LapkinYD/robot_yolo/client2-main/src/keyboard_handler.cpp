#include "keyboard_handler.h"
#include <QKeyEvent>
#include <QDebug>

KeyboardHandler::KeyboardHandler(CommandSender *sender, QObject *parent)
    : QObject(parent), m_sender(sender)
{
    m_keyMap[Qt::Key_W] = "Forward";
    m_keyMap[Qt::Key_S] = "Back";
    m_keyMap[Qt::Key_A] = "Left";
    m_keyMap[Qt::Key_D] = "Right";
    m_keyMap[Qt::Key_Q] = "Quit";
    m_keyMap[Qt::Key_L] = "Lost";

    m_repeatTimer = new QTimer(this);
    m_repeatTimer->setInterval(80);
    connect(m_repeatTimer, &QTimer::timeout, this, &KeyboardHandler::onRepeatTimer);
}

bool KeyboardHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->isAutoRepeat()) return true;
        if (m_keyMap.contains(ke->key())) {
            QString cmd = m_keyMap[ke->key()];
            m_currentCommand = cmd;
            sendCommand(cmd);
            emit commandReceived(cmd);
            if (cmd != "Quit")
                m_repeatTimer->start();
            return true;
        }
    } else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->isAutoRepeat()) return true;
        if (m_keyMap.contains(ke->key()) && m_keyMap[ke->key()] != "Quit") {
            m_repeatTimer->stop();
            m_currentCommand = "";
            sendCommand("Stop");
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void KeyboardHandler::sendCommand(const QString &command)
{
    if (m_sender)
        m_sender->sendCommand(command);
    qDebug() << "CMD:" << command;
}

void KeyboardHandler::onRepeatTimer()
{
    if (!m_currentCommand.isEmpty())
        sendCommand(m_currentCommand);
}
