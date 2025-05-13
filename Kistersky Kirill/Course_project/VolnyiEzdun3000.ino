#include <SoftwareSerial.h>

// Конфигурация пинов
#define DIR_R_PIN 4
#define DIR_L_PIN 7
#define PWR_R_PIN 5
#define PWR_L_PIN 6
#define SENS_R_PIN A0
#define SENS_L_PIN A1
#define BTN_PIN 8
#define BUZZER 11

// Настройки
#define BASE_SPEED 75
#define TURN_SPEED 120
#define TURN_DURATION 2000
#define BUZZ_DURATION 200

// Направления
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// Карта маршрутов {N, E, S, W}
const int ROUTE_MAP[12][4] = {
  /* 0 */ { 0, 0, 0, 0 },
  /* 1 */ { 4, 0, 0, 0 },
  /* 2 */ { 0, 0, 0, 4 },
  /* 3 */ { 0, 4, 0, 0 },
  /* 4 */ { 7, 2, 1, 3 },
  /* 5 */ { 0, 0, 0, 7 },
  /* 6 */ { 0, 7, 0, 0 },
  /* 7 */ { 10, 5, 4, 6 },
  /* 8 */ { 0, 0, 0, 10 },
  /* 9 */ { 0, 10, 0, 0 },
  /* 10 */ { 11, 8, 7, 9 },
  /* 11 */ { 0, 0, 10, 0 }
};

enum State {
  STATE_CALIBRATE,
  STATE_WAIT_DESTINATION,
  STATE_FOLLOW_LINE,
  STATE_AT_INTERSECTION,
  STATE_TURNING,
  STATE_DEAD_END,
  STATE_REVERSE,
  STATE_ARRIVED
};

// Глобальные переменные
State currentState = STATE_CALIBRATE;
int destination = 0;
int currentPosition = 0;
int previousPosition = 0;
int orientation = NORTH;
unsigned long turnStartTime = 0;
int turnDirection = 0;
bool lineLost = false;
unsigned long lineLostTime = 0;
int lineThreshold = 0;
int color_black = 0;
int color_white = 0;
bool flag_turning = 0;

void setup() {
  pinMode(DIR_R_PIN, OUTPUT);
  pinMode(DIR_L_PIN, OUTPUT);
  pinMode(PWR_R_PIN, OUTPUT);
  pinMode(PWR_L_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
  calibrateSensors();
  beep(1);
  Serial.println("SYSTEM READY");
}

void loop() {
  checkSerial();
  int rightSensor = analogRead(SENS_R_PIN);
  int leftSensor = analogRead(SENS_L_PIN);
  handleSensors(rightSensor, leftSensor);
  runStateMachine();
}

void checkSerial() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    Serial.print("Received: ");
    Serial.println(input);

    if (input.startsWith("GO")) {
      int newDest = input.substring(2).toInt();
      if (newDest >= 1 && newDest <= 11) {
        destination = newDest;
        Serial.print("New destination: ");
        Serial.println(destination);
        //beep(2);
        if (currentState == STATE_ARRIVED) {
          currentState = STATE_FOLLOW_LINE;
        }
      }
    } else if (input.toInt() >= 1 && input.toInt() <= 11) {
      handleQRCode(input.toInt());
    }
  }
}

void handleQRCode(int newPos) {
  previousPosition = currentPosition;
  currentPosition = newPos;

  // Определение ориентации
  for (int dir = 0; dir < 4; dir++) {
    if (ROUTE_MAP[previousPosition][dir] == currentPosition) {
      orientation = dir;
      break;
    }
  }

  Serial.print("Position: ");
  Serial.print(currentPosition);
  Serial.print(" | Orientation: ");
  printDirection(orientation);

  if (currentPosition == destination) {
    currentState = STATE_ARRIVED;
    //beep(3);
  }
}

void printDirection(int dir) {
  const char* directions[] = { "NORTH", "EAST", "SOUTH", "WEST" };
  if (dir >= 0 && dir < 4) {
    Serial.println(directions[dir]);
  } else {
    Serial.println("UNKNOWN");
  }
}

void handleSensors(int rightSensor, int leftSensor) {
  bool rightOnLine = rightSensor > lineThreshold;
  bool leftOnLine = leftSensor > lineThreshold;

  switch (currentState) {
    case STATE_FOLLOW_LINE:
      if (rightOnLine && leftOnLine) {
        setMotors(BASE_SPEED, BASE_SPEED);
        //Serial.println("Followline"); //для тестирования
        lineLost = false;
      } else if (rightOnLine) {
        setMotors(BASE_SPEED, 0);
        lineLost = false;
      } else if (leftOnLine) {
        setMotors(0, BASE_SPEED);
        lineLost = false;
      } else {
        if (!lineLost) {
          lineLost = true;
          lineLostTime = millis();
        }
        if (millis() - lineLostTime > 1000) {
          setMotors(-BASE_SPEED, BASE_SPEED);
        }
      }
      break;

    case STATE_AT_INTERSECTION:
    case STATE_ARRIVED:
      stopMotors();
      break;
  }
}

