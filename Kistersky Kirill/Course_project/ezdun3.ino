// Конфигурация оборудования
#define DIR_R_PIN 4
#define DIR_L_PIN 7
#define PWR_R_PIN 5
#define PWR_L_PIN 6
#define SENS_R_PIN A0
#define SENS_L_PIN A1
#define BTN_PIN 8
#define BUZZER 11

// Направления движения
#define LEFT 0
#define STRAIGHT 1
#define RIGHT 2
#define BACK 3

// Идентификаторы точек
enum {
  END_1 = 1,
  END_2 = 2,
  END_3 = 3,
  CROSS_4 = 4,
  END_5 = 5,
  END_6 = 6,
  CROSS_7 = 7,
  END_8 = 8,
  END_9 = 9,
  CROSS_10 = 10,
  END_11 = 11
};

// Добавляем новый enum для относительных направлений
enum MovementDirection {
  MOVE_FORWARD,
  MOVE_BACKWARD,
  MOVE_LEFT,
  MOVE_RIGHT
};


// Настройки движения
#define BASE_SPEED 100
#define TURN_SPEED 100
#define MAX_SPEED 150
#define TURN_TIME 350
#define U_TURN_TIME 700

// Переменные для датчиков линии
typedef unsigned int light;
light cur_color_L, cur_color_R, color_gray, last_color_L, color_black, color_white;
unsigned long search_time = 0;

// Карта маршрутов
const int ROUTE_MAP[12][4] = {
  /* 0 */  {0, 0, 0, 0},
  /* 1 */  {0, 0, 0, BACK},
  /* 2 */  {0, 0, 0, BACK},
  /* 3 */  {0, 0, 0, BACK},
  /* 4 */  {END_2, CROSS_7, END_3, END_1},
  /* 5 */  {0, 0, 0, BACK},
  /* 6 */  {0, 0, 0, BACK},
  /* 7 */  {END_5, CROSS_10, END_6, CROSS_4},
  /* 8 */  {0, 0, 0, BACK},
  /* 9 */  {0, 0, 0, BACK},
  /* 10 */ {END_9, END_8, END_11, CROSS_7},
  /* 11 */ {0, 0, 0, BACK}
};

// Системные переменные
int current_node = -1;
int target_node = -1;
bool calibrated = false;
bool arrived = false;
MovementDirection current_direction = MOVE_FORWARD;

void pin_init() {
  pinMode(DIR_R_PIN, OUTPUT);
  pinMode(DIR_L_PIN, OUTPUT);
  pinMode(PWR_R_PIN, OUTPUT);
  pinMode(PWR_L_PIN, OUTPUT);
  pinMode(SENS_R_PIN, INPUT);
  pinMode(SENS_L_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
}

void beep(int count) {
  for(int i = 0; i < count; i++) {
    tone(BUZZER, 2000, 200);
    delay(300);
  }
}

void stopMotors() {
  analogWrite(PWR_R_PIN, 0);
  analogWrite(PWR_L_PIN, 0);
  }

// ============ Движение по линии ============
void lineFollowing() {
  // Чтение датчиков с отладкой
  cur_color_L = analogRead(SENS_L_PIN);
  cur_color_R = analogRead(SENS_R_PIN);

  // Экстренные случаи (как у вас)
  if (cur_color_L > color_gray && cur_color_R > color_gray) {
    // Прямое движение на перекрёстке
    digitalWrite(DIR_R_PIN, 1);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, BASE_SPEED);
    analogWrite(PWR_L_PIN, BASE_SPEED);
    current_direction = MOVE_FORWARD;
    search_time = 0;
    return;
  }

  // Потеря линии (аварийный останов)
  if (cur_color_L < color_gray && cur_color_R < color_gray) {
    search_time++;
    if(search_time >= 10000) {
      end_music();
      stopMotors();
      return;
    }

    if (last_color_L > color_gray) {
      analogWrite(PWR_L_PIN, 120); // Плавный разворот
      analogWrite(PWR_R_PIN, 80);
    } else {
      analogWrite(PWR_L_PIN, 80);
      analogWrite(PWR_R_PIN, 120);
    }
    return;
  }

  // Гибридный PID (упрощённый)
  static int last_error = 0;
  int error = cur_color_L - cur_color_R;
  int correction = error * 0.5 + (error - last_error) * 0.3; // P + D
  last_error = error;

  // Применяем коррекцию
  int leftSpeed  = constrain(BASE_SPEED + correction, 0, MAX_SPEED);
  int rightSpeed = constrain(BASE_SPEED - correction, 0, MAX_SPEED);

  // Движение с плавностью PID, но с вашей логикой направлений
  if (cur_color_L > color_gray) { // Левее линии
    leftSpeed = BASE_SPEED - 5; // Лёгкий подтормаживающий эффект
  } 
  else if (cur_color_R > color_gray) { // Правее линии
    rightSpeed = BASE_SPEED - 5;
  }

  digitalWrite(DIR_R_PIN, 1);
  digitalWrite(DIR_L_PIN, 1);
  analogWrite(PWR_R_PIN, rightSpeed);
  analogWrite(PWR_L_PIN, leftSpeed);
  
  last_color_L = cur_color_L; // Сохраняем для поиска
  search_time = 0;
}

