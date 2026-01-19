#include <Arduino.h>

// Пины моторов
#define PIN_DIR_R 4
#define PIN_DIR_L 7
#define PIN_PWR_R 5
#define PIN_PWR_L 6

// Пины ультразвукового датчика HC-SR04
#define HC_TRIG 11
#define HC_ECHO 10

// Константы
const unsigned long SENSOR_SEND_INTERVAL = 100; // 0.1 секунды (100 мс)
const float MIN_DISTANCE_CM = 20.0; // Минимальное расстояние для движения вперед (20 см)

// Переменные
unsigned long lastSensorSendTime = 0;
bool forwardBlocked = false; // Флаг блокировки движения вперед
float lastDistance = 0.0;

// Функция инициализации пинов
void initPins() {
    pinMode(PIN_DIR_R, OUTPUT);
    pinMode(PIN_DIR_L, OUTPUT);
    pinMode(PIN_PWR_R, OUTPUT);
    pinMode(PIN_PWR_L, OUTPUT);
    
    // Пины ультразвукового датчика
    pinMode(HC_TRIG, OUTPUT);
    pinMode(HC_ECHO, INPUT);
    
    stopMotors();
}

// Остановка моторов
void stopMotors() {
    digitalWrite(PIN_PWR_L, LOW);
    digitalWrite(PIN_PWR_R, LOW);
}

// Движение вперед с проверкой расстояния
void moveForward() {
    if (!forwardBlocked) {
        digitalWrite(PIN_DIR_L, HIGH);
        digitalWrite(PIN_DIR_R, HIGH);
        digitalWrite(PIN_PWR_L, HIGH);
        digitalWrite(PIN_PWR_R, HIGH);
        Serial.println("CMD:F");
    } else {
        // Расстояние слишком маленькое - движение вперед заблокировано
        stopMotors();
        Serial.println("WARN:Forward blocked - obstacle too close!");
    }
}

// Движение назад (без проверки расстояния)
void moveBackward() {
    digitalWrite(PIN_DIR_L, LOW);
    digitalWrite(PIN_DIR_R, LOW);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
    Serial.println("CMD:B");
}

// Поворот налево (без проверки расстояния)
void turnLeft() {
    digitalWrite(PIN_DIR_L, LOW);
    digitalWrite(PIN_DIR_R, HIGH);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
    Serial.println("CMD:L");
}

// Поворот направо (без проверки расстояния)
void turnRight() {
    digitalWrite(PIN_DIR_L, HIGH);
    digitalWrite(PIN_DIR_R, LOW);
    digitalWrite(PIN_PWR_L, HIGH);
    digitalWrite(PIN_PWR_R, HIGH);
    Serial.println("CMD:R");
}

// Измерение расстояния ультразвуковым датчиком
float measureDistance() {
    // Отправляем импульс
    digitalWrite(HC_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(HC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(HC_TRIG, LOW);
    
    // Измеряем время отклика
    long duration = pulseIn(HC_ECHO, HIGH, 30000); // Таймаут 30 мс (~5 метров)
    
    // Рассчитываем расстояние (скорость звука ~340 м/с)
    // distance = duration * 0.034 / 2
    float distance = duration * 0.034 / 2.0;
    
    // Если нет отклика, возвращаем большое расстояние
    if (duration == 0) {
        distance = 999.0; // Максимальное расстояние
    }
    
    return distance;
}

// Проверка и обновление флага блокировки
void updateDistanceSafety() {
    float distance = measureDistance();
    lastDistance = distance;
    
    // Обновляем флаг блокировки
    forwardBlocked = (distance < MIN_DISTANCE_CM);
    
    // Если расстояние маленькое и робот едет вперед - останавливаем
    if (forwardBlocked) {
        // Проверяем текущее направление
        // Если двигаемся вперед - останавливаем
        if (digitalRead(PIN_DIR_L) == HIGH && digitalRead(PIN_DIR_R) == HIGH &&
            digitalRead(PIN_PWR_L) == HIGH && digitalRead(PIN_PWR_R) == HIGH) {
            stopMotors();
            Serial.println("WARN:Emergency stop - obstacle detected!");
        }
    }
}

// Отправка данных с датчика
void sendSensorData() {
    Serial.print("DISTANCE:");
    Serial.print(lastDistance);
    Serial.print("cm");
    
    // Добавляем информацию о блокировке
    if (forwardBlocked) {
        Serial.print(" BLOCKED");
    }
    
    Serial.println();
}

void setup() {
    initPins();
    Serial.begin(115200);
    delay(2000); // Даем время на установку соединения
    
    Serial.println("READY:Ultrasonic sensor initialized"); // Raspberry Pi узнает, что Arduino готово
    lastSensorSendTime = millis();
}

void loop() {
    // Постоянно проверяем расстояние для безопасности
    updateDistanceSafety();
    
    // Чтение команд с Serial (от Raspberry Pi)
    if (Serial.available() > 0) {
        char incomingChar = Serial.read();
        
        switch(incomingChar) {
            case 'w': case 'W':
                moveForward();
                break;
            case 's': case 'S':
                moveBackward();
                break;
            case 'a': case 'A':
                turnLeft();
                break;
            case 'd': case 'D':
                turnRight();
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
    
    // Отправка данных датчика каждые 0.1 секунды (100 мс)
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorSendTime >= SENSOR_SEND_INTERVAL) {
        sendSensorData();
        lastSensorSendTime = currentMillis;
    }
    
    // Небольшая задержка для стабильности
    delay(10);
}
