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
#define MAX_NODES 12
#define MAX_EDGES 4
#define BASE_SPEED 85
#define TURN_SPEED 120
#define TURN_DURATION 2000
#define BUZZ_DURATION 200
#define STOP_DURATION 1000
#define DECELERATION_INTERVAL 700    // Интервал замедления в мс
#define DECELERATION_STEP 7          // Шаг уменьшения скорости
#define MIN_SPEED 45                 // Минимальная скорость
#define ACCELERATION_INTERVAL 500   // Интервал для обратного разгона
#define DEAD_END_TURN_DURATION 2500  // Время для разворота в тупике

// Обновленные настройки для плавных поворотов
#define TURN_RATIO 0.6f          // Соотношение скоростей моторов при повороте
#define TURN_BASE_DURATION 1500  // Базовое время поворота (на 90 градусов)
#define TURN_SPEED_MAIN 120      // Основная скорость поворота
#define TURN_SPEED_SECONDARY 75  // Вспомогательная скорость

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
  STATE_STOP_AT_INTERSECTION,  // Новое состояние остановки
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
int currentSpeed = BASE_SPEED;
unsigned long lastDecelerationTime = 0;
unsigned long lastAccelerationTime = 0;
bool acceleratingBack = false;
unsigned long turnDuration = TURN_DURATION;

// Добавляем флаг, чтобы можно было включать и отключать ускорение и замедление
bool enableSpeedControl = false;  // Разрешение на управление скоростью

// Структура для хранения пути
struct Path {
  int next_node;
  int direction;
};

// Поиск кратчайшего пути (упрощенный BFS)
Path findShortestPath(int start, int target) {
  // Массив посещенных узлов
  bool visited[MAX_NODES] = { false };
  // Очередь для BFS: {current_node, previous_direction}
  int queue[MAX_NODES * 2];
  int front = 0, rear = 0;

  // Храним предыдущий узел и направление
  int parent[MAX_NODES] = { -1 };
  int directions[MAX_NODES] = { -1 };

  // Инициализация
  queue[rear++] = start;
  queue[rear++] = -1;  // Направление для стартового узла отсутствует
  visited[start] = true;

  // Обход в ширину
  while (front < rear) {
    int current = queue[front++];
    int prevDir = queue[front++];

    // Проверка цели
    if (current == target) {
      // Восстановление пути от цели к старту
      int node = target;
      while (parent[node] != start && node != -1) {
        node = parent[node];
      }
      if (node == -1) return { -1, -1 };
      return { target, directions[node] };
    }

    // Проверка направлений в порядке: N, E, S, W
    for (int dir = 0; dir < 4; dir++) {
      int next = ROUTE_MAP[current][dir];
      if (next != 0 && !visited[next]) {
        visited[next] = true;
        parent[next] = current;
        directions[next] = dir;
        queue[rear++] = next;
        queue[rear++] = dir;
      }
    }
  }

  return { -1, -1 };  // Путь не найден
}


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

  // Сброс параметров скорости при обнаружении QR-кода
  lastDecelerationTime = millis();
  acceleratingBack = false;
  currentSpeed = BASE_SPEED;

  // Определение ориентации
  for (int dir = 0; dir < 4; dir++) {
    if (ROUTE_MAP[previousPosition][dir] == currentPosition) {
      orientation = dir % 4;  //защита от превышения
      break;
    }
  }

  // Переход в состояние перекрестка ТОЛЬКО через QR-код
  if ((currentPosition == 4 || currentPosition == 7 || currentPosition == 10) && currentState != STATE_WAIT_DESTINATION) {  //если нет цели, перекрёсток или нет - неважно
    currentSpeed = BASE_SPEED;                                                                                              // Сброс скорости
    currentState = STATE_STOP_AT_INTERSECTION;
    Serial.println("Intersection detected via QR");
  }

  Serial.print("Position: ");
  Serial.print(currentPosition);
  Serial.print(" | Orientation: ");
  printDirection(orientation);

  if (currentPosition == destination) {
    currentState = STATE_ARRIVED;
    Serial.println("Route completed");
    beep(3);
  }
}