void end_music() {
  tone(BUZZER, 261.63, 600);
  delay(600);
  tone(BUZZER, 329.63, 600);
  delay(600);
  tone(BUZZER, 392, 600);
  delay(600);
  noTone(BUZZER);
}

// ============ Улучшенные повороты ============
void turnRelative(bool turnRight) {
  unsigned long startTime = millis();
  bool actualTurnRight = turnRight;
  
  if(current_direction == MOVE_BACKWARD) {
    actualTurnRight = !actualTurnRight;
  }
  
  while(millis() - startTime < TURN_TIME) {
    if(actualTurnRight) {
      digitalWrite(DIR_R_PIN, 0);
      digitalWrite(DIR_L_PIN, 1);
      analogWrite(PWR_R_PIN, TURN_SPEED-20);
      analogWrite(PWR_L_PIN, TURN_SPEED);
    } else {
      digitalWrite(DIR_R_PIN, 1);
      digitalWrite(DIR_L_PIN, 0);
      analogWrite(PWR_R_PIN, TURN_SPEED);
      analogWrite(PWR_L_PIN, TURN_SPEED-20);
    }
    
    if((actualTurnRight && analogRead(SENS_L_PIN) > color_gray) ||
       (!actualTurnRight && analogRead(SENS_R_PIN) > color_gray)) {
      break;
    }
    
    checkSerial();
    if(current_node == target_node) break;
  }
  
  // Обновляем текущее направление
  if(turnRight) {
    current_direction = static_cast<MovementDirection>((current_direction + 1) % 4);
  } else {
    current_direction = static_cast<MovementDirection>((current_direction + 3) % 4);
  }
}

void turnLeft() {
  turnRelative(false);
}

void turnRight() {
  turnRelative(true);
}

void turnAround() {
  turnRelative(false);
  delay(100);
  turnRelative(false);
}

void moveForward() {
  digitalWrite(DIR_R_PIN, 1);
  digitalWrite(DIR_L_PIN, 1);
  current_direction = MOVE_FORWARD;
}

void moveBackward() {
  digitalWrite(DIR_R_PIN, 0);
  digitalWrite(DIR_L_PIN, 0);
  current_direction = MOVE_BACKWARD;
}


// ============ Навигация ============
bool checkIntersection() {
  if(color_gray == 0) return false;
  
  static unsigned long lastDetection = 0;
  bool detected = (analogRead(SENS_L_PIN) > color_gray) && 
                 (analogRead(SENS_R_PIN) > color_gray);
  
  if(detected && millis() - lastDetection > 300) {
    lastDetection = millis();
    return true;
  }
  return false;
}

int calculateBestDirection() {
  if(current_node == -1) return -1;
  if(current_node == target_node) return -1;

  // Для всех END-точек - только разворот
  if(current_node >= END_1 && current_node <= END_11 && 
     current_node != CROSS_4 && current_node != CROSS_7 && current_node != CROSS_10) {
    return BACK;
  }
  
  // Определяем направление относительно текущего положения
  switch(current_node) {
    case CROSS_4:
      if(target_node == END_1) return (current_direction == MOVE_BACKWARD) ? STRAIGHT : BACK;
      if(target_node == END_2) return LEFT;
      if(target_node == END_3) return RIGHT;
      if(target_node == CROSS_7) return STRAIGHT;
      break;
      
    case CROSS_7:
      if(target_node == END_5 || target_node == END_6) return LEFT;
      if(target_node == END_8) return RIGHT;
      if(target_node == CROSS_4) return (current_direction == MOVE_FORWARD) ? BACK : STRAIGHT;
      if(target_node == CROSS_10) return STRAIGHT;
      break;
      
    case CROSS_10:
      if(target_node == END_9) return LEFT;
      if(target_node == END_11) return RIGHT;
      if(target_node == END_8) return STRAIGHT;
      if(target_node == CROSS_7) return (current_direction == MOVE_FORWARD) ? BACK : STRAIGHT;
      break;
  }
  
  return -1;
}

