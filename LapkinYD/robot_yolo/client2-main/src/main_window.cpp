#include "main_window.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QDateTime>
#include <QDebug>
#include <QLineEdit>
#include <atomic>

std::atomic<int> g_displaySkip{1};

static const char *ROBOT_IP = "192.168.31.149";
static const int ROBOT_PORT = 8888;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_cmdSender = new CommandSender(ROBOT_IP, ROBOT_PORT, this);
    connect(m_cmdSender, &CommandSender::connected, this, &MainWindow::onRobotConnected);
    connect(m_cmdSender, &CommandSender::disconnected, this, &MainWindow::onRobotDisconnected);
    connect(m_cmdSender, &CommandSender::statusChanged, this, &MainWindow::appendLog);

    setupUi();

    m_keyboardHandler = new KeyboardHandler(m_cmdSender, this);
    this->installEventFilter(m_keyboardHandler);
    connect(m_keyboardHandler, &KeyboardHandler::commandReceived,
            this, &MainWindow::onCommandReceived);

    m_sensorReceiver = new SensorReceiver(5601, this);
    connect(m_sensorReceiver, &SensorReceiver::distanceUpdated,
            this, &MainWindow::onDistanceUpdated);
}

void MainWindow::setupUi()
{
    setWindowTitle("Панель управления");
    showMaximized();

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #0d1117; color: #e6edf3; }
        QLabel { color: #e6edf3; font-size: 13px; }
        QTextEdit { background: #161b22; color: #8b949e; border: 1px solid #30363d;
                    font-family: monospace; font-size: 11px; border-radius: 4px; }
        QPushButton {
            background: #21262d; color: #e6edf3; border: 1px solid #30363d;
            border-radius: 6px; padding: 8px 16px; font-size: 13px;
        }
        QPushButton:hover  { background: #30363d; border-color: #58a6ff; }
        QPushButton:pressed{ background: #388bfd; }
        QPushButton#wasd { background: #1f2937; border: 2px solid #374151;
                           border-radius: 8px; font-size: 18px; font-weight: bold;
                           min-width: 64px; min-height: 64px; }
        QPushButton#wasd:hover   { background: #374151; border-color: #60a5fa; }
        QPushButton#wasd:pressed { background: #2563eb; border-color: #93c5fd; }
        QPushButton#stop { background: #7f1d1d; border: 2px solid #991b1b;
                           border-radius: 8px; font-size: 14px; font-weight: bold;
                           min-width: 64px; min-height: 64px; }
        QPushButton#stop:hover   { background: #991b1b; }
        QPushButton#stop:pressed { background: #dc2626; }
    )");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    m_videoWidget = new VideoWidget(this);
    m_videoWidget->setMinimumSize(960, 720);
    mainLayout->addWidget(m_videoWidget, 7);

    QWidget *panel = new QWidget();
    panel->setMaximumWidth(260);
    panel->setStyleSheet("QWidget { background: #161b22; border-radius: 8px; }");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(12, 12, 12, 12);
    panelLayout->setSpacing(10);
    mainLayout->addWidget(panel, 1);

    auto addSep = [&]()
    {
        QFrame *sep = new QFrame();
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color: #30363d; background: #30363d; max-height: 1px;");
        panelLayout->addWidget(sep);
    };

    m_statusLabel = new QLabel("Статус подключения");
    m_statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #f0b429;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(m_statusLabel);
    addSep();

    QLabel *ctrlLabel = new QLabel("Управление  (W A S D)");
    ctrlLabel->setStyleSheet("color: #8b949e; font-size: 11px; font-weight: bold;");
    ctrlLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(ctrlLabel);

    QGridLayout *wasdGrid = new QGridLayout();
    wasdGrid->setSpacing(6);
    m_btnW = makeWasdBtn("W");
    m_btnA = makeWasdBtn("A");
    m_btnS = makeWasdBtn("S");
    m_btnD = makeWasdBtn("D");
    m_btnStop = new QPushButton("СТОП");
    m_btnStop->setObjectName("stop");
    m_btnLost = new QPushButton("LOST");
    m_btnLost->setObjectName("wasd");
    m_btnLost->setStyleSheet(
        "QPushButton#wasd { background: #1a1a2e; border: 2px solid #7c3aed; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; "
        "min-width: 64px; min-height: 64px; color: #c4b5fd; }"
        "QPushButton#wasd:hover { background: #2d1b69; border-color: #a78bfa; }"
        "QPushButton#wasd:pressed { background: #7c3aed; }");

    wasdGrid->addWidget(m_btnW, 0, 1);
    wasdGrid->addWidget(m_btnA, 1, 0);
    wasdGrid->addWidget(m_btnStop, 1, 1);
    wasdGrid->addWidget(m_btnD, 1, 2);
    wasdGrid->addWidget(m_btnS, 2, 1);
    wasdGrid->addWidget(m_btnLost, 0, 2);

    connect(m_btnW, &QPushButton::pressed, this, [this]
            { onBtnPressed("Forward"); });
    connect(m_btnS, &QPushButton::pressed, this, [this]
            { onBtnPressed("Back"); });
    connect(m_btnA, &QPushButton::pressed, this, [this]
            { onBtnPressed("Left"); });
    connect(m_btnD, &QPushButton::pressed, this, [this]
            { onBtnPressed("Right"); });
    connect(m_btnStop, &QPushButton::clicked, this, [this]
            { m_cmdSender->sendCommand("Stop"); appendLog("Ручной стоп"); });
    connect(m_btnLost, &QPushButton::clicked, this, [this]
            { m_cmdSender->sendCommand("Lost"); appendLog("<span style='color:#a78bfa;'>↩ Lost mode активирован</span>"); });
    connect(m_btnW, &QPushButton::released, this, &MainWindow::onBtnReleased);
    connect(m_btnS, &QPushButton::released, this, &MainWindow::onBtnReleased);
    connect(m_btnA, &QPushButton::released, this, &MainWindow::onBtnReleased);
    connect(m_btnD, &QPushButton::released, this, &MainWindow::onBtnReleased);

    panelLayout->addLayout(wasdGrid);
    addSep();

    m_distanceLabel = new QLabel("Расстояние до ближайшего объекта: ");
    m_distanceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #3fb950;");
    m_distanceLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(m_distanceLabel);
    addSep();

    m_lastCmdLabel = new QLabel("Команда: ");
    m_lastCmdLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #58a6ff;");
    m_lastCmdLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(m_lastCmdLabel);
    addSep();

    QLabel *skipTitle = new QLabel("Кадры видео");
    skipTitle->setStyleSheet("color: #8b949e; font-size: 11px; font-weight: bold;");
    skipTitle->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(skipTitle);

    QHBoxLayout *skipLayout = new QHBoxLayout();
    QPushButton *btnFewer = new QPushButton("− FPS");
    QPushButton *btnMore = new QPushButton("+ FPS");
    m_skipLabel = new QLabel("Пропуск: 1");
    m_skipLabel->setAlignment(Qt::AlignCenter);
    m_skipLabel->setStyleSheet("color: #79c0ff; font-weight: bold;");
    connect(btnFewer, &QPushButton::clicked, this, &MainWindow::onSkipMore);
    connect(btnMore, &QPushButton::clicked, this, &MainWindow::onSkipLess);
    skipLayout->addWidget(btnFewer);
    skipLayout->addWidget(m_skipLabel);
    skipLayout->addWidget(btnMore);
    panelLayout->addLayout(skipLayout);
    addSep();

    QLabel *batchTitle = new QLabel("Batch команды");
    batchTitle->setStyleSheet("color: #8b949e; font-size: 11px; font-weight: bold;");
    batchTitle->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(batchTitle);

    QLineEdit *batchEdit = new QLineEdit();
    batchEdit->setPlaceholderText("Forward:2s;Left:1s;Back:1s");
    batchEdit->setStyleSheet(
        "QLineEdit { background: #161b22; color: #e6edf3; border: 1px solid #30363d;"
        "border-radius: 4px; padding: 4px; font-size: 11px; }");
    panelLayout->addWidget(batchEdit);

    QPushButton *btnSendBatch = new QPushButton("Выполнить");
    btnSendBatch->setStyleSheet(
        "QPushButton { background: #1f6feb; color: white; border-radius: 6px;"
        "padding: 6px; font-weight: bold; }"
        "QPushButton:hover { background: #388bfd; }");
    connect(btnSendBatch, &QPushButton::clicked, this, [this, batchEdit]()
            {
        QString cmd = batchEdit->text().trimmed();
        if (!cmd.isEmpty()) {
            m_cmdSender->sendBatch(cmd);
            appendLog("Batch: " + cmd);
        } });
    panelLayout->addWidget(btnSendBatch);
    addSep();

    QLabel *logTitle = new QLabel("Системный лог");
    logTitle->setStyleSheet("color: #8b949e; font-size: 11px; font-weight: bold;");
    logTitle->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(logTitle);

    m_logWidget = new QTextEdit();
    m_logWidget->setReadOnly(true);
    panelLayout->addWidget(m_logWidget, 1);

    updateSkipLabel();
    appendLog("Система запущена");
}

