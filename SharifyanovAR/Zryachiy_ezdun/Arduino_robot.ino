#include <Arduino.h>

#define PIN_DIR_R 4
#define PIN_DIR_L 7
#define PIN_PWR_R 5
#define PIN_PWR_L 6
#define PHOTO_SENSOR A0

unsigned long lastSensorSendTime = 0;
const unsigned long SENSOR_SEND_INTERVAL = 1000;

void initPins() {
    pinMode(PIN_DIR_R, OUTPUT);
    pinMode(PIN_DIR_L, OUTPUT);
    pinMode(PIN_PWR_R, OUTPUT);
    pinMode(PIN_PWR_L, OUTPUT);
    pinMode(PHOTO_SENSOR, INPUT);
    stopMotors();
}

void stopMotors() {
    digitalWrite(PIN_PWR_L, LOW);
    digitalWrite(PIN_PWR_R, LOW);
}

void moveForward() {
    digitalWrite(PIN_DIR_L, HIGH);
    digitalWrite(PIN_DIR_R, HIGH);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
}

void moveBackward() {
    digitalWrite(PIN_DIR_L, LOW);
    digitalWrite(PIN_DIR_R, LOW);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
}

void turnLeft() {
    digitalWrite(PIN_DIR_L, LOW);
    digitalWrite(PIN_DIR_R, HIGH);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
}

void turnRight() {
    digitalWrite(PIN_DIR_L, HIGH);
    digitalWrite(PIN_DIR_R, LOW);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
}

int readPhotoSensor() {
    return analogRead(PHOTO_SENSOR);
}

void sendSensorData() {
    int sensorValue = readPhotoSensor();
    Serial.print("SENSOR:");
    Serial.println(sensorValue);
}

void setup() {
    initPins();
    Serial.begin(115200);
    delay(2000); // Даем время на установку соединения
    
    Serial.println("READY"); // Raspberry Pi узнает, что Arduino готово
    lastSensorSendTime = millis();
}

void loop() {
    // Чтение команд с Serial (от Raspberry Pi)
    if (Serial.available() > 0) {
        char incomingChar = Serial.read();
        
        switch(incomingChar) {
            case 'w': case 'W':
                moveForward();
                Serial.println("CMD:F");
                break;
            case 's': case 'S':
                moveBackward();
                Serial.println("CMD:B");
                break;
            case 'a': case 'A':
                turnLeft();
                Serial.println("CMD:L");
                break;
            case 'd': case 'D':
                turnRight();
                Serial.println("CMD:R");
                break;
            case ' ':
                stopMotors();
                Serial.println("CMD:S");
                break;
            case 'q': case 'Q':
                stopMotors();
                Serial.println("EXIT");
                while(true) { delay(1000); }
                break;
        }
    }
    
    // Отправка данных датчика раз в секунду
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorSendTime >= SENSOR_SEND_INTERVAL) {
        sendSensorData();
        lastSensorSendTime = currentMillis;
    }
    
    delay(10);
}
