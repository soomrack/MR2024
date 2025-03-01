#include <DHT11.h>

#define PIN_FAN 7 // вентилятор
#define PIN_LED_LIGHT 6 // включение света
#define PIN_PUMP 5 // включение насоса
#define PIN_HEATER 4 // нагреватель
#define LIGHT_SENSOR_PIN A0 // датчик света
#define PIN_WET_SOIL_SENSOR A1 // датчик почвы
#define PIN_DHT11 13 // датчик температуры и влажности


DHT11 dht11(13); // сообщаем на каком порту будет датчик


int middle_light, middle_wet_soil;


struct Micro_Climate {
  int box_tempurature;
  int soil_humidity; 
  float air_humidity;
  float box_light;
};


struct time {
  int hours; 
  int minutes;
  int seconds;
};


typedef struct Micro_Climate Micro_Climate;
typedef struct time time;


// Глобальный экземпляр структуры для хранения информации о микроклимате
Micro_Climate Greenhouse;


time initialTime = {0, 0, 0}; // Начальное время
time currentTime = {0, 0, 0}; // Текущее время
unsigned long startMillis = 0; // Время начала отсчета


// Функция для установки начального времени
void setInitialTime(int hours, int minutes, int seconds) {
  initialTime.hours = hours;
  initialTime.minutes = minutes;
  initialTime.seconds = seconds;
}


time timeDefining () {

  unsigned long totalMillis = millis(); // время с начала отсчета
  unsigned long totalSeconds = totalMillis / 1000; // Переводим миллисекунды в секунды

  // Вычисляем часы, минуты и секунды
  currentTime.seconds = initialTime.seconds + totalSeconds;
  currentTime.minutes = initialTime.minutes + (currentTime.seconds / 60);
  currentTime.hours = initialTime.hours + (currentTime.minutes / 60);

  // Корректируем секунды, минуты и часы
  currentTime.seconds %= 60;
  currentTime.minutes %= 60;
  currentTime.hours %= 24;

  return currentTime; // Возвращаем текущее время
}


int middleValue(const byte pin) 
{
  int first_value = analogRead(pin);

  delay(3000); // задержка 3 секунды, чтобы поместить датчик в другую среду

  int second_value = analogRead(pin);

  int middle_value = (first_value + second_value) / 2; // определяем границу между измененными средами

  return middle_value;
}


void ledTurning (const int middle_light) 
{
  Greenhouse.box_light = analogRead(LIGHT_SENSOR_PIN); // считываем уровень освещенности

  if ((Greenhouse.box_light > middle_light)) { // если темно
    if ((currentTime.hours < 22) && (currentTime.hours > 5)) { // если сейчас день, растениям нужен период ночи с 22:00 по 4:59. 
      digitalWrite(PIN_LED_LIGHT, HIGH);
    }
  }

  else { // выключить свет в теплице, если светло или сейчас ночь
    digitalWrite(PIN_LED_LIGHT, LOW);
  }
}


void pumpTurning (const int middle_wet_soil) {
  Greenhouse.soil_humidity = analogRead(PIN_WET_SOIL_SENSOR);

  if (Greenhouse.soil_humidity > middle_wet_soil) { // проверяем, сухая ли почва
    while (Greenhouse.soil_humidity > middle_wet_soil) { // пока почва не станет влажной, включаем помпу на 3 секунды, затем ждем, пока вода впитается и меряем влажность снова
      digitalWrite(PIN_PUMP, HIGH);

      delay(3000);

      digitalWrite(PIN_PUMP, LOW);

      delay(3000);

      Greenhouse.soil_humidity = analogRead(PIN_WET_SOIL_SENSOR);
    }
  }
}


void heaterTurning () {
  Greenhouse.box_tempurature = dht11.readTemperature(); // считываем значение температуры теплицы

  // если сейчас день
  if ((currentTime.hours < 22) && (currentTime.hours > 5)) { 
    if (Greenhouse.box_tempurature < 23) { // 23 градуса по Цельсию оптимальная температура для роста помидор днем
      digitalWrite(PIN_HEATER, HIGH); // если меньше 23 градусов то включаем нагреватель и вентилятор
      digitalWrite(PIN_FAN, HIGH);
    }
  }

  // если сейчас ночь
  else { 
    if (Greenhouse.box_tempurature < 18) { //оптимальная температура для роста помидор ночью
      digitalWrite(PIN_HEATER, HIGH); // если меньше 18 градусов то включаем нагреватель и вентилятор
      digitalWrite(PIN_FAN, HIGH);
    }

    else {
      digitalWrite(PIN_HEATER, LOW); // в остальных случаях вентилятор и нагреватель выключены
      digitalWrite(PIN_FAN, LOW);
    }
  }
}


//Включаем вентилятор каждые 3 часа на 5 минут
void airing (const int hours) {

  if ((initialTime.hours - hours) / 3) {
    digitalWrite(PIN_FAN, HIGH);

    delay(300000);

    digitalWrite(PIN_FAN, LOW);
  } 

  else {
    digitalWrite(PIN_FAN, LOW);
  }
}


void setup() {
  Serial.begin(9600);

  setInitialTime(18,44,30);

  pinMode(PIN_FAN, OUTPUT); // включаем пины на выход
  pinMode(PIN_LED_LIGHT, OUTPUT); 
  pinMode(PIN_PUMP, OUTPUT); 
  pinMode( PIN_HEATER, OUTPUT); 
  

  middle_light = middleValue(LIGHT_SENSOR_PIN); // определяем среднее значение для освещенности

  delay(5000); // задержка, чтобы успеть переключиться со считывания света на считывание влажности почвы

  middle_wet_soil = middleValue(PIN_WET_SOIL_SENSOR); // определяем среднее значение для влажности почвы

  Greenhouse.air_humidity = dht11.readHumidity(); // считываем значение влажности воздуха в теплице 
}

void loop() {
  
  time currentTime = timeDefining();

  ledTurning (middle_light);
  pumpTurning(middle_wet_soil);
  heaterTurning ();
  airing(currentTime.hours);


  // Выводим время в формате HH:MM:SS
  Serial.print("Current Time: ");
  Serial.print(currentTime.hours);
  Serial.print(":");
  Serial.print(currentTime.minutes);
  Serial.print(":");
  Serial.println(currentTime.seconds);

  delay(1000); // Задержка 1 секунда

}
