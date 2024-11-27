//
// Authors: Artamonov, Ivanov
//

#define PIN_1_DIR 4
#define PIN_1_PWM 5
#define PIN_2_DIR 7
#define PIN_2_PWM 6
#define PIN_5_BEEP 9


const int COLOR_THRESHOLD = 650;

const int MAX_SEARCH_SPEED =  170;
const int MAX_SPEED = 255;
const int FORWARD_SPEED = 180;
const int FAST_SIDE_TURN_SPEED = 230;
const int SLOW_SIDE_TURN_SPEED = 160;

const int BEEP_TONE = 130;
const int BEEP_COUNT = 1;
const int BEEP_DURATION = 100;

const int STOP_TIMEOUT = 5000;
const int RUN_TIME = 50;

int line_search_speed = 100;
int flag  = 0;


void init_motors()
{
  pinMode(PIN_1_DIR, OUTPUT);
  pinMode(PIN_1_PWM, OUTPUT);
  pinMode(PIN_2_DIR, OUTPUT);
  pinMode(PIN_2_PWM, OUTPUT);
}


void move_motors(int left_side_speed, int right_side_speed)
{
  if(left_side_speed > MAX_SPEED) left_side_speed = MAX_SPEED;
  if(left_side_speed < - MAX_SPEED) left_side_speed = - MAX_SPEED;
  if(right_side_speed > MAX_SPEED) right_side_speed = MAX_SPEED;
  if(right_side_speed < - MAX_SPEED) right_side_speed = - MAX_SPEED;

  if(left_side_speed > 0){
    digitalWrite(PIN_1_DIR, 1);
    analogWrite(PIN_1_PWM, left_side_speed);
  }
  else{
    digitalWrite(PIN_1_DIR, 0);
    analogWrite(PIN_1_PWM, -left_side_speed);
  }

  if(right_side_speed > 0){
    digitalWrite(PIN_2_DIR, 1);
    analogWrite(PIN_2_PWM, right_side_speed);
  }
  else{
    digitalWrite(PIN_2_DIR, 0);
    analogWrite(PIN_2_PWM, -right_side_speed);
  }
}


void move_forward(const int speed)
{
  move_motors(speed, speed);
}


void move_right(const int left_side_speed ,const int right_side_speed)
{
  move_motors(left_side_speed, - right_side_speed);
}


void move_left(const int Speed_Left ,const int Speed_Right)
{
  move_motors(- Speed_Left, Speed_Right);
}


int reed_color_sensor(const int sensor_pin)
{
  if(sensor_pin < A0 || sensor_pin > A3)
    return -1;

  int data = analogRead(sensor_pin);
  return data;
}


void make_beep()  // Звуковое сопровождение
{
    for(int i = 0; i < BEEP_COUNT; i++)
    {
      tone(PIN_5_BEEP, BEEP_TONE);
      delay(BEEP_DURATION);
      noTone(PIN_5_BEEP);
    }
}

void stop_motors()
{
  move_motors(0, 0);

}


void stop() {
  unsigned long time = millis();

  while (reed_color_sensor(A0) < COLOR_THRESHOLD && reed_color_sensor(A1) < COLOR_THRESHOLD) {
    unsigned long timel = millis();

    while (millis() - timel < RUN_TIME){ 
      move_left(MAX_SPEED, MAX_SPEED);
    }

    unsigned long timef = millis();

    while (millis() - timef < RUN_TIME){
      move_forward(line_search_speed);
    }

    line_search_speed = line_search_speed + 2;

    if (line_search_speed >= MAX_SPEED) { line_search_speed = MAX_SPEED; }
    if (millis() - time > STOP_TIMEOUT) { flag = 1; }
  }
}


void setup()
{
  init_motors();
  pinMode(PIN_5_BEEP, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop() {
  if (reed_color_sensor(A0) > COLOR_THRESHOLD && reed_color_sensor(A1) > COLOR_THRESHOLD) {
    move_forward(FORWARD_SPEED);
  } 
  else if (reed_color_sensor(A1) > COLOR_THRESHOLD && reed_color_sensor(A0) < COLOR_THRESHOLD) {
    move_left(FAST_SIDE_TURN_SPEED, SLOW_SIDE_TURN_SPEED);
  } 
  else if (reed_color_sensor(A0) > COLOR_THRESHOLD && reed_color_sensor(A1) < COLOR_THRESHOLD) {
    move_right(SLOW_SIDE_TURN_SPEED, FAST_SIDE_TURN_SPEED);
  } 
  else {
    stop();
    if (flag == 1) { stop_motors(); } 
  }
}