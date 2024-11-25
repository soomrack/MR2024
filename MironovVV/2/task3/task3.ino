#define left_motor_1 6
#define left_motor_2 7
#define right_motor_1 5
#define right_motor_2 4
#define left_sensor A2
#define right_sensor A3
#define button 8
#define velocity 100
#define kp 1

uint16_t left_min_value;
uint16_t left_max_value;
uint16_t left_grey;
uint16_t right_min_value;
uint16_t right_max_value;
uint16_t right_grey;

void pins_init()
{
  pinMode(left_motor_1, OUTPUT);
  pinMode(left_motor_2, OUTPUT);
  pinMode(right_motor_1, OUTPUT);
  pinMode(right_motor_2, OUTPUT);
  pinMode(left_sensor, INPUT);
  pinMode(right_sensor, INPUT);
  pinMode(button, INPUT_PULLUP);
}


void sensors_calibration()
{
  left_min_value = analogRead(left_sensor);
  left_max_value = analogRead(left_sensor);
  right_min_value = analogRead(right_sensor);
  right_max_value = analogRead(right_sensor);

  analogWrite(left_motor_1, 180);
  digitalWrite(left_motor_2, 0);
  analogWrite(right_motor_1, 180);
  digitalWrite(left_motor_2, 1);

  uint32_t old_time = millis();
  while (millis() - old_time < 3000) {
    uint16_t left_sensor_value = analogRead(left_sensor);
    uint16_t right_sensor_value = analogRead(right_sensor);

    if (left_sensor_value < left_min_value) left_min_value = left_sensor_value;
    if (left_sensor_value > left_max_value) left_max_value = left_sensor_value;
    if (right_sensor_value < right_min_value) right_min_value = right_sensor_value;
    if (right_sensor_value > right_max_value) right_max_value = right_sensor_value;
  }

  left_grey = (left_min_value + left_max_value) / 2;
  right_grey = (right_min_value + right_max_value) / 2;

  analogWrite(left_motor_1, 0);
  digitalWrite(left_motor_2, 0);
  analogWrite(right_motor_1, 0);
  digitalWrite(right_motor_2, 0);
}

void liner()
{
  int16_t error = (left_grey - analogRead(left_sensor)) - (right_grey - analogRead(right_sensor));
  int16_t u = error * kp;

  analogWrite(left_motor_1, constrain(velocity + u, 0, 220));
  digitalWrite(left_motor_2, 1);
  analogWrite(right_motor_1, constrain(velocity - u, 0, 220));
  digitalWrite(right_motor_2, 1);
}

void line_check()
{
  uint32_t timer_old = millis();

  while (analogRead(left_sensor) < left_grey && analogRead(right_sensor) < right_grey)
  {
    if (millis() - timer_old < 1000)
    {
      analogWrite(left_motor_1, 100);
      digitalWrite(left_motor_2, 0);
      analogWrite(right_motor_1, 100);
      digitalWrite(right_motor_2, 0);
    }
    else
    {
      analogWrite(left_motor_1, 0);
      digitalWrite(left_motor_2, 1);
      analogWrite(right_motor_1, 0);
      digitalWrite(right_motor_2, 1);
    }
  }
}

void setup()
{
  pins_init();

  while (digitalRead(button) == 0) delay(1);

  sensors_calibration();

  while (digitalRead(button) == 0) delay(1);
}


void loop()
{
  line_check();
  liner();
}
