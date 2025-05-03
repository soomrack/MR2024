// --- Конфигурация оборудования ---
#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9
#define ENB 10
#define IR_LEFT A0
#define IR_RIGHT A1
#define CALIB_BTN 2
#define BUZZER 3

// Карта перекрестков
const int INTERSECTIONS[][4] = {
  {-1, 1, -1, -1},   // Узел 0
  {2, -1, 0, -1},    // Узел 1
  {-1, 3, 1, -1},    // Узел 2
  {4, -1, -1, 2},    // Узел 3
  {-1, -1, 3, -1}    // Узел 4
};

// Настройки ПИД-регулятора
float Kp = 0.8, Ki = 0.01, Kd = 0.1;
float error = 0, lastError = 0, integral = 0;

int current_pos = 0;
int target_pos = -1;
int threshold_left, threshold_right;
bool calibrated = false;
unsigned long lastIntersectionTime = 0;

// Структура для очереди
const int QUEUE_SIZE = 10;
int queue[QUEUE_SIZE];
int front = 0, rear = -1, itemCount = 0;

void setup() {
  pinMode(CALIB_BTN, INPUT_PULLUP);
  initHardware();
  while (!calibrated) calibration();
  Serial.begin(9600);
  Serial.println("System READY");
  Serial.print("Current node: "); Serial.println(current_pos);
}

void loop() {
  if (target_pos == -1) {
    lineFollowing();
  } else {
    navigateToTarget();
  }
  
  checkSerial();
  
  if (target_pos != -1 && millis() - lastIntersectionTime > 10000) {
    Serial.println("ERROR: Navigation failed - timeout");
    stopMotors();
    target_pos = -1;
  }
}

// Функции очереди
void enqueue(int value) {
  if (itemCount < QUEUE_SIZE) {
    if (rear == QUEUE_SIZE-1) rear = -1;
    queue[++rear] = value;
    itemCount++;
  }
}

int dequeue() {
  int value = queue[front++];
  if (front == QUEUE_SIZE) front = 0;
  itemCount--;
  return value;
}

bool isEmpty() {
  return itemCount == 0;
}

// Навигация
int findNextStep(int from, int to) {
  bool visited[5] = {false};
  int parent[5] = {-1};
  
  front = 0;
  rear = -1;
  itemCount = 0;
  
  enqueue(from);
  visited[from] = true;
  
  while (!isEmpty()) {
    int current = dequeue();
    
    if (current == to) break;
    
    for (int i = 0; i < 4; i++) {
      int neighbor = INTERSECTIONS[current][i];
      if (neighbor != -1 && !visited[neighbor]) {
        visited[neighbor] = true;
        parent[neighbor] = current;
        enqueue(neighbor);
      }
    }
  }
  
  if (!visited[to]) return -1;
  
  int step = to;
  while (parent[step] != from && parent[step] != -1) {
    step = parent[step];
  }
  
  return step;
}

void navigateToTarget() {
  if (current_pos == target_pos) {
    target_pos = -1;
    Serial.print("ARRIVED at node: "); Serial.println(current_pos);
    beep(2);
    return;
  }

  int next_step = findNextStep(current_pos, target_pos);
  if (next_step == -1) {
    Serial.println("ERROR: No path found");
    target_pos = -1;
    return;
  }

  int direction = -1;
  for (int i = 0; i < 4; i++) {
    if (INTERSECTIONS[current_pos][i] == next_step) {
      direction = i;
      break;
    }
  }

  switch (direction) {
    case 0: moveNorth(); break;
    case 1: moveEast(); break;
    case 2: moveSouth(); break;
    case 3: moveWest(); break;
    default: 
      Serial.println("ERROR: Invalid direction");
      stopMotors();
      target_pos = -1;
      return;
  }

  if (checkIntersection()) {
    lastIntersectionTime = millis();
    current_pos = next_step;
    Serial.print("Reached node: "); Serial.println(current_pos);
    beep(1);
  }
}

