//kungurtsev, degteva, drachkova

#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6
#define PIN_5_BEEP 9

const int LINE_SENSOR_THRESHOLD = 650; // эмпирический предел для датчика света
const int FIND_TIME_VALUE = 50; // время движения при поиске линии, мс
const int SPEED_FORWARD_VALUE = 200; // скорость моторов при движении вперед
const int SPEED_TURN_VALUE = 230; // скорость мотора при повороте
const int SPEED_TURN_SEC_VALUE = 150; // скорость противоположного мотора при повороте
const int TIMEOUT_VALUE = 10000; // время поиска линии до отключения, мс (10с)
int last_detected_sensor = -1;  //последний датчик, который был на линии ( 0 для A0, 1 для A1 )
int millis_flag = 0;


void init_motors()
{
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}


void setup()
{
  pinMode(PIN_5_BEEP, INPUT_PULLUP);
  init_motors();
  Serial.begin(9600);
}


int button_read(int ButtonPin)
{
  int Data = digitalRead(ButtonPin);
  if(ButtonPin == 12) Data = !Data;
  return Data;
}


void motors(int Speed1, int Speed2)
{
  Speed1 = constrain(Speed1, -255, 255);
  Speed2 = constrain(Speed2, -255, 255);

  digitalWrite(M1_DIR, Speed1 > 0 ? HIGH : LOW);
  analogWrite(M1_PWM, abs(Speed1));

  digitalWrite(M2_DIR, Speed2 > 0 ? HIGH : LOW);
  analogWrite(M2_PWM, abs(Speed2));
}


void move_forward(int Speed, int sensorA0, int sensorA1)
{
  if (sensorA0 > LINE_SENSOR_THRESHOLD && sensorA1 > LINE_SENSOR_THRESHOLD) {
    motors(Speed, Speed);
    last_detected_sensor = -1;
    millis_flag = 0;
  }
}


void move_right(int Speed_1, int Speed_2,int sensorA0, int sensorA1)
{
  if (sensorA0 > LINE_SENSOR_THRESHOLD && sensorA1 < LINE_SENSOR_THRESHOLD) {
    motors(Speed_1, -Speed_2);
    last_detected_sensor = 0;
    millis_flag = 0;
  }
}


void move_left(int Speed_1, int Speed_2, int sensorA0, int sensorA1)
{
  if (sensorA0 < LINE_SENSOR_THRESHOLD && sensorA1 > LINE_SENSOR_THRESHOLD) {
    motors(-Speed_1, Speed_2);
    last_detected_sensor = 1;
    millis_flag = 0;
  }
}


int line_sensor_read(int SensorPin)
{
  if (SensorPin < A0 || SensorPin > A3) return -1;
  return analogRead(SensorPin);
}


void stop()
{
  motors(0, 0);
  tone(PIN_5_BEEP, 1000, 500);
}


void sound_alarm()
{
    if (button_read(9)) tone(8, 3000);
    else noTone(8);
}


int flag = 1;
int speed = 70;
int radius = 160;
unsigned long move_time = -1;


void find_line(int sensorA0, int sensorA1)
{
  if (sensorA0 < LINE_SENSOR_THRESHOLD && sensorA1 < LINE_SENSOR_THRESHOLD) {
    if (last_detected_sensor == 0) {
      if (millis_flag == 0) {
        unsigned long move_time = millis();
        int speed = 70;
        int radius = 160;
      } 
      millis_flag = 1;

      if (millis() - move_time < FIND_TIME_VALUE) motors(radius, radius - 69); //налево
      else if (millis() - move_time < FIND_TIME_VALUE * (1 + 1.4)) {
        motors(speed,speed);                                                   //вперед
      }
      radius = radius + 15;
      speed = speed + 7;

      if (millis() - move_time < TIMEOUT_VALUE) {
        stop();
        flag = 0;
      }
    }

    else if (last_detected_sensor == 1){
      if (millis_flag == 0) {
        unsigned long move_time = millis();
        int speed = 70;
        int radius = 160;
      } 
      millis_flag = 1;

      if (millis() - move_time < FIND_TIME_VALUE) motors(radius - 69, radius); //направо
      else if (millis() - move_time < FIND_TIME_VALUE * (1 + 1.4)) {
        motors(speed,speed);                                                   //вперед
      }
      radius = radius + 15;
      speed = speed + 7;

      if (millis() - move_time < TIMEOUT_VALUE) {
        stop();
        flag = 0;
      }
    }
  }
}


void loop()
{
  int sensor_A0 = line_sensor_read(A0);
  int sensor_A1 = line_sensor_read(A1);

  sound_alarm();

  if(flag == 1){
    move_forward(SPEED_FORWARD_VALUE, sensor_A0, sensor_A1);
    move_left(SPEED_TURN_VALUE, SPEED_TURN_SEC_VALUE, sensor_A0, sensor_A1);
    move_right(SPEED_TURN_VALUE, SPEED_TURN_SEC_VALUE, sensor_A0, sensor_A1);
    find_line(sensor_A0, sensor_A1);
  }
}
