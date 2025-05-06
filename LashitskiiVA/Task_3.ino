#define PIN_DIR_R 4
#define PIN_POW_R 5
#define PIN_DIR_L 7
#define PIN_POW_L 6
#define PIN_BUTTON 8
#define PIN_SOUND 10

int sens_L, sens_R, grey_L, grey_R;
bool position_R, position_L, position_R2, position_L2, flag, stop_flag = 0;
uint32_t duration, time;


void button_calibrate() 
{
    int black_L, white_L, black_R, white_R;
    delay(1000); // время на считывание
    while (digitalRead(PIN_BUTTON) == 0) {
    white_L = analogRead(A2);
    white_R = analogRead(A3);
    }
    delay(1000); // время на считывание
    while (digitalRead(PIN_BUTTON) == 0) {
    black_L = analogRead(A2);
    black_R = analogRead(A3);
    }
   grey_L = (black_L + white_L) / 2;
   grey_R = (black_R + white_R) / 2;
}


void Motors(int r_speed , int l_speed)
{
  if (r_speed > 255) r_speed = 255;
  if (r_speed < -255) r_speed = -255;
  if (l_speed > 255) l_speed = 255;
  if (l_speed < -255) l_speed = -255;

  if (r_speed > 0) digitalWrite(PIN_DIR_R, 1);
  else digitalWrite(PIN_DIR_R, 0);
  analogWrite(PIN_POW_R, abs(r_speed));

  if (l_speed > 0) digitalWrite(PIN_DIR_L, 1);
  else digitalWrite(PIN_DIR_L, 0);
  analogWrite(PIN_POW_L, abs(l_speed));
}


void linesensor_read () 
{
  sens_L = analogRead(A2);
  sens_R = analogRead(A3);
  if (sens_L > grey_L) {
    if (position_L2 != position_L || position_R2 != position_R)position_L2 = position_L;
    position_L = 1;
  }
  else {
    if (position_L2 != position_L || position_R2 != position_R)position_L2 = position_L;
    position_L = 0;
  }
  if (sens_R > grey_R) {
    if (position_L2 != position_L ||  position_R2 != position_R)position_R2 = position_R;
    position_R = 1;
  }
  else {
    if (position_L2 != position_L || position_R2 != position_R)position_R2 = position_R;
    position_R = 0;
  }
}


void line_drive() 
{
  int l_m, r_m;  
  if (position_L == 0 && position_R == 0) return;
  if (position_L == 1 && position_R == 0) {
    l_m = 255;
    r_m = 70;
    flag = 0;
  }
  if (position_L == 0 && position_R == 1) {
    l_m = 70;
    r_m = 255;
    flag = 0;
  }
  if (position_L == 1 && position_R == 1) {
    l_m = 150;
    r_m = 150;
    flag = 0;
  }
  Motors(l_m, r_m);
}


void line_find()
{
  int l_m, r_m;
  if (position_L != 0 & position_R != 0) {
   return;  
  }
  if (flag == 0) {
    time = duration;
    flag = 1;
  }
  if (position_R2 == 1) {
    l_m = -100;
    r_m = 200;
  }
 
  else if (position_L2 == 1) {
    l_m = 200;
    r_m = -100;
  }
  if (duration > time + 1000 && duration <= time + 3000 ) {
    if (l_m < 0) l_m = -200;
    if (r_m < 0) r_m = -200;
  }
  if (duration > time + 3000 && duration <= time + 4000 ) {
    if (l_m < 0) l_m = 100;
    if (r_m < 0) r_m = 100;
  }
  
  if (duration > time + 4000 ) {
    stop_flag = 1;
  }

  Motors(l_m, r_m);
}

void stop(){
  if (stop_flag == 1){
    Motors(0, 0);
    tone(PIN_SOUND, 700, 1000);
    delay(20000);
  }
}

void setup() 
{
  button_calibrate();
  pinMode(PIN_SOUND, OUTPUT);
}


void loop()
{
  duration = millis();
  stop();
  linesensor_read ();
  line_drive();
  line_find();   
}