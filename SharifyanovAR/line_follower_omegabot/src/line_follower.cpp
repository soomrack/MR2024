#include <Arduino.h>

#define PIN_DIR_R 4
#define PIN_DIR_L 7
#define PIN_PWR_R 5
#define PIN_PWR_L 6
#define PIN_SENS_R A0
#define PIN_SENS_L A3
#define PIN_BTN 8

unsigned long START_TIMER_MILLIS = 0;
const int SEARCH_INTERVAL = 10000; // 10 sec

enum State {
    FOLLOW_LINE,
    SEARCH_LINE,
    STOPPED
};

enum lastPositionOfLine {
    LEFT,
    RIGHT
};

State CURRENT_STATE;
lastPositionOfLine LAST_POSITION_OF_LINE;
int COLOR_BARRIER;

void initPins() {
    pinMode(PIN_DIR_R, OUTPUT);
    pinMode(PIN_DIR_L, OUTPUT);
    pinMode(PIN_PWR_R, OUTPUT);
    pinMode(PIN_PWR_L, OUTPUT);
    pinMode(PIN_SENS_R, INPUT);
    pinMode(PIN_SENS_L, INPUT);
    pinMode(PIN_BTN, INPUT);
}

int identifyCurrentColor() {
    int rightSensor = analogRead(PIN_SENS_R);
    int leftSensor = analogRead(PIN_SENS_L);
    return (rightSensor + leftSensor) / 2;
}

void followLine() {
    if (isOnLine()) {
        goForward();
    } else if (isLineOnTheLeft()) {
        goLeft();
        LAST_POSITION_OF_LINE = LEFT;
    } else if (isLineOnTheRight()) {
        goRight();
        LAST_POSITION_OF_LINE = RIGHT;
    } else {
        startSearch();
    }
}

void searchLine() {
    unsigned long currentMillis = millis();
    if (currentMillis - START_TIMER_MILLIS >= SEARCH_INTERVAL) {
        CURRENT_STATE = STOPPED;
        return;
    }
    if (isLineOnTheLeft() || isLineOnTheRight() || isOnLine()) {
        //goForward();
        stopSearchLineFound();
        return;
    }
    if (LAST_POSITION_OF_LINE == RIGHT) {
        goSearchRight();
    } else if (LAST_POSITION_OF_LINE == LEFT) {
        goSearchLeft();
    }

}

void goForward() {
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 1);

    for (int i = 50; i <= 255; i += 6) {
        analogWrite(PIN_PWR_R, i);
        analogWrite(PIN_PWR_L, i);
    }
}

void goLeft() {
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 0);

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PIN_PWR_R, i);
    }
    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PIN_PWR_L, i);
    }
    delay(30);
}
void goRight() {
    digitalWrite(PIN_DIR_R, 0);
    digitalWrite(PIN_DIR_L, 1);

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PIN_PWR_R, i);
    }
    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PIN_PWR_L, i);
    }
    delay(30);
}

void goSearchLeft() {
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 150);

    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 0);
}

void goSearchRight() {
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 0);

    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 150);
}

void stop() {
    analogWrite(PIN_PWR_L, 0);
    analogWrite(PIN_PWR_R, 0);
}

bool isOnLine() {
    return leftIsBlack() && rightIsBlack();
}

bool isLineOnTheLeft() {
    return leftIsBlack() && !rightIsBlack();
}

bool isLineOnTheRight() {
    return !leftIsBlack() && rightIsBlack();
}

bool leftIsBlack() {
    return analogRead(PIN_SENS_L) > COLOR_BARRIER;
}

bool rightIsBlack() {
    return analogRead(PIN_SENS_R) > COLOR_BARRIER;
}

void startSearch() {
    CURRENT_STATE = SEARCH_LINE;
    START_TIMER_MILLIS = millis();
}

void stopSearchNothingFound() {
    playEndMusic();
    stop();
    exit(1);
}

void stopSearchLineFound() {
    CURRENT_STATE = FOLLOW_LINE;
}

bool buttonPressed() {
    return digitalRead(PIN_BTN);
}

void playEndMusic()
{
    tone(9, 261.63);
    delay(600);
    tone(9, 329.63);
    delay(600);
    tone(9, 392);
    delay(600);
    noTone(9);
}


void setup() {
    initPins();

    CURRENT_STATE = FOLLOW_LINE;
    LAST_POSITION_OF_LINE = RIGHT;

    while (!buttonPressed()) {}

    int whiteColor = identifyCurrentColor();

    delay(1000); // задержка для корректного считывания данных с датчиков цвета для калибровки; установлено эмпирическим путем
    while (!buttonPressed()) {}

    int blackColor = identifyCurrentColor();

    COLOR_BARRIER = (whiteColor + blackColor) / 2;

    delay(1000); // задержка для корректного считывания данных с датчиков цвета для калибровки; установлено эмпирическим путем
    while (!buttonPressed()) {}
}

void loop() {
    if (CURRENT_STATE == FOLLOW_LINE) {
        followLine();
    } else if (CURRENT_STATE == SEARCH_LINE) {
        searchLine();
    } else if (CURRENT_STATE == STOPPED) {
        stopSearchNothingFound();
    }
    
}