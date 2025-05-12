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

// Настройки движения
#define BASE_SPEED 80
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
  /* 0 */  {-1, -1, -1, -1},
  /* 1 */  {-1, -1, -1, CROSS_4},
  /* 2 */  {-1, -1, -1, CROSS_4},
  /* 3 */  {-1, -1, -1, CROSS_4},
  /* 4 */  {END_2, CROSS_7, END_3, END_1},
  /* 5 */  {-1, -1, -1, CROSS_7},
  /* 6 */  {-1, -1, -1, CROSS_7},
  /* 7 */  {END_5, CROSS_10, END_6, CROSS_4},
  /* 8 */  {-1, -1, -1, CROSS_10},
  /* 9 */  {-1, -1, -1, CROSS_10},
  /* 10 */ {END_9, END_8, END_11, CROSS_7},
  /* 11 */ {-1, -1, -1, CROSS_10}
};

// Системные переменные
int current_node = -1;
int target_node = 0;
bool calibrated = false;
bool arrived = false;

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
  
  //Serial.print("L:"); Serial.print(cur_color_L);
  //Serial.print(" R:"); Serial.print(cur_color_R);
  //Serial.print(" Th:"); Serial.println(color_gray);

  // Экстренные случаи (как у вас)
  if (cur_color_L > color_gray && cur_color_R > color_gray) {
    // Прямое движение на перекрёстке
    digitalWrite(DIR_R_PIN, 1);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, BASE_SPEED);
    analogWrite(PWR_L_PIN, BASE_SPEED);
    search_time = 0;
    return;
  }

  // Потеря линии (аварийный останов)
  if (cur_color_L < color_gray && cur_color_R < color_gray) {
    search_time++;
    if(search_time >= 10000) {
      end_music();
      stopMotors();
    }
    // Ваш алгоритм поиска линии
    if (last_color_L > color_gray) {
      analogWrite(PWR_L_PIN, 100); // Плавный разворот
      analogWrite(PWR_R_PIN, 0);
    } else {
      analogWrite(PWR_L_PIN, 0);
      analogWrite(PWR_R_PIN, 100);
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
    leftSpeed = BASE_SPEED - 30; // Лёгкий подтормаживающий эффект
  } 
  else if (cur_color_R > color_gray) { // Правее линии
    rightSpeed = BASE_SPEED - 30;
  }

  digitalWrite(DIR_R_PIN, 1);
  digitalWrite(DIR_L_PIN, 1);
  analogWrite(PWR_R_PIN, rightSpeed);
  analogWrite(PWR_L_PIN, leftSpeed);
  
  last_color_L = cur_color_L; // Сохраняем для поиска
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
void turnLeft() {
  unsigned long startTime = millis();
  while(millis() - startTime < TURN_TIME) {
    digitalWrite(DIR_R_PIN, 1);
    digitalWrite(DIR_L_PIN, 0);
    analogWrite(PWR_R_PIN, TURN_SPEED);
    analogWrite(PWR_L_PIN, TURN_SPEED-20);
    
    if(analogRead(SENS_R_PIN) > color_gray) break;
  }
  lineFollowing();
}

void turnRight() {
  unsigned long startTime = millis();
  while(millis() - startTime < TURN_TIME) {
    digitalWrite(DIR_R_PIN, 0);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, TURN_SPEED-20);
    analogWrite(PWR_L_PIN, TURN_SPEED);
    
    if(analogRead(SENS_L_PIN) > color_gray) break;
  }
  lineFollowing();
}

void turnAround() {
  unsigned long startTime = millis();
  while(millis() - startTime < U_TURN_TIME) {
    digitalWrite(DIR_R_PIN, 0);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, TURN_SPEED);
    analogWrite(PWR_L_PIN, TURN_SPEED);

    checkSerial(); // Проверка QR во время разворота!
    if(current_node != -1) break; // Прерывание, если QR считан
  }
  lineFollowing();
}

// ============ Навигация ============
bool checkIntersection() {
  if(color_gray == 0) return false;
  
  static unsigned long lastDetection = 0;
  bool detected = (analogRead(SENS_L_PIN) > color_gray) && 
                 (analogRead(SENS_R_PIN) > color_gray);
  
  if(detected && millis() - lastDetection > 500) {
    lastDetection = millis();
    return true;
  }
  return false;
}

int calculateBestDirection() {
  if(current_node == target_node) return -1;
  
  switch(current_node) {
    case CROSS_4:
      if(target_node == END_2) return LEFT;
      if(target_node == END_3) return RIGHT;
      return STRAIGHT;
      
    case CROSS_7:
      if(target_node == END_5 || target_node == END_6) return LEFT;
      if(target_node == END_8) return RIGHT;
      return STRAIGHT;
      
    case CROSS_10:
      if(target_node == END_9) return LEFT;
      if(target_node == END_11) return RIGHT;
      return BACK;
      
    //default: 
      //return -1;
  }
}

void executeMovement(int direction) {
  switch(direction) {
    case LEFT: turnLeft(); break;
    case RIGHT: turnRight(); break;
    case BACK: turnAround(); break;
    default: lineFollowing();
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
  
  if(target_node == -1) {
    lineFollowing();
  } else {
    navigateToTarget();
  }
  
  checkSerial();
}
