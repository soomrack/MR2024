#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "sensors_and_leds_handler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<Sensors_and_leds_handler>("sense", 1, 0, "Sensors_and_leds_handler");

    const QUrl url("qrc:/path/main.qml");
    engine.load(url);

    return app.exec();
}
