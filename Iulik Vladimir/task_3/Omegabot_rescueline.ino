#define LEFT_PWM 6
#define LEFT_DIR 7
#define RIGHT_PWM 5
#define RIGHT_DIR 4
#define SOUND 9

#define K_P 7.0
#define K_D 4.5

#define V 200

int TIME_SOUND = 0;
int LEFT_MIN = 1023;
int LEFT_MAX = 0;
int RIGHT_MIN = 1023;
int RIGHT_MAX = 0;

int LAST_DIRECTION = 0;

int ERROLD = 0;

bool FLAG = false;
bool BUTTON_OLD = 1;

bool RECOVERING = false; // Флаг восстановления линии


// Функция управления движением
void drive(int left, int right) 
{    
  digitalWrite(LEFT_DIR, left > 0);
  digitalWrite(RIGHT_DIR, right > 0);

  analogWrite(LEFT_PWM, abs(left));
  analogWrite(RIGHT_PWM, abs(right));
}


// Функция восстановления линии с дуговым движением
void recoverLine(int LAST_DIRECTION) 
{
  RECOVERING = true; // Флаг восстановления линии
  if (LAST_DIRECTION == 0)/*Толкнули влево (черный на правом датчике)*/{
    drive(-20, 240);
    //ERROLD = 100;
  }
  else/*Толкнули вправо (черный на левом датчике)*/{
    drive(240, -20);
    //ERROLD = -100;
  }
  
}


void end_music() // Сигнал прекращения попыток возврата на линию
{
    tone(SOUND, 1000, 2000);
}


void check_line(int s1, int s2)
{
  if (s1 < 60 && s2 < 60) {
      if (millis() - TIME_SOUND > 5 * 1000) {
        end_music();
        flag = 0;
      }
      recoverLine(LAST_DIRECTION); // Вызов функции восстановления линии
    } else {
        TIME_SOUND = 0;
        double err = (s1 - s2);
        double u = err * K_P + (err - ERROLD) * K_D;
        drive(constrain(V + u, -250, 250), constrain(V - u, -250, 250));
        ERROLD = err;
        if (s2 > s1)/*толкнули влево*/{
          LAST_DIRECTION = 0;
        }
        else{
          LAST_DIRECTION = 1;
        }
        TIME_SOUND = millis();
      } 
}


void start_button()
{
   if (digitalRead(A2) == 1 && BUTTON_OLD == 0) {
   delay(5);
   if (digitalRead(A2) == 1) {
     FLAG = !FLAG; 
    }
  }
  BUTTON_OLD = digitalRead(A2);
}


void read_sensors()
{
  if (FLAG) {
    int s1 = map(analogRead(A0), LEFT_MIN, LEFT_MAX, 0, 100);
    int s2 = map(analogRead(A1), RIGHT_MIN, RIGHT_MAX, 0, 100);
    // Проверяем, потеряна ли линия
    check_line(s1, s2);
  }
  else {
    drive(0, 0);
  }  
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
  read_sensors();
    
  // Обработка кнопки включения/выключения режима
  start_button();
}
