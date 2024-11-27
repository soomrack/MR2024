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
const int SPEED_INCREMENT = 2;

const int BEEP_TONE = 130;
const int BEEP_COUNT = 1;
const int BEEP_DURATION = 100;

const int STOP_TIMEOUT = 5000;  // Miliseconds
const int SEARCH_TIMEOUT = 10;  // Seconds
const int MOVEMENT_CHANGE_TIMEOUT = 1;

int line_search_speed = 100;
int stop_flag = 0;
int search_move = -1;

unsigned long start_move_time = -1;
unsigned long search_start_time = -1;
unsigned long beep_start_time = -1;


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


void make_beep()
{
  unsigned long current_time = millis();

  if (search_start_time == -1) { beep_start_time = current_time; }

  if (current_time - beep_start_time < BEEP_DURATION) {
    tone(PIN_5_BEEP, BEEP_TONE);
    noTone(PIN_5_BEEP);
  }
}


void reset_search_time()
{
  search_start_time = -1;
}

void reset_search_move_time()
{
  start_move_time = -1;
}


void increase_speed()
{
  line_search_speed += SPEED_INCREMENT;
  if (line_search_speed >= MAX_SPEED) { line_search_speed = MAX_SPEED; }
}


void stop_motors()
{
  move_motors(0, 0);
  make_beep();
}


void make_movement(const unsigned long current_time) 
{

  if (search_start_time == -1) {
    search_start_time = current_time;
  }
  else if (current_time - start_move_time >= MOVEMENT_CHANGE_TIMEOUT) {
    search_move = -search_move; 
    start_move_time = current_time;
  }

  if (search_move == -1) {
    move_left(MAX_SPEED, MAX_SPEED);
  }
  else {
    move_forward(line_search_speed);
  }
  increase_speed();
}


void search_for_line() {
  unsigned long current_time = millis();

  if (search_start_time == -1) { search_start_time = current_time; }

  if (current_time - search_start_time < SEARCH_TIMEOUT) {
    make_movement(current_time);
  }

  if (current_time - search_start_time > STOP_TIMEOUT) { stop_flag = 1; }
}


void setup()
{
  init_motors();
  pinMode(PIN_5_BEEP, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop() {
  if (reed_color_sensor(A0) > COLOR_THRESHOLD && reed_color_sensor(A1) > COLOR_THRESHOLD) {
    reset_search_time();
    reset_search_move_time();
    move_forward(FORWARD_SPEED);
  } 
  else if (reed_color_sensor(A1) > COLOR_THRESHOLD && reed_color_sensor(A0) < COLOR_THRESHOLD) {
    reset_search_time();
    reset_search_move_time();
    move_left(FAST_SIDE_TURN_SPEED, SLOW_SIDE_TURN_SPEED);
  } 
  else if (reed_color_sensor(A0) > COLOR_THRESHOLD && reed_color_sensor(A1) < COLOR_THRESHOLD) {
    reset_search_time();
    reset_search_move_time();
    move_right(SLOW_SIDE_TURN_SPEED, FAST_SIDE_TURN_SPEED);
  } 
  else if (stop_flag == 0){
    search_for_line();
  }
  else {
    stop_motors();
  }
}