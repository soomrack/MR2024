#include <stdint.h>


#define PIN_PWM_LEFT 6
#define PIN_DIR_LEFT 7
#define PIN_PWM_RIGHT 5
#define PIN_DIR_RIGHT 4
#define PIN_BUTTON 11
#define PIN_SOUND_SENSOR 9
uint8_t PINS_ANALOG[2] = {A2, A3};


static bool is_running = 0;


const int DELAY_MS = 5;


const float K_P = 7;
const float K_D = 20.0;


const int DEFAULT_SPEED = 200;


const int SPEED_MAX = 250;
const int SPEED_MIN = -100;


static int search_speed = 180;
const int BLACK_THRESHOLD = 60;


struct Sensor_limit
{
  int min;
  int max;

  Sensor_limit()
  {
    min = 1024;
    max = 0;
  }
};


Sensor_limit calibration_data[2];
int sensors_data_raw[2];
int sensors_data[2];


void pins_init()
{
  pinMode(PIN_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIR_LEFT, OUTPUT);
  pinMode(PIN_PWM_RIGHT, OUTPUT);
  pinMode(PIN_DIR_RIGHT, OUTPUT);

  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_SOUND_SENSOR, OUTPUT);
}


void motors_set_speed(int left, int right)
{
  left = constrain(left, -(int)UINT8_MAX, (int)UINT8_MAX);
  right = constrain(right, -(int)UINT8_MAX, (int)UINT8_MAX);

  digitalWrite(PIN_DIR_LEFT, left < 0);
  digitalWrite(PIN_DIR_RIGHT, right < 0);

  analogWrite(PIN_PWM_LEFT, abs(left));
  analogWrite(PIN_PWM_RIGHT, abs(right));
}


bool button_is_pressed()
{
  static bool old_state = 0;
  static bool is_pressed = 0;
  static unsigned long timeout_end = 0;


  bool state = digitalRead(PIN_BUTTON);

  if (!is_pressed) {
    if (state && !old_state) {
      is_pressed = 1;
      timeout_end = millis() + DELAY_MS;
    }
    old_state = state;
  }
  else {
    if (millis() > timeout_end) {
      is_pressed = 0;
      if (state) return 1;
    }
  }

  return 0;
}


void sensors_read_raw()
{
  for (uint8_t idx = 0; idx < 2; idx++) {
    sensors_data_raw[idx] = analogRead(PINS_ANALOG[idx]);
  }
}


void sensors_update_limits()
{
  for (uint8_t idx = 0; idx < 2; idx++) {
    calibration_data[idx].min = min(sensors_data_raw[idx], calibration_data[idx].min);
    calibration_data[idx].max = max(sensors_data_raw[idx], calibration_data[idx].max);
  }
}


void calibration()
{
  unsigned long time_end = millis() + 3500;

  motors_set_speed(140, -140);

  while (millis() < time_end) {
    sensors_read_raw();
    sensors_update_limits();
  }

  motors_set_speed(0, 0);
}


void sensors_read()
{
  sensors_read_raw();

  for (uint8_t idx = 0; idx < 2; idx++) {
    sensors_data[idx] = map(sensors_data_raw[idx], calibration_data[idx].min, calibration_data[idx].max, 0, 100);
  }
}


bool is_on_line()
{
  bool on_line = 0;

  for (uint8_t idx = 0; idx < 2; idx++) {
    if (sensors_data[idx] > BLACK_THRESHOLD) on_line = 1;
  }

  return on_line;
}


int PD_regulator()
{
  static int u_old = 0;
  static float err_old = 0.0f;

  float err = sensors_data[0] - sensors_data[1];
  float u = err * K_P + (err - err_old) * K_D;
  err_old = err;

  if (!is_on_line()) {
    if (u_old > 0) {
      return search_speed;
    }
    else return -search_speed;
  }

  u_old = u;

  return u;
}


bool line_is_not_found() {
  static unsigned long time_end = 5000 + millis();

  if (!is_on_line()) {
    if (millis() > time_end) {
      is_running = 0;
      return 1;
    }
  } else {
    time_end = 2000 + millis();
  }

  return 0;
}


void play_sound() {
  const unsigned long sound_duration = 500;
  unsigned long start_time = millis();

  while (millis() - start_time < sound_duration) {
    digitalWrite(PIN_SOUND_SENSOR, HIGH);
  }

  digitalWrite(PIN_SOUND_SENSOR, LOW);
}


void drive_line()
{
  sensors_read();

  int u = PD_regulator();

  int left_speed = constrain(DEFAULT_SPEED + u, SPEED_MIN, SPEED_MAX);
  int right_speed = constrain(DEFAULT_SPEED - u, SPEED_MIN, SPEED_MAX);
  motors_set_speed(left_speed, right_speed);
}

void find_line()
{
  sensors_read();

  int u = PD_regulator();

  if (line_is_not_found()) {
    play_sound();
    return;
  }
  
  int left_speed = constrain(DEFAULT_SPEED + u, SPEED_MIN, SPEED_MAX);
  int right_speed = constrain(DEFAULT_SPEED - u, SPEED_MIN, SPEED_MAX);
  motors_set_speed(left_speed, right_speed);
}

void setup()
{
  pins_init();

  while (true) {
    if (button_is_pressed()) {
      break;
    }
  }

  calibration();
}


void loop()
{

  if (button_is_pressed()) {
    is_running = !is_running;
  }
  
  if (is_running) {
    drive_line();

    if(!is_on_line()) {
      find_line();
    }
  }
  else {
    motors_set_speed(0, 0); //stopping robot
  }

  delay(1);
}
