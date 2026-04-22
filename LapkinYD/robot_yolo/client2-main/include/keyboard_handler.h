#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include "command_sender.h"

class KeyboardHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardHandler(CommandSender *sender, QObject *parent = nullptr);
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void commandReceived(const QString &command);

private slots:
    void onRepeatTimer();

private:
    CommandSender       *m_sender;
    QTimer              *m_repeatTimer;
    QString              m_currentCommand;
    QMap<int, QString>   m_keyMap;
    void sendCommand(const QString &command);
};

#endif // KEYBOARD_HANDLER_H
