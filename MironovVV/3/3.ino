#define PIN_LEFT_MOTOR_1 6
#define PIN_LEFT_MOTOR_2 7
#define PIN_RIGHT_MOTOR_1 5
#define PIN_RIGHT_MOTOR_2 4
#define PIN_LEFT_SENSOR A2
#define PIN_RIGHT_SENSOR A3
#define PIN_BUTTON 8
#define velocity 100
#define kp 1

uint16_t left_min_value;
uint16_t left_max_value;
uint16_t left_grey;
uint16_t right_min_value;
uint16_t right_max_value;
uint16_t right_grey;

bool timer_state = 0;

void pins_init()
{
  pinMode(PIN_LEFT_MOTOR_1, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_2, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_1, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_2, OUTPUT);
  pinMode(PIN_LEFT_SENSOR, INPUT);
  pinMode(PIN_RIGHT_SENSOR, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}


void sensors_calibration()
{
  left_min_value = analogRead(PIN_LEFT_SENSOR);
  left_max_value = analogRead(PIN_LEFT_SENSOR);
  right_min_value = analogRead(PIN_RIGHT_SENSOR);
  right_max_value = analogRead(PIN_RIGHT_SENSOR);

  motors_set(160, 0, 160, 1);

  uint32_t old_time = millis();
  while (millis() - old_time < 3000) {
    uint16_t PIN_LEFT_SENSOR_value = analogRead(PIN_LEFT_SENSOR);
    uint16_t PIN_RIGHT_SENSOR_value = analogRead(PIN_RIGHT_SENSOR);

    if (PIN_LEFT_SENSOR_value < left_min_value) left_min_value = PIN_LEFT_SENSOR_value;
    if (PIN_LEFT_SENSOR_value > left_max_value) left_max_value = PIN_LEFT_SENSOR_value;
    if (PIN_RIGHT_SENSOR_value < right_min_value) right_min_value = PIN_RIGHT_SENSOR_value;
    if (PIN_RIGHT_SENSOR_value > right_max_value) right_max_value = PIN_RIGHT_SENSOR_value;
  }

  left_grey = (left_min_value + left_max_value) / 2;
  right_grey = (right_min_value + right_max_value) / 2;

  motors_set(0, 0, 0, 0);
}

void line_ride()
{
  if (analogRead(PIN_LEFT_SENSOR) > left_grey || analogRead(PIN_RIGHT_SENSOR) > right_grey)
  {
    int16_t error = (left_grey - analogRead(PIN_LEFT_SENSOR)) - (right_grey - analogRead(PIN_RIGHT_SENSOR));
    int16_t u = error * kp;

    motors_set(constrain(velocity + u, 0, 220), 1, constrain(velocity - u, 0, 220), 1);
  }
}

void line_back()
{
  if (analogRead(PIN_LEFT_SENSOR) < left_grey && analogRead(PIN_RIGHT_SENSOR) < right_grey)
  {
    if (timer_state == 0)
    {
      timer_old = millis();
      timer_state = 1;
    }

    if (millis() - timer_old < 1000) motors_set(100, 0, 100, 0);
    else motors_set(0, 0, 0, 0);
  }

  timer_state = 0;
}

void motors_set(uint8_t value1, uint8_t value2, uint8_t value3, uint8_t value4)
{
  analogWrite(PIN_LEFT_MOTOR_1, value1);
  digitalWrite(PIN_LEFT_MOTOR_2, value2);
  analogWrite(PIN_RIGHT_MOTOR_1, value3);
  digitalWrite(PIN_RIGHT_MOTOR_2, value4);
}

void setup()
{
  pins_init();

  while (digitalRead(PIN_BUTTON) == 0) delay(1);

  sensors_calibration();

  while (digitalRead(PIN_BUTTON) == 0) delay(1);
}


void loop()
{
  line_back();

  line_ride();
}
