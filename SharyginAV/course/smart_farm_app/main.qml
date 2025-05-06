import QtQuick
import QtQuick.Controls
import sense 1.0

ApplicationWindow {
    width: 300
    height: 500
    visible: true
    title: "Smart_farm"

    Sensors_and_leds_handler {
        id: sensors_and_leds_handler
        onSensorValuesChanged: {
            text_time.text = "Текущее время: " + time;
            text_temperature.text = "Температура: " + temperature;
            text_humidity.text = "Влажность воздуха: " + humidity;
            text_humidity_dirt.text = "Влажность почвы: " + humidity_dirt;
        }
    }

    Column {
        spacing: 10
        anchors.centerIn: parent

        Text {
            id: text_time
            text: "Текущее время: -"
        }

        Text {
            id: text_temperature
            text: "Температура: -"
        }

        Text {
            id: text_humidity
            text: "Влажность воздуха: -"
        }

        Text {
            id: text_humidity_dirt
            text: "Влажность почвы: -"
        }

        Button {
            text: "Начать чтение"
            onClicked: sensors_and_leds_handler.startReading("ttyUSB0");
        }

        Switch {
            text: "Лампа"
            checked: false
            onToggled: {
                if(checked === true) {
                    sensors_and_leds_handler.sendCommand("l");
                }
                else {
                    sensors_and_leds_handler.sendCommand("k");
                }
            }
        }

        Switch {
            text: "Нагреватель"
            checked: false
            onToggled: {
                if(checked === true) {
                    sensors_and_leds_handler.sendCommand("h");
                }
                else {
                    sensors_and_leds_handler.sendCommand("j");
                }
            }
        }

        Switch {
            text: "Насос"
            checked: false
            onToggled: {
                if(checked === true) {
                    sensors_and_leds_handler.sendCommand("p");
                }
                else {
                    sensors_and_leds_handler.sendCommand("o");
                }
            }
        }

        Switch {
            text: "Вентилятор"
            checked: false
            onToggled: {
                if(checked === true) {
                    sensors_and_leds_handler.sendCommand("v");
                }
                else {
                    sensors_and_leds_handler.sendCommand("b");
                }
            }
        }
    }

}