void executeMovement(int direction) {
  switch(direction) {
    case LEFT: 
      turnLeft(); 
      break;
    case RIGHT: 
      turnRight(); 
      break;
    case BACK: 
      if(current_direction == MOVE_FORWARD) {
        turnAround();
      } else {
        moveForward();
      }
      break;
    case STRAIGHT: 
      if(current_direction == MOVE_BACKWARD) {
        moveBackward();
      } else {
        moveForward();
      }
      break;
    default: 
      stopMotors();
  }
}

void processQR(String data) {
  if (data.length() > 0 && data[0] >= '0' && data[0] <= '9') {
    int node = data.toInt();
    if (current_node == node) {
      Serial.print("QR detected same QR: ");
      printNodeName(current_node);
    }

    else if (node >= 1 && node <= 11) {
      current_node = node;
      Serial.print("QR detected: ");
      printNodeName(current_node);
      
      if (current_node == target_node) {
        arrived = true;
        beep(3);
      } else {
        arrived = false;
        beep(1);
      }
    }
  }
}

void checkSerial() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    
    // Если это QR-код (число от 1 до 11)
    if (data.length() > 0 && data[0] >= '0' && data[0] <= '9') {
      processQR(data);
    }
    // Если это команда (например, "GO5", "STOP")
    else if (data.startsWith("GO")) {
      int node = data.substring(2).toInt();
      if (node >= 1 && node <= 11) {
        target_node = node;
        arrived = false;
        Serial.print("New target: ");
        printNodeName(target_node);
      }
    }
    else if (data == "STOP") {
      target_node = -1;
      arrived = true;
      stopMotors();
      Serial.println("Navigation stopped");
    }
  }
}

void navigateToTarget() {
  if(current_node == -1) {
    Serial.println("Current node unknown - waiting for QR");
    stopMotors();
    return;
  }

  if(current_node == target_node) {
    arrived = true;
    beep(3);
    stopMotors();
    return;
  }

  int direction = calculateBestDirection();
  /*if(direction == -1) {
    // Если нет пути — разворот и сброс текущего узла
    Serial.println("No path: U-turn and reset");
    turnAround();
    current_node = -1; // Сброс для повторного чтения QR
    return;
  }*/

  executeMovement(direction);

  unsigned long startTime = millis();
  while(!checkIntersection() && millis() - startTime < 5000) {
    lineFollowing();
    checkSerial(); //Проверяем QR даже в движении!
    if(current_node == target_node) break;
  }
  
  if(checkIntersection()) {
    delay(200);
  }
}

void printNodeName(int node) {
  switch(node) {
    case END_1: Serial.println("END_1"); break;
    case END_2: Serial.println("END_2"); break;
    case END_3: Serial.println("END_3"); break;
    case CROSS_4: Serial.println("CROSS_4"); break;
    case END_5: Serial.println("END_5"); break;
    case END_6: Serial.println("END_6"); break;
    case CROSS_7: Serial.println("CROSS_7"); break;
    case END_8: Serial.println("END_8"); break;
    case END_9: Serial.println("END_9"); break;
    case CROSS_10: Serial.println("CROSS_10"); break;
    case END_11: Serial.println("END_11"); break;
    default: Serial.println(node);
  }
}

void setup() {
  pin_init();
  Serial.begin(9600);
  
  Serial.println("Calibration started");
  
  // 1. Белая поверхность (первая!)
  Serial.println("1. Place on WHITE surface and press button");
  while(digitalRead(BTN_PIN) == LOW);
  color_white = (analogRead(SENS_L_PIN) + analogRead(SENS_R_PIN)) / 2;
  beep(1);
  
  delay(1000);
  
  // 2. Черная линия
  Serial.println("2. Place on BLACK line and press button");
  while(digitalRead(BTN_PIN) == LOW);
  color_black = (analogRead(SENS_L_PIN) + analogRead(SENS_R_PIN)) / 2;
  beep(2);
  
  // Проверка калибровки
  if(color_white >= color_black || abs(color_white - color_black) < 100) {
    Serial.println("CALIBRATION ERROR!");
    while(1) { beep(4); delay(1000); }
  }
  color_gray = (color_white + color_black) / 2;
  
  Serial.println("Calibration results:");
  Serial.print("White: "); Serial.println(color_white);
  Serial.print("Black: "); Serial.println(color_black);
  Serial.print("Threshold: "); Serial.println(color_gray);
  
  Serial.println("System READY");
}

void loop() {
  if(arrived) {
    stopMotors();
    delay(100);
    return;
  }
  

  navigateToTarget();
  checkSerial();
}