void printDirection(int dir) {
  const char* directions[] = { "NORTH", "EAST", "SOUTH", "WEST" };
  Serial.println(directions[(dir % 4 + 4) % 4]);  // Гарантированный диапазон 0-3
}

void handleSensors(int rightSensor, int leftSensor) {
  // Игнорировать датчики во время поворотов и на перекрестках
  if (currentState == STATE_TURNING || currentState == STATE_AT_INTERSECTION) return;

  bool rightOnLine = rightSensor > lineThreshold;
  bool leftOnLine = leftSensor > lineThreshold;

  switch (currentState) {
    case STATE_FOLLOW_LINE:
      // Механизм плавного замедления
      if (enableSpeedControl && millis() - lastDecelerationTime > DECELERATION_INTERVAL) {
        if (currentSpeed > MIN_SPEED) {
          currentSpeed = max(currentSpeed - DECELERATION_STEP, MIN_SPEED);
        }
        lastDecelerationTime = millis();
      }

      // Механизм обратного разгона
      if (currentSpeed <= MIN_SPEED && !acceleratingBack) {
        lastAccelerationTime = millis();
        acceleratingBack = true;
      }

      if (enableSpeedControl && acceleratingBack && (millis() - lastAccelerationTime > ACCELERATION_INTERVAL)) {
        currentSpeed = BASE_SPEED;
        acceleratingBack = false;
      }

      if (rightOnLine && leftOnLine) {
        // Оба датчика на линии - активируем управление скоростью
        enableSpeedControl = true;
        setMotors(currentSpeed, currentSpeed);
        lineLost = false;

      } else if (rightOnLine) {
        setMotors(BASE_SPEED, 0);
        enableSpeedControl = false;
        currentSpeed = BASE_SPEED;
        lineLost = false;

      } else if (leftOnLine) {
        setMotors(0, BASE_SPEED);
        enableSpeedControl = false;
        currentSpeed = BASE_SPEED;
        lineLost = false;

      } else {
        if (!lineLost) {
          enableSpeedControl = false;
          currentSpeed = BASE_SPEED;
          lineLost = true;
          lineLostTime = millis();
        }
        if (millis() - lineLostTime > 5000) {
          setMotors(BASE_SPEED, TURN_SPEED);
        }
      }
      break;
  }
}

