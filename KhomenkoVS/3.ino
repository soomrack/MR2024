#define PIN_MR_DIR 4
#define PIN_MR_SPEED 5
#define PIN_ML_DIR 7
#define PIN_ML_SPEED 6
#define PIN_BUTTON 8
#define PIN_SOUND 10

int L_Sensor, R_Sensor, L_Value, R_Value;
bool R_status, L_status, R_status2, L_status2, mark;
uint32_t Dur, Time;


void button_calibrate() 
{
    int l_sensor_black, l_sensor_white, r_sensor_black, r_sensor_white;
    delay(1000); //время на установку на белом цвете
    while (digitalRead(PIN_BUTTON) == 0) {
    l_sensor_white = analogRead(A2);
    r_sensor_white = analogRead(A3);
    }
    delay(1000); //время на установку на черном цвете
    while (digitalRead(PIN_BUTTON) == 0) {
    l_sensor_black = analogRead(A2);
    r_sensor_black = analogRead(A3);
    }
   L_Value = (l_sensor_black + l_sensor_white) / 2;
   R_Value = (r_sensor_black + r_sensor_white) / 2;
}


void Motors(int r_speed , int l_speed)
{
  if (r_speed > 255) r_speed = 255;
  if (r_speed < -255) r_speed = -255;
  if (l_speed > 255) l_speed = 255;
  if (l_speed < -255) l_speed = -255;

  if (r_speed > 0)digitalWrite(PIN_MR_DIR, 1);
  else digitalWrite(PIN_MR_DIR, 0);
  analogWrite(PIN_MR_SPEED, abs(r_speed));

  if (l_speed > 0)digitalWrite(PIN_ML_DIR, 1);
  else digitalWrite(PIN_ML_DIR, 0);
  analogWrite(PIN_ML_SPEED, abs(l_speed));
}


void linesensor_read () 
{
  L_Sensor = analogRead(A2);
  R_Sensor = analogRead(A3);
  if (L_Sensor > L_Value) {
    if (L_status2 != L_status || R_status2 != R_status)L_status2 = L_status;
    L_status = 1;
  }
  else {
    if (L_status2 != L_status || R_status2 != R_status)L_status2 = L_status;
    L_status = 0;
  }
  if (R_Sensor > R_Value) {
    if (L_status2 != L_status ||  R_status2 != R_status)R_status2 = R_status;
    R_status = 1;
  }
  else {
    if (L_status2 != L_status || R_status2 != R_status)R_status2 = R_status;
    R_status = 0;
  }
}


void line_drive() 
{
  int l_m, r_m;  
  if (L_status == 0 && R_status == 0) return;
  if (L_status == 1 && R_status == 0) {
    l_m = 255;
    r_m = 70;
    mark = 0;
  }
  if (L_status == 0 && R_status == 1) {
    l_m = 70;
    r_m = 255;
    mark = 0;
  }
  if (L_status == 1 && R_status == 1) {
    l_m = 150;
    r_m = 150;
    mark = 0;
  }
  Motors(l_m, r_m);
}


void line_find()
{
  int l_m, r_m;
  if ( L_status != 0 & R_status != 0) {
   return;  
  }
  if (mark == 0) {
    Time = Dur;
    mark = 1;
  }
  if (R_status2 == 1) {
    l_m = -100;
    r_m = 200;
  }
 
  else if (L_status2 == 1) {
    l_m = 200;
    r_m = -100;
  }
  if (Dur > Time + 1000 && Dur <= Time + 3000 ) {
    if (l_m < 0) l_m = -200;
    if (r_m < 0) r_m = -200;
  }
  if (Dur > Time + 3000 && Dur <= Time + 5000 ) {
    if (l_m < 0) l_m = 100;
    if (r_m < 0) r_m = 100;
  }
  
  if (Dur > Time + 5000 ) {
    tone(PIN_SOUND, 659, 2000);
    l_m = 0;
    r_m = 0;
  }

  Motors(l_m, r_m);
}


void setup() 
{
  button_calibrate();
  pinMode(PIN_SOUND, OUTPUT);
}


void loop()
{
  Dur = millis();
  linesensor_read ();
  line_drive();
  line_find();   
}