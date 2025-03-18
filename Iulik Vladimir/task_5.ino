#include <DHT.h>
#define DHTTYPE DHT11

#define DHTPIN 12 // влажность и тепмератуура воздуха
#define LAMP_PIN 6 // лампа
#define FAN_PIN 7 // вентилятор
#define NAGREV_PIN 4 // наргреватель
#define POMP_PIN 5 // помпа
#define HUM_SOIL_PIN A1 // влажность почвы
#define ILLUMINATION_PIN 9 // датчик света

DHT dht(DHTPIN, DHTTYPE);

float temp = 25.0;
float hum = 0.2;

boolean FLAG_POMP_SWITCH = false;
boolean FLAG_POMP_TIME = true;

unsigned long previousMillis = 0;
const long interval = 1000; 

int hours = 13;  // Начальное время (часы)
int minutes = 27; // Начальное время (минуты)
int seconds = 0; // Начальное время (секунды)

bool time_of_day(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    seconds++;  
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }
    if (hours >= 6 && hours < 18) {
      return true;
    } else {
      return false;
    }
  }
}


// Датчик освещенности
int Illumination_sensor(){
  return digitalRead(ILLUMINATION_PIN);
}

// Температура и влажность воздуха
void DHT_sensor(float *temp, float *hum){
  *temp = dht.readTemperature();
  *hum = dht.readHumidity();
}

// Влажность почвы
int Soil_humidity_sensor(){
  return analogRead(HUM_SOIL_PIN);
}

// Помпа
void Pomp_switch(const boolean pomp_flag){
  digitalWrite(POMP_PIN, pomp_flag);
}

// Нагреватель
void Heating_switch(const boolean heat_flag){
  digitalWrite(NAGREV_PIN, heat_flag);
}

// Вентилятор
void Ventilation_switch(const boolean fan_flag){
  digitalWrite(FAN_PIN, fan_flag);
}

// УФ лампа
void Lamp_switch(const boolean lamp_flag){
  digitalWrite(LAMP_PIN, lamp_flag);
}

void Illumination_control(){
  if (time_of_day())
  Lamp_switch(Illumination_sensor());
}

void Humidity_soil_control(){
  static unsigned long int myTime = 0;
  if (Soil_humidity_sensor() > 500 and FLAG_POMP_SWITCH == false) 
  {
    Pomp_switch(true);
    FLAG_POMP_SWITCH = true;
    myTime = millis(); 
  }
 else if (millis() - myTime > 5000){
    Pomp_switch(false);
    FLAG_POMP_SWITCH = false;
 }
}

void Fan_heat_control(){ 
  DHT_sensor(&temp, &hum);
  if (temp < 27.0 or hum > 30.0){
    Ventilation_switch(true);
    if (temp < 27.0 and FLAG_POMP_SWITCH == false) Heating_switch(true);
    else Heating_switch(false);
  }
  else Ventilation_switch(false);
}

void setup() {
  Serial.begin(9600);  
  dht.begin();

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(NAGREV_PIN, OUTPUT);
  pinMode(POMP_PIN, OUTPUT);

}

void loop() {
  time_of_day();
  Illumination_control();
  Humidity_soil_control();
  Fan_heat_control();
  
}