QPushButton *MainWindow::makeWasdBtn(const QString &label)
{
    QPushButton *btn = new QPushButton(label);
    btn->setObjectName("wasd");
    return btn;
}

void MainWindow::updateFrame(const QImage &frame)
{
    QMetaObject::invokeMethod(m_videoWidget, "updateFrame",
                              Qt::QueuedConnection, Q_ARG(QImage, frame));
}

void MainWindow::appendLog(const QString &msg)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logWidget->append(QString("<span style='color:#6e7681;'>[%1]</span> %2").arg(time, msg));
}

void MainWindow::onCommandReceived(const QString &cmd)
{
    QString color = (cmd == "Stop") ? "#f85149" : "#3fb950";
    m_lastCmdLabel->setText(QString("<span style='color:%1;'>%2</span>").arg(color, cmd));
}

void MainWindow::onBtnPressed(const QString &cmd)
{
    m_cmdSender->sendCommand(cmd);
    onCommandReceived(cmd);
}

void MainWindow::onBtnReleased()
{
    m_cmdSender->sendCommand("Stop");
    onCommandReceived("Stop");
}

void MainWindow::onRobotConnected()
{
    m_statusLabel->setText(" (!) Подключён к роботу");
    m_statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #3fb950;");
    appendLog("<span style='color:#3fb950;'>Робот подключён</span>");
}

void MainWindow::onRobotDisconnected()
{
    m_statusLabel->setText(" (Х) Нет связи с роботом");
    m_statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #f85149;");
    appendLog("<span style='color:#f85149;'>Робот отключён</span>");
}

void MainWindow::onSkipMore()
{
    if (g_displaySkip < 10)
        g_displaySkip++;
    updateSkipLabel();
}

void MainWindow::onSkipLess()
{
    if (g_displaySkip > 1)
        g_displaySkip--;
    updateSkipLabel();
}

void MainWindow::updateSkipLabel()
{
    m_skipLabel->setText(QString("Пропуск: %1").arg((int)g_displaySkip));
}

void MainWindow::onDistanceUpdated(float cm, bool blocked)
{
    if (blocked)
    {
        m_distanceLabel->setText(QString("Стоп: %1 см").arg(cm, 0, 'f', 1));
        m_distanceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #f85149;");
    }
    else if (cm < 15.0f)
    {
        m_distanceLabel->setText(QString("Близко: %1 см").arg(cm, 0, 'f', 1));
        m_distanceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #f0b429;");
    }
    else
    {
        m_distanceLabel->setText(QString("Расстояние: %1 см").arg(cm, 0, 'f', 1));
        m_distanceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #3fb950;");
    }
}
