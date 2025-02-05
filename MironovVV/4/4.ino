#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

const int LIGHT_THRESHOLD = 70;
const int SOIL_HUMIDITY_THRESHOLD = 40;
const int AIR_HUMIDITY_THRESHOLD = 70;
const int TEMP_HIGH_THRESHOLD = 25;
const int TEMP_LOW_THRESHOLD = 20;
const int FAN_TIMER = 3;
const int TIME_SUNRISE = 7;
const int TIME_SUNSET = 20;
const int PUMP_ON_TIME = 2000;
const int PUMP_OFF_TIME = 3000;

const int FAN_PIN = 7;
const int LIGHT_PIN = 6;
const int PUMP_PIN = 5;
const int HEATER_PIN = 4;

const int LIGHT_SENSOR_PIN = A0;
const int SOIL_SENSOR_PIN = A1;
const int DHT11_PIN = 13;

int fan_humidity_state = 0;
int fan_temperature_state = 0;
int fan_timer_state = 0;

int light_sensor_state = 0;
int light_timer_state = 0;

int pump_humidity_state = 0;
int pump_timer_state = 0;

int heater_state = 0;

int air_humidity;
int air_temperature;
int light_level;
int soil_humidity;
int current_time;

void data_print() {
  static long old_time;
  if (millis() - old_time > 2000)
  {
    Serial.print("Light: "); Serial.println(light_level);
    Serial.print("Soil Humidity: "); Serial.println(soil_humidity);
    Serial.print("Air Humidity: "); Serial.println(air_humidity);
    Serial.print("Temperature: "); Serial.println(air_temperature);
    Serial.print("Current time: "); Serial.println(current_time);
    Serial.println();
    old_time = millis();
  }
}

void data_read() {
  DHT.read(DHT11_PIN);
  air_humidity = DHT.humidity;
  air_temperature = DHT.temperature;
  light_level = map(analogRead(LIGHT_SENSOR_PIN), 980, 38, 0, 100);
  soil_humidity = map(analogRead(SOIL_SENSOR_PIN), 1023, 0, 0, 100);
  current_time = millis() / 2000 % 24;
}

void light_check() {
  if (TIME_SUNRISE < current_time && current_time < TIME_SUNSET) light_timer_state = 1;
  else light_timer_state = 0;

  if (light_level < LIGHT_THRESHOLD) light_sensor_state = 1;
  else light_sensor_state = 0;
}

void temperature_check() {
  if (air_temperature < TEMP_LOW_THRESHOLD)
  {
    heater_state = 1;
    fan_temperature_state = 1;
  }
  else if (air_temperature > TEMP_HIGH_THRESHOLD)
  {
    heater_state = 0;
    fan_temperature_state = 1;
  }
  else
  {
    heater_state = 0;
    fan_temperature_state = 0;
  }
}

void fan_check(){
  if (current_time % FAN_TIMER == 0) fan_timer_state = 1;
  else fan_timer_state = 0;
}

void pump_check(){
  if (soil_humidity < SOIL_HUMIDITY_THRESHOLD) pump_humidity_state = 1;
  else pump_humidity_state = 0;
  
  static long old_time;
  if (millis() - old_time >= 0 && millis() - old_time < PUMP_ON_TIME)
  {
    pump_timer_state = 1;
  }
  else if (millis() - old_time >= PUMP_ON_TIME && millis() - old_time < PUMP_ON_TIME + PUMP_OFF_TIME)
  {
    pump_timer_state = 0;
  }
  else old_time = millis();
}

void actuate() {
  if (light_sensor_state && light_timer_state) digitalWrite(LIGHT_PIN, HIGH);
  else digitalWrite(LIGHT_PIN, LOW);

  if (heater_state) digitalWrite(HEATER_PIN, HIGH);
  else digitalWrite(HEATER_PIN, LOW);

  if (fan_humidity_state || fan_temperature_state || fan_timer_state) digitalWrite(FAN_PIN, HIGH);
  else digitalWrite(FAN_PIN, LOW);

  if (pump_timer_state && pump_humidity_state) digitalWrite(PUMP_PIN, HIGH);
  else digitalWrite(PUMP_PIN, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);

  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(DHT11_PIN, INPUT);

  data_read();
  data_print();
  delay(1000);
}

void loop() {
  static long old_time;
  if (millis() - old_time > 1000)
  {
    data_read();
    data_print();
    old_time = millis();
  }

  light_check();
  temperature_check();
  fan_check();
  pump_check();

  actuate();

  delay(1);
}
