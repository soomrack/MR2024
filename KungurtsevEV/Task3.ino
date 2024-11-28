#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6

const int LINE_SENSOR_THRESHOLD = 650;
int lastDetectedSensor = -1;  // 0 для A0, 1 для A1
typedef unsigned int time;
const int speed = 200;
const int Speed_1 = 230;
const int Speed_2 = 150;
unsigned long current_time = 0;
unsigned long lastFindLineTime = 0; // Время последнего вызова find_line
int const FIND_TIME_VALUE = 50;

void setup() {
  pinMode(12, INPUT_PULLUP);
  InitMotors();
  Serial.begin(9600);
}

void InitMotors() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void Motors(int Speed1, int Speed2) {
  Speed1 = constrain(Speed1, -255, 255);
  Speed2 = constrain(Speed2, -255, 255);

  digitalWrite(M1_DIR, Speed1 > 0 ? HIGH : LOW);
  analogWrite(M1_PWM, abs(Speed1));

  digitalWrite(M2_DIR, Speed2 > 0 ? HIGH : LOW);
  analogWrite(M2_PWM, abs(Speed2));
}

void MoveForward(int Speed, unsigned long time, int sensorA0, int sensorA1) {
  if (sensorA0 > LINE_SENSOR_THRESHOLD && sensorA1 > LINE_SENSOR_THRESHOLD) {
    Motors(Speed, Speed);
    current_time = 0;
    lastDetectedSensor = -1;
  }
}

void MoveRight(int Speed_1, int Speed_2, unsigned long time, int sensorA0, int sensorA1) {
  if (sensorA0 > LINE_SENSOR_THRESHOLD && sensorA1 < LINE_SENSOR_THRESHOLD) {
    Motors(Speed_1, -Speed_2);
    current_time = 0;
    lastDetectedSensor = 0;
  }
}

void MoveLeft(int Speed_1, int Speed_2, unsigned long time, int sensorA0, int sensorA1) {
  if (sensorA0 < LINE_SENSOR_THRESHOLD && sensorA1 > LINE_SENSOR_THRESHOLD) {
    Motors(-Speed_1, Speed_2);
    current_time = 0;
    lastDetectedSensor = -1;
  }
}

int LineSensorRead(int SensorPin) {
  if (SensorPin < A0 || SensorPin > A3) return -1;
  return analogRead(SensorPin);
}

void Stop() {
  Motors(0, 0);
}

int flag = 1;
int counter = 0;
int millis_flag = 0;

void find_line(int sensorA0, int sensorA1) {
  if (sensorA0 < LINE_SENSOR_THRESHOLD && sensorA1 < LINE_SENSOR_THRESHOLD) {
    unsigned long move_time = millis();
    int speed = 100;
    int radius = 160;
    if (millis() - move_time < FIND_TIME_VALUE){
    Motors(-radius, radius - 69);
    move_time = 0;
    }
    if (millis() - move_time < FIND_TIME_VALUE * (1 + 1.4))
    Motors(-speed, -speed);
    radius = radius + 15;
    speed = speed + 7;

    lastFindLineTime = millis();

    if (millis() - lastFindLineTime >= 10000)
      Stop();
  }
}

void loop() {
  int sensorA0 = LineSensorRead(A0);
  int sensorA1 = LineSensorRead(A1);

  MoveForward(speed, current_time, sensorA0, sensorA1);
  MoveLeft(Speed_1, Speed_2, current_time, sensorA0, sensorA1);
  MoveRight(Speed_1, Speed_2, current_time, sensorA0, sensorA1);

  if (millis() - lastFindLineTime >= FIND_TIME_VALUE) find_line(sensorA0, sensorA1);

  delay(1);
}