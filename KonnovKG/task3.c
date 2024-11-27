// Konnov, Tsypin, Polyakov

#define PIN_1_DIR 4
#define PIN_2_DIR 5
#define PIN_3_DIR 7
#define PIN_4_DIR 6
#define PIN_5_BEEP 9

int const COLOR_VALUE = 650;  // Показатель датчиков, соответствующий цвету линии
int const TURN_VALUE = 240;  // Скорость мотора, соответствующего направлению поворота
int const TURN_PAIR_VALUE = 160;  // Скорость мотора, противоположенного направлению поворота
int const FORWARD_VALUE = 175;  // Скорость мотора при движении вперед
int const TIMEOUT_VALUE = 7500;  // Время поиска линии перед отключением, мс

void init_motors()
{
    pinMode(PIN_1_DIR, OUTPUT);
    pinMode(PIN_2_DIR, OUTPUT);
    pinMode(PIN_3_DIR, OUTPUT);
    pinMode(PIN_4_DIR, OUTPUT);
}


int button_read(int ButtonPin)
{
  int Data = digitalRead(ButtonPin);
  if(ButtonPin == 12) Data = !Data;
  return Data;
}


void motors(int Speed1, int Speed2)  // Запуск моторов
{
    if (Speed1 > 255)
        Speed1 = 255;
    if (Speed1 < -255)
        Speed1 = -255;
    if (Speed2 > 255)
        Speed2 = 255;
    if (Speed2 < -255)
        Speed2 = -255;

    if (Speed1 > 0)
    {
        digitalWrite(PIN_1_DIR, 1);
        analogWrite(PIN_2_DIR, Speed1);
    }
    else
    {
        digitalWrite(PIN_1_DIR, 0);
        analogWrite(PIN_2_DIR, -Speed1);
    }

    if (Speed2 > 0)
    {
        digitalWrite(PIN_3_DIR, 1);
        analogWrite(PIN_4_DIR, Speed2);
    }
    else
    {
        digitalWrite(PIN_3_DIR, 0);
        analogWrite(PIN_4_DIR, -Speed2);
    }
}


void move_forward(int Speed)
{
    motors(Speed, Speed);
}


void move_right(int Speed_1, int Speed_2)
{
    motors(Speed_1, -Speed_2);
}

void move_left(int Speed_1, int Speed_2)
{
    motors(-Speed_1, Speed_2);
}


int line_sensor_read(int SensorPin)  // Считывание данных с датчиков
{
    if (SensorPin < A0 || SensorPin > A3)
        return -1;

    int Data = analogRead(SensorPin);
    return Data;
}


void Stop()
{
    motors(0, 0);
}


void piezo_beep(int PiezoPin, int Tone, int TimeON, int Count)  // Звуковое сопровождение
{
    for(int i = 0; i < Count; i++)
    {
      tone(PiezoPin, Tone);
      delay(TimeON);
      noTone(PiezoPin);
    }
}


void sound_alarm()
{
    while ( button_read(9) )
    {
        piezo_beep(8, 3000, 3000, 1);
    }
}


void setup()
{
    pinMode(PIN_5_BEEP, INPUT_PULLUP);
    init_motors();
    Serial.begin(9600);
}


int flag = 1;


void find_line()
{
    int speed = 70;
    int radius = 160;
    unsigned long time = millis();

    while (analogRead(A0) < COLOR_VALUE && analogRead(A1) < COLOR_VALUE)
    { 
        unsigned long left_time = millis();
        while (millis() - left_time < 51)
        {
            move_left(radius, radius-69);
        }
        unsigned long forward_time = millis();
        while (millis() - forward_time < 71)
        {
            move_forward(speed);
        }

        radius = radius + 15;
        speed = speed + 7;

        if (millis() - time > TIMEOUT_VALUE)
        {
            Stop();
            flag = 0;
        }
    }
}


void ride_by_line()
{
    if (line_sensor_read(A0) > COLOR_VALUE && line_sensor_read(A1) > COLOR_VALUE)
    {
        move_forward(FORWARD_VALUE);
    }
    else if (line_sensor_read(A1) > COLOR_VALUE && line_sensor_read(A0) < COLOR_VALUE)
    {
        move_left(TURN_VALUE, TURN_PAIR_VALUE);
    }
    else if (line_sensor_read(A0) > COLOR_VALUE && line_sensor_read(A1) < COLOR_VALUE)
    {
        move_right(TURN_VALUE, TURN_PAIR_VALUE);
    }
    else
    {
        find_line();
    }
}


void loop()
{
    sound_alarm();
    if (flag == 1) {ride_by_line();};
}
