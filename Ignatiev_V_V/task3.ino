// Определение используемых пинов
#define DIR_R_PIN 4
#define DIR_L_PIN 7
#define PWR_R_PIN 5
#define PWR_L_PIN 6
#define SENS_R_PIN A0
#define SENS_L_PIN A3
#define BTN_PIN 8

typedef unsigned int light;  // Тип данных для хранения показателей фоторезисторов
typedef unsigned int time; // Тип данных для таймера

light cur_color_L; // Текущие показания левого фоторезистора 
light cur_color_R; // Текущие показания правого фоторезистора
light color_gray; // Контрольный граничный показатель цвета
light last_color_L; // Последние показания левого фоторезистора для ситуативного выбора стиля поведения

time search_time; // Таймер (мс), контролирующий время нахождения вне линии


void pin_init() // Инициализация пинов
{
    pinMode(DIR_R_PIN, OUTPUT);
    pinMode(DIR_L_PIN, OUTPUT);
    pinMode(PWR_R_PIN, OUTPUT);
    pinMode(PWR_L_PIN, OUTPUT);
    pinMode(SENS_R_PIN, INPUT);
    pinMode(SENS_L_PIN, INPUT);
    pinMode(BTN_PIN, INPUT);
}


void color_ident(light color) // Определение текущего цвета
{
    light sens_R = analogRead(SENS_R_PIN);
    light sens_L = analogRead(SENS_L_PIN);
    color = (sens_R + sens_L) / 2;
}


void forward(time search_time, const light color_L, const light color_R) // Движение вперед с плавным разгоном при датчиках Ч-Ч
{
    if (color_L > color_gray && color_R > color_gray) {
    
        search_time = 0;

        digitalWrite(DIR_R_PIN, 1);
        digitalWrite(DIR_L_PIN, 1);

        for (int i = 50; i <= 255; i += 6) {
            analogWrite(PWR_R_PIN, i);
            analogWrite(PWR_L_PIN, i);
        }
    }
}


void left(time search_time, const light color_L, const light color_R) // Поворот налево плавный при датчиках Ч-Б 
{
    if (color_L > color_gray && color_R < color_gray) {

        search_time = 0;
    
        digitalWrite(DIR_R_PIN, 1);
        digitalWrite(DIR_L_PIN, 0);

        for (int i = 0; i <= 150; i += 6) {
            analogWrite(PWR_R_PIN, i);
        }

        for (int i = 0; i <= 150; i += 6) {
            analogWrite(PWR_L_PIN, i);
        }

        delay(30); // Задержка для снижения дерганности

    }
}


void right(time search_time, const light color_L, const light color_R) // Поворот направно плавный при датчиках Б-Ч 
{
    if (color_L < color_gray && color_R > color_gray) {
    
        search_time = 0;
    
        digitalWrite(DIR_R_PIN, 0);
        digitalWrite(DIR_L_PIN, 1);

        for (int i = 0; i <= 150; i += 6) {
            analogWrite(PWR_R_PIN, i);
        }

        for (int i = 0; i <= 150; i += 6) {
            analogWrite(PWR_L_PIN, i);
        }

        delay(30);
    }
}


void search(time search_time, const light color_L, const light color_R, const light last_color_L) // Вращение на месте при съезде с линии при датчиках Б-Б
{
    if (color_L < color_gray && color_R < color_gray) {

      // Работа таймера нахождения в состоянии неопределенности
      search_time++;
      delay(1);

      // Ситуативный выбор стиля поведения в зависимости от последнего состояния определенности
      if (last_color_L > color_gray) {

          digitalWrite(DIR_L_PIN, 0);
          analogWrite(PWR_L_PIN, 150); 

          digitalWrite(DIR_R_PIN, 0);
          analogWrite(PWR_R_PIN, 0);
      
      } else {

          digitalWrite(DIR_L_PIN, 0);
          analogWrite(PWR_L_PIN, 0);

          digitalWrite(DIR_R_PIN, 0);
          analogWrite(PWR_R_PIN, 150);

      }
  }

    if (search_time >= 10 * 1000) {
        analogWrite(PWR_L_PIN, 0);
        analogWrite(PWR_R_PIN, 0);

        end_music();
        exit(1);
    }
}


void end_music() // Сигнал прекращения попыток возврата на линию
{
    tone(9, 261.63);
    delay(600);
    tone(9, 329.63);
    delay(600);
    tone(9, 392);
    delay(600);
    noTone(9);
}


void setup() 
{
    pin_init();

    while (!digitalRead(BTN_PIN)) {}

    light color_white;
    color_ident(color_white);

    delay(1000);  // Ожидание, чтобы избежать ложного нажатия на кнопку
    while (!digitalRead(BTN_PIN)) {}

    light color_black;
    color_ident(color_black);

    color_gray = (color_white + color_black) / 2;

    delay(1000);
    while (!digitalRead(BTN_PIN)) {}

}


void loop() 
{
    // Определение текущего цвета на фоторезисторах
    cur_color_L = analogRead(SENS_L_PIN);
    cur_color_R = analogRead(SENS_R_PIN);

    forward(search_time, cur_color_L, cur_color_R);
    left(search_time, cur_color_L, cur_color_R);
    right(search_time, cur_color_L, cur_color_R);
    search(search_time, cur_color_L, cur_color_R, last_color_L);

    last_color_L = cur_color_L;

}
