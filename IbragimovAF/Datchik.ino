#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6

int  ld_black, ld_white, rd_black, rd_white, ld_sr, rd_sr, lm, rm, ld, rd;
bool r_stat, r_last_stat, l_stat, l_last_stat, flag;
uint32_t sec, tim;


void button_calibrate() {
  while (digitalRead(8) == 0) {
    ld_black = analogRead(A2);
    rd_black = analogRead(A3);
  }
  delay(200);
  while (digitalRead(8) == 0) {
    ld_white = analogRead(A2);
    rd_white = analogRead(A3);
  }
  ld_sr = ( ld_black + ld_white) / 2;
  rd_sr = (rd_black + rd_white) / 2;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  button_calibrate();
}

void Motors(int Speed1, int Speed2)
{
  if (Speed1 > 255) Speed1 = 255;
  if (Speed1 < -255) Speed1 = -255;
  if (Speed2 > 255) Speed2 = 255;
  if (Speed2 < -255) Speed2 = -255;

  if (Speed1 > 0)digitalWrite(M1_DIR, 1);
  else digitalWrite(M1_DIR, 0);
  analogWrite(M1_PWM, abs(Speed1));

  if (Speed2 > 0)digitalWrite(M2_DIR, 1);
  else digitalWrite(M2_DIR, 0);
  analogWrite(M2_PWM, abs(Speed2));
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
  if (flag == 0) {
    tim = sec;
    flag = 1;
  }
  if (r_last_stat == 1) {
    lm = -100;
    rm = 200;
  }
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
    lm = 0;
    rm = 0;
  }
  Motors(lm, rm);
}

void loop() {
  sec = millis();
  linesensor_read ();
  if ( l_stat != 0 & r_stat != 0)line_drive();
  else line_find();
}
