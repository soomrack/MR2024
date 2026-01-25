#pragma once
#include <QWidget>
#include <QPushButton>
#include <unordered_map>
#include <QKeyEvent>

class KeyWindow : public QWidget {
    Q_OBJECT
public:
    explicit KeyWindow(QWidget *parent = nullptr);

    std::unordered_map<int, QPushButton*> keyToButton;
    std::unordered_map<int, bool> pressedKeys;

signals:
    void resolutionDownPressed();  
    void resolutionUpPressed();    

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};
