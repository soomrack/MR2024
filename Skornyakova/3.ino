#define PIN_RM_DIR 4
#define PIN_RM_SPEED 5
#define PIN_LM_DIR 7
#define PIN_LM_SPEED 6
#define PIN_PEEK  9
#define PIN_BUTTON  8

int LEFT_MIDLE_VALUE, RIGHT_MIDLE_VALUE, LEFT_SENSOR, RIGHT_SENSOR;
bool RIGHT_STATUS, RIGHT_LAST_STATUS, LEFT_STATUS, LEFT_LAST_STATUS, FLAG;
uint32_t DURATION_PROGRAM, TIME;


void button_calibrate() 
{
    int left_sensor_black, left_sensor_white, right_sensor_black, right_sensor_white;
    delay(2000);// ДЛЯ УСТАНОВКИ НА БЕЛЫЙ ЦВЕТ 
    while (digitalRead(PIN_BUTTON) == 0) {
    left_sensor_white = analogRead(A2);
    right_sensor_white = analogRead(A3);
    }
    delay(2000);// ДЛЯ ПЕРЕНОСА НА ДРУГОЙ ЦВЕТ 
    while (digitalRead(PIN_BUTTON) == 0) {
    left_sensor_black = analogRead(A2);
    right_sensor_black = analogRead(A3);
    }
   LEFT_MIDLE_VALUE = (left_sensor_black + left_sensor_white) / 2;
   RIGHT_MIDLE_VALUE = (right_sensor_black + right_sensor_white) / 2;
}


void Motors( const int right_speed , const int left_speed)
{
  if (right_speed > 255) right_speed = 255;
  if (right_speed < -255) right_speed = -255;
  if (left_speed > 255) left_speed = 255;
  if (left_speed < -255) left_speed = -255;

  if (right_speed > 0)digitalWrite(RM_DIR, 1);
  else digitalWrite(RM_DIR, 0);
  analogWrite(RM_SPEED, abs(right_speed));

  if (left_speed > 0)digitalWrite(LM_DIR, 1);
  else digitalWrite(LM_DIR, 0);
  analogWrite(LM_SPEED, abs(left_speed));
}


void linesensor_read () 
{
  LEFT_SENSOR = analogRead(A2);
  RIGHT_SENSOR = analogRead(A3);
  if (LEFT_SENSOR > LEFT_MIDLE_VALUE) {
    if (LEFT_LAST_STATUS != LEFT_STATUS || RIGHT_LAST_STATUS != RIGHT_STATUS)LEFT_LAST_STATUS = LEFT_STATUS;
    LEFT_STATUS = 1;
  }
  else {
    if (LEFT_LAST_STATUS != LEFT_STATUS || RIGHT_LAST_STATUS != RIGHT_STATUS)LEFT_LAST_STATUS = LEFT_STATUS;
    LEFT_STATUS = 0;
  }
  if (RIGHT_SENSOR > RIGHT_MIDLE_VALUE) {
    if (LEFT_LAST_STATUS != LEFT_STATUS || RIGHT_LAST_STATUS != RIGHT_STATUS)RIGHT_LAST_STATUS = RIGHT_STATUS;
    RIGHT_STATUS = 1;
  }
  else {
    if (LEFT_LAST_STATUS != LEFT_STATUS || RIGHT_LAST_STATUS != RIGHT_STATUS)RIGHT_LAST_STATUS = RIGHT_STATUS;
    RIGHT_STATUS = 0;
  }
}


void line_drive() 
{
  int lm, rm;  
  if ( LEFT_STATUS == 0 && RIGHT_STATUS == 0) return;
  if ( LEFT_STATUS == 1 && RIGHT_STATUS == 0) {
    lm = 255;
    rm = 70;
    FLAG = 0;
  }
  if (LEFT_STATUS == 0 && RIGHT_STATUS == 1) {
    lm = 70;
    rm = 255;
    FLAG = 0;
  }
  if ( LEFT_STATUS == 1 && RIGHT_STATUS == 1) {
    lm = 150;
    rm = 150;
    FLAG = 0;
  }
  Motors(lm, rm);
}


void line_find()
{
  int lm, rm;
  if ( LEFT_STATUS != 0 & RIGHT_STATUS != 0) {
   return;  
  }
  if (FLAG == 0) {
    TIME = DURATION_PROGRAM;
    FLAG = 1;
  }
  if (RIGHT_LAST_STATUS == 1) {
    lm = -100;
    rm = 200;
  }
  //проверить 
  else if (LEFT_LAST_STATUS == 1) {
    lm = 200;
    rm = -100;
  }
  if (DURATION_PROGRAM > TIME + 500 && DURATION_PROGRAM <= TIME + 1000 ) {
    if (lm < 0) lm = -200;
    if (rm < 0) rm = -200;
  }
  if (DURATION_PROGRAM > TIME + 1000 && DURATION_PROGRAM <= TIME + 3000 ) {
    if (lm < 0) lm = 100;
    if (rm < 0) rm = 100;
  }
  
  if (DURATION_PROGRAM > TIME + 3000) {
    tone(PEEK , 1000, 2000);
    lm = 0;
    rm = 0;
  }
  Motors(lm, rm);
}


void setup() 
{
  button_calibrate();
  pinMode(PEEK, OUTPUT);
}


void loop()
{
  DURATION_PROGRAM = millis();
  linesensor_read ();
  line_drive();
  line_find();   
}
