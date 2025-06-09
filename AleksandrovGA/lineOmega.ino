#define LEFT_PWM 5
#define LEFT_DIR 4
#define RIGHT_PWM 6
#define RIGHT_DIR 7
#define SOUND 9

#define K_P 8.0
#define K_D 4.0

#define V 150

long long int TIME_SOUND = 0;
int LEFT_MIN = 1023;
int LEFT_MAX = 0;
int RIGHT_MIN = 1023;
int RIGHT_MAX = 0;

int LAST_DIRECTION = 0;

int ERROLD = 0;

bool IS_TURNED = false;
bool BUTTON_OLD = 1;

bool RECOVERING = false;


void drive(int left, int right)
{
  digitalWrite(LEFT_DIR, left > 0);
  digitalWrite(RIGHT_DIR, right > 0);

  analogWrite(LEFT_PWM, abs(left));
  analogWrite(RIGHT_PWM, abs(right));
}


void recoverLine(int LAST_DIRECTION)
{
  RECOVERING = true;
  if (LAST_DIRECTION == 0) {
    drive(-20, 240);
  }
  else {
    drive(240, -20);
  }

}


/*void end_music() 
{
  tone(SOUND, 1000, 2000);
}*/

void check_line(int sensor1, int sensor2)
{
  if (sensor1 < 60 && sensor2 < 60) {
    if (millis() - TIME_SOUND > 5 * 1000) {
      //end_music();
      IS_TURNED = 0;
    }
  } else {
    TIME_SOUND = millis();
    double err = (sensor1 - sensor2);
    double u = err * K_P + (err - ERROLD) * K_D;
    drive(constrain(V + u, -250, 250), constrain(V - u, -250, 250));
    ERROLD = err;
    if (sensor2 > sensor1) {
      LAST_DIRECTION = 0;
    }
    else {
      LAST_DIRECTION = 1;
    }
  }
}


void start_button()
{
  if (digitalRead(A2) == 1 && BUTTON_OLD == 0) {
    delay(5);
    if (digitalRead(A2) == 1) {
      IS_TURNED = !IS_TURNED;
    }
  }
  BUTTON_OLD = digitalRead(A2);
}


void setup()
{
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(LEFT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(SOUND, OUTPUT);

  int tim = millis();
  while (millis() - tim < 4000) {
    drive(180, -180);
    int left = analogRead(A0);
    int right = analogRead(A1);
    if (left < LEFT_MIN) LEFT_MIN = left;
    if (left > LEFT_MAX) LEFT_MAX = left;
    if (right < RIGHT_MIN) RIGHT_MIN = right;
    if (right > RIGHT_MAX) RIGHT_MAX = right;
  }
  drive(0, 0);
  pinMode(A2, INPUT);
  while (1) {
    if (digitalRead(A1) == 1) {
      delay(5);
      if (digitalRead(A1) == 1) {
        break;
      }
    }
  }
}


void loop()
{
  if (IS_TURNED) {
    int sensor1 = map(analogRead(A0), LEFT_MIN, LEFT_MAX, 0, 100);
    int sensor2 = map(analogRead(A1), RIGHT_MIN, RIGHT_MAX, 0, 100);
    check_line(sensor1, sensor2);

    if (sensor1 < 60 && sensor2 < 60) {
        recoverLine(LAST_DIRECTION);
    }
  }
  else {
    drive(0, 0);
  };

  start_button();
}
