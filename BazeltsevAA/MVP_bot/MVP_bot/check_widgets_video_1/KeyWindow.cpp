#include "KeyWindow.h"

KeyWindow::KeyWindow(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void KeyWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();

    // Игнорирование авто-повтора
    if (event->isAutoRepeat())
        return;

    // Обработка клавиш разрешения
    if (key == Qt::Key_X) {
        emit resolutionDownPressed();
        return;
    }
    
    if (key == Qt::Key_C) {
        emit resolutionUpPressed();
        return;
    }

    auto it = keyToButton.find(key);
    if (it != keyToButton.end() && it->second != nullptr) {
        QPushButton* btn = it->second;
        if (!pressedKeys[key]) {
            btn->setDown(true);
            emit btn->pressed();      // вызов сигнала pressed
            pressedKeys[key] = true;
        }
    }

    QWidget::keyPressEvent(event);
}

void KeyWindow::keyReleaseEvent(QKeyEvent* event) {
    int key = event->key();

    // Игнорирование авто-повтора отпускания
    if (event->isAutoRepeat())
        return;

    // Игнорирование клавиши разрешения при отпускании
    if (key == Qt::Key_X || key == Qt::Key_C) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    auto it = keyToButton.find(key);
    if (it != keyToButton.end() && it->second != nullptr) {
        QPushButton* btn = it->second;
        btn->setDown(false);    
        emit btn->released();   // вызов сигнал released
        pressedKeys[key] = false;
    }

    QWidget::keyReleaseEvent(event);
}
