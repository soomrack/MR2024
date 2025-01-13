#define LEFT_PWM 6
#define LEFT_DIR 7
#define RIGHT_PWM 5
#define RIGHT_DIR 4
#define SOUND 9

#define K_P 7.0
#define K_D 4.5

#define V 200

int time_sound = 0;
int left_min = 1023;
int left_max = 0;
int right_min = 1023;
int right_max = 0;

int left_old = 0;
int right_old = 0;

int last_direction = 0;
int return_direction = 0;

int errold = 0;

bool flag = false;
bool button_old = 1;

bool recovering = false; // Флаг восстановления линии


// Функция управления движением
void drive(int left, int right) {    
  digitalWrite(LEFT_DIR, left > 0);
  digitalWrite(RIGHT_DIR, right > 0);

  analogWrite(LEFT_PWM, abs(left));
  analogWrite(RIGHT_PWM, abs(right));
}


// Функция восстановления линии с дуговым движением
void recoverLine(int last_direction) {
  recovering = true; // Флаг восстановления линии
  if (last_direction == 0)/*Толкнули влево (черный на правом датчике)*/{
    drive(-20, 240);
    //errold = 100;
  }
  else/*Толкнули вправо (черный на левом датчике)*/{
    drive(240, -20);
    //errold = -100;
  }
  
}


void end_music() // Сигнал прекращения попыток возврата на линию
{
    tone(SOUND, 1000, 2000);
}


void check_line(int s1, int s2)
{
  if (s1 < 60 && s2 < 60) {
      if (millis() - time_sound > 5 * 1000) {
        end_music();
        flag = 0;
      }
      recoverLine(last_direction); // Вызов функции восстановления линии
    } else {
        time_sound = 0;
        double err = (s1 - s2);
        double u = err * K_P + (err - errold) * K_D;
        drive(constrain(V + u, -250, 250), constrain(V - u, -250, 250));
        errold = err;
        if (s2 > s1)/*толкнули влево*/{
          last_direction = 0;
        }
        else{
          last_direction = 1;
        }
        time_sound = millis();
      } 
}


void start_button()
{
   if (digitalRead(A2) == 1 && button_old == 0) {
   delay(5);
   if (digitalRead(A2) == 1) {
     flag = !flag; 
    }
  }
  button_old = digitalRead(A2);
}


void read_sensors()
{
  if (flag) {
    int s1 = map(analogRead(A0), left_min, left_max, 0, 100);
    int s2 = map(analogRead(A1), right_min, right_max, 0, 100);
    // Проверяем, потеряна ли линия
    check_line(s1, s2);
  }
  else {
    drive(0, 0);
  }  
}


void setup() {
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
    if (left < left_min) left_min = left;
    if (left > left_max) left_max = left; 
    if (right < right_min) right_min = right;
    if (right > right_max) right_max = right;
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


void loop() {
  read_sensors();
    
  // Обработка кнопки включения/выключения режима
  start_button();
}
