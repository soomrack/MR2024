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
light cur_color_L, cur_color_R, color_gray, color_black, color_white;
unsigned long search_time = 0;

// Карта маршрутов
const int ROUTE_MAP[12][4] = {
  /* 0 */  {0, 0, 0, 0},
  /* 1 */  {0, 0, 0, 0},
  /* 2 */  {0, 0, 0, CROSS_4},
  /* 3 */  {0, 0, 0, CROSS_4},
  /* 4 */  {END_2, CROSS_7, END_3, 0},
  /* 5 */  {0, 0, 0, CROSS_7},
  /* 6 */  {0, 0, 0, CROSS_7},
  /* 7 */  {END_5, CROSS_10, END_8, CROSS_4},
  /* 8 */  {0, 0, 0, CROSS_7},
  /* 9 */  {0, 0, 0, CROSS_10},
  /* 10 */ {END_9, 0, END_11, CROSS_7},
  /* 11 */ {0, 0, 0, CROSS_10}
};

// Системные переменные
int current_node = CROSS_4;
int target_node = -1;
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
  // Плавная остановка
  for(int i = BASE_SPEED; i >= 0; i-=10) {
    analogWrite(PWR_R_PIN, i);
    analogWrite(PWR_L_PIN, i);
    delay(50);
  }
}

// ============ Улучшенное движение по линии ============
void lineFollowing() {
  cur_color_L = analogRead(SENS_L_PIN);
  cur_color_R = analogRead(SENS_R_PIN);

  // ПИД-регулятор
  static int last_error = 0;
  static float integral = 0;
  
  int error = cur_color_L - cur_color_R;
  integral += error * 0.1;
  integral = constrain(integral, -100, 100);
  
  int derivative = error - last_error;
  last_error = error;
  
  int correction = error * 0.7 + integral * 0.05 + derivative * 0.3;

  int leftSpeed = constrain(BASE_SPEED + correction, 0, MAX_SPEED);
  int rightSpeed = constrain(BASE_SPEED - correction, 0, MAX_SPEED);

  digitalWrite(DIR_R_PIN, 1);
  digitalWrite(DIR_L_PIN, 1);
  analogWrite(PWR_R_PIN, rightSpeed);
  analogWrite(PWR_L_PIN, leftSpeed);
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
      
    default: 
      return -1;
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
  data.trim();
  if(data.length() > 0 && isDigit(data[0])) {
    int node = data.toInt();
    if(node >= 1 && node <= 11) {
      current_node = node;
      Serial.print("QR detected: ");
      printNodeName(current_node);
      
      if(current_node == target_node) {
        arrived = true;
        beep(3);
      } else {
        beep(1);
      }
    }
  }
}

void navigateToTarget() {
  if(current_node == target_node) {
    arrived = true;
    beep(3);
    return;
  }

  int direction = calculateBestDirection();
  if(direction == -1) {
    Serial.println("ERROR: No valid path");
    stopMotors();
    return;
  }

  executeMovement(direction);

  unsigned long startTime = millis();
  while(!checkIntersection() && millis() - startTime < 5000) {
    lineFollowing();
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
  Serial.println(color_white);
  beep(1);
  
  delay(1000);
  
  // 2. Черная линия
  Serial.println("2. Place on BLACK line and press button");
  while(digitalRead(BTN_PIN) == LOW);
  color_black = (analogRead(SENS_L_PIN) + analogRead(SENS_R_PIN)) / 2;
  Serial.println(color_black);
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
  
  if(Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    
    if(data.startsWith("GO")) {
      int node = data.substring(2).toInt();
      if(node >= 1 && node <= 11) {
        target_node = node;
        arrived = false;
        Serial.print("New target: ");
        printNodeName(target_node);
      }
    }
    else if(data == "STOP") {
      target_node = -1;
      arrived = true;
      stopMotors();
    }
  }
}
