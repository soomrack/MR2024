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
int lineThreshold = 500;
#define BASE_SPEED 150
#define TURN_SPEED 200
#define TURN_DURATION 500
#define BUZZ_DURATION 200
#define QR_TIMEOUT 3000

// Направления
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// Карта маршрутов {N, E, S, W}
const int ROUTE_MAP[12][4] = {
  /* 0 */  {0,  0,  0,  0},
  /* 1 */  {4,  0,  0,  0},  // N:4
  /* 2 */  {0,  0,  0,  4},  // W:4
  /* 3 */  {0,  4,  0,  0},  // E:4
  /* 4 */  {7,  2,  1,  3},  // N:7, E:2, S:1, W:3
  /* 5 */  {0,  0,  0,  7},  // W:7
  /* 6 */  {0,  7,  0,  0},  // E:7
  /* 7 */  {10, 5,  4,  6},  // N:10, E:5, S:4, W:6
  /* 8 */  {0,  0,  0, 10},  // W:10
  /* 9 */  {0, 10,  0,  0},  // E:10
  /* 10 */ {11, 8,  7,  9},  // N:11, E:8, S:7, W:9
  /* 11 */ {0,  0, 10,  0}   // S:10
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

State currentState = STATE_CALIBRATE;
int destination = 0;
int currentPosition = 0;
int previousPosition = 0;
int orientation = NORTH;
unsigned long turnStartTime = 0;
int turnDirection = 0;
unsigned long qrScanTime = 0;

SoftwareSerial qrSerial(2, 3);

void setup() {
  pinMode(DIR_R_PIN, OUTPUT);
  pinMode(DIR_L_PIN, OUTPUT);
  pinMode(PWR_R_PIN, OUTPUT);
  pinMode(PWR_L_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  qrSerial.begin(9600);
  Serial.begin(9600);
  calibrateSensors();
  beep(1);
}

void loop() {
  int rightSensor = analogRead(SENS_R_PIN);
  int leftSensor = analogRead(SENS_L_PIN);
  bool rightOnLine = rightSensor > lineThreshold;
  bool leftOnLine = leftSensor > lineThreshold;
  
  switch(currentState) {
    case STATE_CALIBRATE:
      if(digitalRead(BTN_PIN)) {
        calibrateSensors();
        beep(2);
        currentState = STATE_WAIT_DESTINATION;
      }
      break;
      
    case STATE_WAIT_DESTINATION:
      if(qrSerial.available()) {
        destination = qrSerial.parseInt();
        if(destination >= 1 && destination <= 11) {
          beep(3);
          currentState = STATE_FOLLOW_LINE;
        }
      }
      break;
      
    case STATE_FOLLOW_LINE:
      handleLineFollowing(rightOnLine, leftOnLine);
      
      if(rightOnLine && leftOnLine) {
        currentState = STATE_AT_INTERSECTION;
        stopMotors();
        beep(1);
      }
      
      if(!rightOnLine && !leftOnLine) {
        currentState = STATE_DEAD_END;
        stopMotors();
        beep(2);
      }
      break;
      
    case STATE_AT_INTERSECTION:
      if(qrSerial.available()) {
        handleQRCode(qrSerial.parseInt());
        if(currentPosition == destination) {
          currentState = STATE_ARRIVED;
          beep(3);
        } else {
          determineNextMove();
        }
      } else if(millis() - qrScanTime > QR_TIMEOUT) {
        turnDirection = 0;
        currentState = STATE_TURNING;
        turnStartTime = millis();
      }
      break;
      
    case STATE_TURNING:
      executeTurn();
      break;
      
    case STATE_DEAD_END:
      if(currentPosition == destination) {
        currentState = STATE_ARRIVED;
        beep(3);
      } else {
        orientation = (orientation + 2) % 4;
        currentState = STATE_REVERSE;
        turnStartTime = millis();
      }
      break;
      
    case STATE_REVERSE:
      reverseAndTurn();
      break;
      
    case STATE_ARRIVED:
      stopMotors();
      if(qrSerial.available()) {
        destination = qrSerial.parseInt();
        if(destination >= 1 && destination <= 11) {
          beep(3);
          currentState = STATE_FOLLOW_LINE;
        }
      }
      break;
  }
  checkQRSerial();
}

void handleQRCode(int newPos) {
  previousPosition = currentPosition;
  currentPosition = newPos;
  qrScanTime = millis();
  
  for(int dir = 0; dir < 4; dir++) {
    if(ROUTE_MAP[previousPosition][dir] == currentPosition) {
      orientation = dir;
      break;
    }
  }
}

void determineNextMove() {
  int possiblePaths[4];
  memcpy(possiblePaths, ROUTE_MAP[currentPosition], sizeof(possiblePaths));
  
  int relativeForward = orientation;
  int relativeRight = (orientation + 1) % 4;
  int relativeLeft = (orientation + 3) % 4;

  if(possiblePaths[relativeForward] == destination) {
    turnDirection = 0;
  } else if(possiblePaths[relativeRight] == destination) {
    turnDirection = 1;
  } else if(possiblePaths[relativeLeft] == destination) {
    turnDirection = -1;
  } else {
    turnDirection = 0;
  }
  
  orientation = (orientation + turnDirection + 4) % 4;
  currentState = STATE_TURNING;
  turnStartTime = millis();
}

void checkQRSerial() {
  if(qrSerial.available()) {
    int newPos = qrSerial.parseInt();
    if(newPos >= 1 && newPos <= 11) {
      handleQRCode(newPos);
      if(currentPosition == destination) {
        currentState = STATE_ARRIVED;
        beep(3);
      }
    }
  }
}

void setMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(DIR_L_PIN, leftSpeed > 0 ? HIGH : LOW);
  digitalWrite(DIR_R_PIN, rightSpeed > 0 ? HIGH : LOW);
  analogWrite(PWR_L_PIN, abs(leftSpeed));
  analogWrite(PWR_R_PIN, abs(rightSpeed));
}

void stopMotors() { setMotors(0, 0); }

void calibrateSensors() {
  lineThreshold = (analogRead(SENS_R_PIN) + analogRead(SENS_L_PIN)) / 2 - 100;
}

void beep(int count) {
  for(int i = 0; i < count; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(BUZZ_DURATION);
    digitalWrite(BUZZER, LOW);
    if(i < count - 1) delay(BUZZ_DURATION);
  }
}

void handleLineFollowing(bool rightOnLine, bool leftOnLine) {
  if(rightOnLine && leftOnLine) {
    setMotors(BASE_SPEED, BASE_SPEED);
  } else if(rightOnLine) {
    setMotors(BASE_SPEED, 0);
  } else if(leftOnLine) {
    setMotors(0, BASE_SPEED);
  } else {
    setMotors(-BASE_SPEED, BASE_SPEED);
  }
}

void executeTurn() {
  if(millis() - turnStartTime < TURN_DURATION) {
    if(turnDirection == 1) {
      setMotors(TURN_SPEED, -TURN_SPEED);
    } else if(turnDirection == -1) {
      setMotors(-TURN_SPEED, TURN_SPEED);
    } else {
      setMotors(BASE_SPEED, BASE_SPEED);
    }
  } else {
    currentState = STATE_FOLLOW_LINE;
  }
}

void reverseAndTurn() {
  if(millis() - turnStartTime < TURN_DURATION) {
    setMotors(-BASE_SPEED, -BASE_SPEED);
  } else {
    currentState = STATE_FOLLOW_LINE;
  }
}
