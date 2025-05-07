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

// Переменные для датчиков линии
typedef unsigned int light;
light cur_color_L, cur_color_R, color_gray, last_color_L;
unsigned long search_time = 0;

// Карта маршрутов
const int ROUTE_MAP[12][4] = {
  /* 0 */  {-1, -1, -1, -1},
  /* 1 */  {-1, -1, -1, -1},
  /* 2 */  {-1, -1, -1, CROSS_4},
  /* 3 */  {-1, -1, -1, CROSS_4},
  /* 4 */  {END_2, CROSS_7, END_3, -1},
  /* 5 */  {-1, -1, -1, CROSS_7},
  /* 6 */  {-1, -1, -1, CROSS_7},
  /* 7 */  {END_5, CROSS_10, END_8, CROSS_4},
  /* 8 */  {-1, -1, -1, CROSS_7},
  /* 9 */  {-1, -1, -1, CROSS_10},
  /* 10 */ {END_9, -1, END_11, CROSS_7},
  /* 11 */ {-1, -1, -1, CROSS_10}
};

// Системные переменные
int current_node = CROSS_4;
int target_node = CROSS_10;
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

void color_ident(light &color) {
  light sens_R = analogRead(SENS_R_PIN);
  light sens_L = analogRead(SENS_L_PIN);
  color = (sens_R + sens_L) / 2;
}

// ============ Движение по линии ============
void lineFollowing() {
  cur_color_L = analogRead(SENS_L_PIN);
  cur_color_R = analogRead(SENS_R_PIN);

  // Для отладки выводим значения датчиков
  Serial.print("L: "); Serial.print(cur_color_L);
  Serial.print(" R: "); Serial.print(cur_color_R);
  Serial.print(" Th: "); Serial.println(color_gray);

  // Движение вперед (оба датчика на черном)
  if (cur_color_L > color_gray && cur_color_R > color_gray) {
    search_time = 0;
    digitalWrite(DIR_R_PIN, 1);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, 80);
    analogWrite(PWR_L_PIN, 80);
  }
  // Поворот налево (левый на черном, правый на белом)
  else if (cur_color_L > color_gray && cur_color_R < color_gray) {
    search_time = 0;
    digitalWrite(DIR_R_PIN, 1);
    digitalWrite(DIR_L_PIN, 0);
    analogWrite(PWR_R_PIN, 150);
    analogWrite(PWR_L_PIN, 100);
  }
  // Поворот направо (левый на белом, правый на черном)
  else if (cur_color_L < color_gray && cur_color_R > color_gray) {
    search_time = 0;
    digitalWrite(DIR_R_PIN, 0);
    digitalWrite(DIR_L_PIN, 1);
    analogWrite(PWR_R_PIN, 100);
    analogWrite(PWR_L_PIN, 150);
  }
  // Поиск линии (оба датчика на белом)
  else {
    search_time++;
    if (last_color_L > color_gray) {
      digitalWrite(DIR_L_PIN, 0);
      analogWrite(PWR_L_PIN, 100); 
      digitalWrite(DIR_R_PIN, 0);
      analogWrite(PWR_R_PIN, 0);
    } else {
      digitalWrite(DIR_L_PIN, 0);
      analogWrite(PWR_L_PIN, 0);
      digitalWrite(DIR_R_PIN, 0);
      analogWrite(PWR_R_PIN, 100);
    }
    
    if (search_time >= 10000) {
      stopMotors();
      end_music();
    }
  }
  
  last_color_L = cur_color_L;
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

// ============ Навигация ============
void turnLeft() {
  digitalWrite(DIR_R_PIN, 1);
  digitalWrite(DIR_L_PIN, 0);
  analogWrite(PWR_R_PIN, 100);
  analogWrite(PWR_L_PIN, 100);
  delay(300);
  lineFollowing();
  arrived = false;
}

void turnRight() {
  digitalWrite(DIR_R_PIN, 0);
  digitalWrite(DIR_L_PIN, 1);
  analogWrite(PWR_R_PIN, 100);
  analogWrite(PWR_L_PIN, 100);
  delay(300);
  lineFollowing();
  arrived = false;
}

void turnAround() {
  digitalWrite(DIR_R_PIN, 0);
  digitalWrite(DIR_L_PIN, 1);
  analogWrite(PWR_R_PIN, 100);
  analogWrite(PWR_L_PIN, 100);
  delay(500);
  lineFollowing();
  arrived = false;
}

void moveForward() {
  lineFollowing();
}

void executeMovement(int direction) {
  switch(direction) {
    case LEFT: turnLeft(); break;
    case RIGHT: turnRight(); break;
    case BACK: turnAround(); break;
    default: moveForward();
  }
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
      
    default: return BACK;
  }
}

bool checkIntersection() {
  return (analogRead(SENS_L_PIN) > color_gray && 
         analogRead(SENS_R_PIN) > color_gray);
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
  if (current_node == target_node) {
    Serial.print("ARRIVED at: ");
    printNodeName(current_node);
    arrived = true;
    beep(3);
    return;
  }

  int direction = calculateBestDirection();
  if (direction == -1) {
    Serial.println("ERROR: No path");
    stopMotors();
    return;
  }

  executeMovement(direction);

  // Проверяем перекрёсток (QR будет обработан в checkSerial())
  if (checkIntersection()) {
    delay(100); // Даём время на считывание QR
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
  
  // Калибровка датчиков
  Serial.println("Calibration started");
  Serial.println("1. Place on WHITE line and press button");
  while(digitalRead(BTN_PIN) == LOW);
  
  light color_black;
  color_ident(color_black);
  beep(1);
  
  delay(1000);
  Serial.println("2. Place on BLACK surface and press button");
  while(digitalRead(BTN_PIN) == LOW);
  
  light color_white;
  color_ident(color_white);
  beep(2);
  
  color_gray = (color_white + color_black) / 2;
  Serial.print("Calibration complete. Threshold: ");
  Serial.println(color_gray);
  
  Serial.println("System READY");
  printNodeName(current_node);
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
