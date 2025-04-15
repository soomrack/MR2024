#define LEFT_PWM 6
#define LEFT_DIR 7
#define RIGHT_PWM 5
#define RIGHT_DIR 4

#define K_P 7 // пропорциональный коэффициент 
#define K_D 3 // дифференц. коэффициент

#define V 200 // базовая скорость движения робота

long long int TIME_LOST = 0; // время потери линии
int LEFT_MIN = 1023; // переменные для калибровки робота
int LEFT_MAX = 0;
int RIGHT_MIN = 1023;
int RIGHT_MAX = 0;

int LAST_DIRECTION = 0;

int ERROLD = 0;

bool drive_control = true;

bool returning = false; // Флаг восстановления линии

// Функция управления движением
void drive(int left, int right)
{
  digitalWrite(LEFT_DIR, left > 0);
  digitalWrite(RIGHT_DIR, right > 0);
  analogWrite(LEFT_PWM, abs(left));
  analogWrite(RIGHT_PWM, abs(right));
}

// Функция восстановления линии с дуговым движением
void returnLine(int LAST_DIRECTION)
{
  returning = true; // Флаг восстановления линии
  if (LAST_DIRECTION == 0) { //Толкнули влево (черный на правом датчике)
    drive(-10, 240);
  }
  else { //Толкнули вправо (черный на левом датчике)
    drive(240, -10);
  }
}


void stopCar() // Функция остановки машинки
{
  drive(0, 0);
  drive_control = false;
}

void check_line(int sensor_left, int sensor_right) // Функция проверки линии
{
  if (sensor_left < 60 && sensor_right < 60) {
    if (TIME_LOST == 0) {
      TIME_LOST = millis();
    }
    if (millis() - TIME_LOST > 8000) {
      stopCar();
    }
    returnLine(LAST_DIRECTION); // Вызов функции восстановления линии
  } else {
    TIME_LOST = 0;
    double err = (sensor_left - sensor_right);
    double u = err * K_P + (err - ERROLD) * K_D;
    drive(constrain(V + u, -250, 250), constrain(V - u, -250, 250));
    ERROLD = err;
    if (sensor_right > sensor_left) { //толкнули влево
      LAST_DIRECTION = 0;
    }
    else {
      LAST_DIRECTION = 1;
    }
  }
}


void setup()
{
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(LEFT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);

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
}

void loop()
{
  if (drive_control) {
    int sensor_left = map(analogRead(A0), LEFT_MIN, LEFT_MAX, 0, 100);
    int sensor_right = map(analogRead(A1), RIGHT_MIN, RIGHT_MAX, 0, 100);
    check_line(sensor_left, sensor_right);
  }
  else {
    drive(0, 0);
  }
}