void runStateMachine() {
  switch (currentState) {
    case STATE_CALIBRATE:
      if (lineThreshold == 0) {
        calibrateSensors();
        currentState = STATE_WAIT_DESTINATION;
        Serial.println("Calibration complete");
      } else {
        currentState = STATE_WAIT_DESTINATION;
      }
      break;

    case STATE_WAIT_DESTINATION:
      if (destination != 0) {
        currentState = STATE_FOLLOW_LINE;
        Serial.println("Starting navigation");
      }
      break;

    case STATE_FOLLOW_LINE:  // Убрана проверка позиции в FOLLOW_LINE - перекрестки только через QR
      // При возврате в режим следования сбрасываем скорость
      if (!enableSpeedControl) {
        currentSpeed = BASE_SPEED;
        enableSpeedControl = true;
        lastDecelerationTime = millis();
      }
      break;

    case STATE_STOP_AT_INTERSECTION:
      stopMotors();
      if (millis() - turnStartTime > STOP_DURATION) {
        if (currentPosition == destination) {
          currentState = STATE_ARRIVED;
          Serial.println("Destination reached");
        } else {
          determineNextMove();
          turnStartTime = millis();   // Критичный сброс таймера
          currentSpeed = BASE_SPEED;  // Сброс скорости перед поворотом
          currentState = STATE_TURNING;
          Serial.println("Starting navigation turn");
        }
      }
      break;

    case STATE_AT_INTERSECTION:
      // Переносим логику в STATE_STOP_AT_INTERSECTION
      currentState = STATE_STOP_AT_INTERSECTION;
      turnStartTime = millis();  // Засекаем время остановки
      break;

    case STATE_TURNING:
      {
        static bool turnMessageSent = false;
        // При повороте жестко фиксируем скорость
        enableSpeedControl = false;
        if (!turnMessageSent) {
          Serial.print("Turning ");
          // Четкое определение типа поворота
          switch (turnDirection) {
            case 0: Serial.println("STRAIGHT (line follow)"); break;
            case 1: Serial.println("RIGHT"); break;
            case -1: Serial.println("LEFT"); break;
            case 2: Serial.println("U-TURN"); break;
          }
          turnMessageSent = true;
          turnStartTime = millis();

          // Автоматический расчет времени поворота
          int turnMultiplier = (turnDirection == 2) ? 2 : 1;
          turnDuration = TURN_BASE_DURATION * turnMultiplier;
        }

        // Особый случай: движение прямо с использованием алгоритма следования
        if (turnDirection == 0) {
          handleSensors(analogRead(SENS_R_PIN), analogRead(SENS_L_PIN));
          currentSpeed = BASE_SPEED;
        }
        // Управление моторами для поворотов
        else {
          if (turnDirection == 1) {
            setMotors(TURN_SPEED_MAIN, TURN_SPEED_SECONDARY);  // Направо
          } else if (turnDirection == -1) {
            setMotors(TURN_SPEED_SECONDARY, TURN_SPEED_MAIN);  // Налево
          } else if (turnDirection == 2) {
            setMotors(TURN_SPEED_MAIN, -TURN_SPEED_MAIN);  // Разворот
          }
        }

        // Плавный переход к линии
        if (millis() - turnStartTime > turnDuration * 0.9) {
          // Начинаем постепенно включать следящий алгоритм
          handleSensors(analogRead(SENS_R_PIN), analogRead(SENS_L_PIN));
        }

        if (millis() - turnStartTime >= TURN_DURATION) {
          stopMotors();
          currentState = STATE_FOLLOW_LINE;
          turnMessageSent = false;
          Serial.println("Turn finalized");

          // Принудительный сброс скорости после поворота
          currentSpeed = BASE_SPEED;
        }
      }
      break;

    case STATE_DEAD_END:
      orientation = (orientation + 2) % 4;
      currentState = STATE_REVERSE;
      turnStartTime = millis();
      Serial.println("Dead end detected");
      // Специальный разворот
      setMotors(-BASE_SPEED, BASE_SPEED);  // Разворот на месте
      break;

    case STATE_REVERSE:
      if (millis() - turnStartTime >= TURN_DURATION) {
        stopMotors();
        // После разворота ищем новый путь
        determineNextMove();
        currentState = STATE_TURNING;
        turnStartTime = millis();
        Serial.println("Dead end turn completed");
      }
      break;

    case STATE_ARRIVED:
      stopMotors();
      break;
  }
}


// Вспомогательная функция для создания отступов
String createIndent(int depth) {
  String indent = "";
  for (int i = 0; i < depth; i++) {
    indent += "  ";  // 2 пробела на уровень глубины
  }
  return indent;
}


// Модифицированная функция determineNextMove
void determineNextMove() {
  Serial.println("\n=== Планирование маршрута ===");
  Path result = findShortestPath(currentPosition, destination);

  if (result.next_node == -1) {
    turnDirection = 2;
    Serial.println("Путь не найден! Разворот.");
  } else {
    // Абсолютное направление из карты
    int requiredDir = result.direction;
    // Преобразование абсолютного направления в относительное
    turnDirection = (result.direction - orientation + 4) % 4;
    if (turnDirection == 3) turnDirection = -1;  // Налево

    Serial.print("Направление: ");
    Serial.println(result.direction);
    // Обновление ориентации
    orientation = requiredDir;
  }

  // Обновление ориентации
  //orientation = (orientation + turnDirection + 4) % 4;
  Serial.print("Требуемый поворот: ");
  switch (turnDirection) {
    case 0: Serial.println("ПРЯМО"); break;
    case 1: Serial.println("НАПРАВО"); break;
    case -1: Serial.println("НАЛЕВО"); break;
    case 2: Serial.println("РАЗВОРОТ"); break;
  }
  Serial.print("Новая ориентация: ");
  printDirection(orientation);
  Serial.println("=============================");
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
