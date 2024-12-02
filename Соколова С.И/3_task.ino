#define RM_DIR 4
#define RM_SPEED 5
#define LM_DIR 7
#define LM_SPEED 6
#define PEEK  9


int  ld_sr, rd_sr, ld, rd;

bool r_stat, r_last_stat, l_stat, l_last_stat, flag;
uint32_t sec, tim;


void button_calibrate() {
  int ld_black, ld_white, rd_black, rd_white;
 delay(2000);
  while (digitalRead(8) == 0) {
    ld_white = analogRead(A2);
    rd_white = analogRead(A3);
}
 delay(2000);
 
  while (digitalRead(8) == 0) {
    ld_black = analogRead(A2);
    rd_black = analogRead(A3);
}
  ld_sr = (ld_black + ld_white) / 2;
  rd_sr = (rd_black + rd_white) / 2;
}


void Motors(int Speed1, int Speed2)
{
  if (Speed1 > 255) Speed1 = 255;
  if (Speed1 < -255) Speed1 = -255;
  if (Speed2 > 255) Speed2 = 255;
  if (Speed2 < -255) Speed2 = -255;

  if (Speed1 > 0)digitalWrite(RM_DIR, 1);
  else digitalWrite(RM_DIR, 0);
  analogWrite(RM_SPEED, abs(Speed1));

  if (Speed2 > 0)digitalWrite(LM_DIR, 1);
  else digitalWrite(LM_DIR, 0);
  analogWrite(LM_SPEED, abs(Speed2));
}


void linesensor_read () {
  ld = analogRead(A2);
  rd = analogRead(A3);
  if (ld > ld_sr) {
    if (l_last_stat != l_stat || r_last_stat != r_stat)l_last_stat = l_stat;
    l_stat = 1;
  }
  else {
    if (l_last_stat != l_stat || r_last_stat != r_stat)l_last_stat = l_stat;
    l_stat = 0;
  }
  if (rd > rd_sr) {
    if (l_last_stat != l_stat || r_last_stat != r_stat)r_last_stat = r_stat;
    r_stat = 1;
  }
  else {
    if (l_last_stat != l_stat || r_last_stat != r_stat)r_last_stat = r_stat;
    r_stat = 0;
  }
}


void line_drive() {
  int lm, rm;  
  if ( l_stat == 0 && r_stat == 0) return;
  if ( l_stat == 1 && r_stat == 0) {
    lm = 255;
    rm = 70;
    flag = 0;
  }
  if (l_stat == 0 && r_stat == 1) {
    lm = 70;
    rm = 255;
    flag = 0;
  }
  if ( l_stat == 1 && r_stat == 1) {
    lm = 150;
    rm = 150;
    flag = 0;
  }
  Motors(lm, rm);
}


void line_find() {
  int lm, rm;
  if ( l_stat != 0 & r_stat != 0) {
   return;
     
  }
  
  if (flag == 0) {
    tim = sec;
    flag = 1;
  }
  if (r_last_stat == 1) {
    lm = -100;
    rm = 200;
  }
  //проверить 
  else if (l_last_stat == 1) {
    lm = 200;
    rm = -100;
  }
  if (sec > tim + 500 && sec <= tim + 1000 ) {
    if (lm < 0) lm = -200;
    if (rm < 0) rm = -200;
  }
  if (sec > tim + 1000 && sec <= tim + 5000 ) {
    if (lm < 0) lm = 100;
    if (rm < 0) rm = 100;
  }
  if (sec > tim + 5000) {
    tone(PEEK , 1000, 200);
    lm = 0;
    rm = 0;
  }
Motors(lm, rm);
}


void setup() {
  Serial.begin(9600);
  button_calibrate();
  pinMode(PEEK, OUTPUT);
}


void loop() {
  sec = millis();
  linesensor_read ();
  line_drive();
  line_find();
   
}