void runStateMachine() {
  switch (currentState) {
    case STATE_CALIBRATE:
      if (lineThreshold == 0) {
        calibrateSensors();
        //beep(2);
        currentState = STATE_WAIT_DESTINATION;
        Serial.println("Calibration complete");
      } else {
        //beep(2);
        currentState = STATE_WAIT_DESTINATION;
      }
      break;

    case STATE_WAIT_DESTINATION:
      if (destination != 0) {
        currentState = STATE_FOLLOW_LINE;
        Serial.println("Starting navigation");
      }
      break;

    case STATE_FOLLOW_LINE:
      if (currentPosition == 4 || currentPosition == 7 || currentPosition == 10) {
        turnStartTime = millis();
        currentState = STATE_AT_INTERSECTION;
        Serial.println("Intersection detected");
        //beep(1);
      }
      break;

    case STATE_AT_INTERSECTION:
      if (currentPosition == destination) {
        currentState = STATE_ARRIVED;
        Serial.println("Destination reached");
      } else {
        determineNextMove();
      }
      break;

    case STATE_TURNING:
      if (flag_turning = 0) {
        Serial.println("State turning");

        if (turnDirection == 0) {
          setMotors(BASE_SPEED, BASE_SPEED);  //Езда прямо
        }
        if (turnDirection == 1) {
          setMotors(TURN_SPEED, BASE_SPEED);  // Поворот направо
        }
        if (turnDirection == -1) {
          setMotors(BASE_SPEED, TURN_SPEED);  // Поворот налево
        }

        flag_turning = 1;

        if (millis() - turnStartTime >= TURN_DURATION) {
          currentState = STATE_FOLLOW_LINE;
          Serial.println("Turn completed");
          flag_turning = 0;
        }
      }

      break;

    case STATE_DEAD_END:
      orientation = (orientation + 2) % 4;
      currentState = STATE_REVERSE;
      turnStartTime = millis();
      Serial.println("Dead end detected");
      break;

    case STATE_REVERSE:
      if (millis() - turnStartTime >= TURN_DURATION) {
        currentState = STATE_FOLLOW_LINE;
        Serial.println("Reverse completed");
      }
      break;

    case STATE_ARRIVED:
      stopMotors();
      break;
  }
}

void determineNextMove() {
  int possiblePaths[4];
  for (int i = 0; i < 4; i++) {
    possiblePaths[i] = ROUTE_MAP[currentPosition][(i + orientation) % 4];
  }

  int relativeForward = orientation;
  int relativeRight = (orientation + 1);
  int relativeLeft = (orientation + 3);

  if (possiblePaths[relativeForward] == destination) {
    turnDirection = 0;
  } else if (possiblePaths[relativeRight] == destination) {
    turnDirection = 1;
  } else if (possiblePaths[relativeLeft] == destination) {
    turnDirection = -1;
  } else {
    turnDirection = 2;  //поменяли с 0, чтобы инвертировать ориентацию при развороте
  }
  //orientation = (orientation + turnDirection + 4) % 4;
  // Обновляем ориентацию с учетом поворота
  if (turnDirection == 2) {
    orientation = (orientation + 2);  // Инвертируем направление
  } else {
    orientation = (orientation + turnDirection + 4);
  }

  currentState = STATE_TURNING;

  Serial.print("Turning ");
  Serial.println(turnDirection == 1 ? "RIGHT" : turnDirection == -1 ? "LEFT"
                                                                    : "STRAIGHT");
}

void setMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(DIR_L_PIN, leftSpeed > 0 ? HIGH : LOW);
  digitalWrite(DIR_R_PIN, rightSpeed > 0 ? HIGH : LOW);
  analogWrite(PWR_L_PIN, abs(leftSpeed));
  analogWrite(PWR_R_PIN, abs(rightSpeed));
}

void stopMotors() {
  setMotors(0, 0);
}

void calibrateSensors() {
  Serial.println("Calibration started");

  // 1. Белая поверхность (первая!)
  Serial.println("1. Place on WHITE surface and press button");
  while (digitalRead(BTN_PIN) == LOW)
    ;
  color_white = (analogRead(SENS_L_PIN) + analogRead(SENS_R_PIN)) / 2;
  Serial.println(color_white);
  beep(1);

  delay(1000);

  // 2. Черная линия
  Serial.println("2. Place on BLACK line and press button");
  while (digitalRead(BTN_PIN) == LOW)
    ;
  color_black = (analogRead(SENS_L_PIN) + analogRead(SENS_R_PIN)) / 2;
  Serial.println(color_black);
  beep(2);

  // Проверка калибровки
  if (color_white >= color_black || abs(color_white - color_black) < 100) {
    Serial.println("CALIBRATION ERROR!");
    while (1) {
      beep(1);
      delay(1000);
    }
  }
  lineThreshold = (color_white + color_black) / 2;

  Serial.println("Calibration results:");
  Serial.print("White: ");
  Serial.println(color_white);
  Serial.print("Black: ");
  Serial.println(color_black);
  Serial.print("Threshold: ");
  Serial.println(lineThreshold);
}

void beep(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(BUZZ_DURATION);
    digitalWrite(BUZZER, LOW);
    if (i < count - 1) delay(BUZZ_DURATION);
  }
}

bool rightOnLine() {
  return analogRead(SENS_R_PIN) > lineThreshold;
}

bool leftOnLine() {
  return analogRead(SENS_L_PIN) > lineThreshold;
}
