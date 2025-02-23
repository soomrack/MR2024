#include "DHT.h"

#define PIN_MOISTURE_SENSOR A1    
#define PIN_LIGHT_SENSOR A0     
#define PIN_DHT 8              
#define PIN_PUMP 5             
#define PIN_LIGHT 6       
#define PIN_FAN 7              
#define PIN_HEATER 4

const int MIN_TEMP = 19;
const int NORMAL_TEMP = 26;
const int MAX_HUMIDITY = 90;
const int NORMAL_HUMIDITY = 60;
const int MIN_MOISTURE = 40;
const int NORMAL_MOISTURE = 60;
const int MIN_LIGHT = 30;

DHT dht(PIN_DHT, DHT11);
unsigned long start_time = 0;
const unsigned long DAY_TIME = 86400;
const unsigned long WEEK_TIME = DAY_TIME * 7;

float temperature = 0;
float humidity = 0;
int moisture = 0;
int light = 0;

bool is_dark = false;
bool is_cold = false;
bool is_hot = false;
bool is_soil_dry = false;
bool is_soil_wet = false;
bool is_air_wet = false;

unsigned long light_activation_time = 0;
unsigned long heater_activation_time = 0;
unsigned long pump_activation_time = 0;
unsigned long fan_activation_time = 0;


void print_status()
{
  int current_day = millis() / DAY_TIME;
  Serial.print("==Состояние теплицы==\n");
  Serial.print("Текущий день с начала работы: ");
  Serial.print(current_day);
  Serial.print("\nВлажность воздуха: ");
  Serial.print(humidity);
  Serial.print("\nТемпература воздуха: ");
  Serial.print(temperature);
  Serial.print("\nВлажность почвы: ");
  Serial.print(moisture);
}


void read_humidity()
{
  humidity = dht.readHumidity();

  if (humidity > MAX_HUMIDITY) {
    is_air_wet = true;
  } else if (humidity <= NORMAL_HUMIDITY) {
    is_air_wet = false;
  }
}


void read_temperature()
{
  temperature = dht.readTemperature();
  
  if (temperature <= MIN_TEMP) {
    is_cold = true;
  } else if (temperature >= NORMAL_TEMP) {
    is_cold = false;
  }
}


void read_moisture()
{
  moisture = map(analogRead(PIN_MOISTURE_SENSOR), 1023, 0, 0, 100);

  if (moisture < MIN_MOISTURE) {
    is_soil_dry = true;
  } else if (moisture >= NORMAL_MOISTURE) {
    is_soil_dry = false;
  }
}


void read_light()
{
  light = map(analogRead(PIN_LIGHT_SENSOR), 1023, 0, 0, 100);

  if (light < MIN_LIGHT) {
    is_dark = true;
  } else {
    is_dark = false;
  }
}


void read_sensors()
{
  read_light();
  read_moisture();
  read_temperature();
  read_humidity();
}


void manage_light()
{
  if (millis() - light_activation_time > 1000) {
    if (is_dark) {
      digitalWrite(PIN_LIGHT, HIGH);
      light_activation_time = millis();
    } else {
      digitalWrite(PIN_LIGHT, LOW);
    }
  }
}


void manage_pump()
{
  if (millis() - pump_activation_time > 3000) {
    if (is_soil_dry && millis() - pump_activation_time > 8000) {
      digitalWrite(PIN_PUMP, HIGH);
      pump_activation_time = millis();
    } else {
      digitalWrite(PIN_PUMP, LOW);
    }
  }
}


void manage_fan()
{
  if (is_cold || is_air_wet) {
    digitalWrite(PIN_FAN, HIGH);
  } else {
    digitalWrite(PIN_FAN, LOW);
  }
}


void manage_heater()
{
  if (is_cold) {
    digitalWrite(PIN_HEATER, HIGH);
  } else {
    digitalWrite(PIN_HEATER, LOW);
  }
}


void control_greenhouse()
{
  read_sensors();
  
  manage_light();
  manage_fan();
  manage_heater();
  manage_pump();

  print_status(); 
}


void setup() {
  Serial.begin(9600);

  pinMode(PIN_MOISTURE_SENSOR, INPUT);
  pinMode(PIN_LIGHT_SENSOR, INPUT);
  pinMode(PIN_DHT, INPUT);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);

  digitalWrite(PIN_PUMP, LOW);
  digitalWrite(PIN_LIGHT, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_HEATER, LOW);
}

void loop() {
  control_greenhouse();
}