// Движение по линии
void lineFollowing() {
  int left = analogRead(IR_LEFT) > threshold_left;
  int right = analogRead(IR_RIGHT) > threshold_right;

  if (!left && !right) {
    searchLine();
    return;
  }

  error = left - right;
  integral += error;
  float derivative = error - lastError;
  lastError = error;
  
  int correction = Kp * error + Ki * integral + Kd * derivative;
  setMotors(150 - correction, 150 + correction);
}

void searchLine() {
  for (int i = 1; i <= 3; i++) {
    setMotors(100, -100); delay(200 * i);
    if (checkSensors()) return;
    setMotors(-100, 100); delay(400 * i);
    if (checkSensors()) return;
  }
  stopMotors();
}

bool checkSensors() {
  return analogRead(IR_LEFT) > threshold_left || analogRead(IR_RIGHT) > threshold_right;
}

// Обработка команд
void checkSerial() {
  if (Serial.available()) {
    String qr = Serial.readStringUntil('\n');
    qr.trim();
    processQR(qr);
    Serial.println("ACK"); // Подтверждение получения
  }
}

void processQR(String data) {
  if (data.length() == 1 && data[0] >= '1' && data[0] <= '4') {
    int node = data.toInt();
    
    if (target_pos == -1) {
      current_pos = node;
      Serial.print("Current position updated to: "); Serial.println(node);
      beep(1);
    } else {
      if (node == current_pos) {
        Serial.print("At node: "); Serial.println(node);
      } else {
        Serial.print("WRONG NODE! Expected: "); Serial.print(current_pos);
        Serial.print(", got: "); Serial.println(node);
      }
    }
  }
  else if (data == "GO1") target_pos = 1;
  else if (data == "GO2") target_pos = 2;
  else if (data == "GO3") target_pos = 3;
  else if (data == "GO4") target_pos = 4;
  else if (data == "STOP") target_pos = -1;
  
  if (target_pos != -1) {
    Serial.print("Navigating to node: "); Serial.println(target_pos);
    lastIntersectionTime = millis();
  } else if (data == "STOP") {
    Serial.println("Navigation stopped");
  }
}

// Управление моторами
void moveNorth() {
  setMotors(150, 150);
}

void moveEast() {
  setMotors(100, -100); 
  delay(500);
  setMotors(150, 150);
}

void moveSouth() {
  setMotors(-100, 100); 
  delay(1000);
  setMotors(150, 150);
}

void moveWest() {
  setMotors(-100, 100);
  delay(500);
  setMotors(150, 150);
}

bool checkIntersection() {
  return (analogRead(IR_LEFT) > threshold_left && 
          analogRead(IR_RIGHT) > threshold_right);
}

void initMotors() {
  for (int pin = 5; pin <= 10; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void setMotors(int left, int right) {
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);
  
  digitalWrite(IN1, left > 0 ? HIGH : LOW);
  digitalWrite(IN2, left < 0 ? HIGH : LOW);
  digitalWrite(IN3, right > 0 ? HIGH : LOW);
  digitalWrite(IN4, right < 0 ? HIGH : LOW);
  analogWrite(ENA, abs(left));
  analogWrite(ENB, abs(right));
}

void stopMotors() {
  setMotors(0, 0);
}

// Вспомогательные функции
void initHardware() {
  pinMode(BUZZER, OUTPUT);
  initMotors();
}

void calibration() {
  if (digitalRead(CALIB_BTN) == LOW) {
    threshold_left = (analogRead(IR_LEFT) + 100) / 2;
    threshold_right = (analogRead(IR_RIGHT) + 100) / 2;
    calibrated = true;
    Serial.println("Sensors calibrated");
    beep(2);
    delay(500);
  }
}

void beep(int count) {
  for (int i = 0; i < count; i++) {
    tone(BUZZER, 2000, 200);
    delay(300);
  }
}
