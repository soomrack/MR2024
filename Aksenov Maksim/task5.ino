#include <DHT11.h>

// Sensors:
#define EXTERNAL_LIGHT_PIN A0
#define SOIL_HUMI_PIN A1
#define AIR_PIN 12

DHT11 dht11(AIR_PIN);

// Actuators:
#define WATER_PUMP_PIN 5
#define LED_LIGHT_PIN 6
#define FAN_PIN 7
#define HEAT_PIN 4


struct Climate 
{
  int temp_min;
  int temp_max;

  int soil_humi_min;
  int soil_humi_max;
  
  int air_humi_min;
  int air_humi_max;
  
  int light_min;
  int light_max;
  
  int pump_work_time;
  int pump_dead_time;
  
  int light_duration;
  int ventilation_duration;
  int sleep_start;
  int sleep_end;
};


Climate currentClimate = // Default climate
{
  240, // TEMP_MIN
  260, // TEMP_MAX
  
  550, // SOIL_HUMI_MIN
  540, // SOIL_HUMI_MAX
  
  20, // AIR_HUMI_MIN
  30, // AIR_HUMI_MAX
  
  600,   // LIGHT_MIN
  700, // LIGHT_MAX
  
  1/3 * 60 * 1000, // Pump work min 20 sec
  1/2 * 60 * 1000, // Pump dead 30 sec
  
  1/60 * 60 * 1000, // Light min 1 sec
  1 * 60 * 1000,  // Vent min 1 minute

  20, // Sleep in (hours)
  8 // Wake up in (hours)
};


Climate tomatoes = 
{
  320,   // TEMP_MIN
  340, // TEMP_MAX
  
  450,   // SOIL_HUMI_MIN
  420, // SOIL_HUMI_MAX
  
  15,    // AIR_HUMI_MIN
  20, // AIR_HUMI_MAX
  
  650,   // LIGHT_MIN
  710, //LIGHT_MAX

  2 * 60 * 1000, // Pump 2 min
  5 * 60 * 1000, // Pump dead 5 min
  5 * 60 * 1000, // Light 5 min
  3 * 60 * 1000,  // Vent 3 min
  21, // Sleep in (hours)
  7 // Wake up in (hours)
};


Climate cucumbers = 
{
  280,   // TEMP_MIN
  300, // TEMP_MAX
  
  580,   // SOIL_HUMI_MIN
  550, // SOIL_HUMI_MAX
  
  10,    // AIR_HUMI_MIN
  15, // AIR_HUMI_MAX
  
  600,   // LIGHT_MIN
  700, //LIGHT_MAX

  2 * 60 * 1000,  // Pump 2 min
  5 * 60 * 1000, // Pump dead 5 min
  7 * 60 * 1000,  // Light 7 min
  10 * 60 * 1000,   // Vent 10 min
  19, // Sleep in (hours)
  6 // Wake up in (hours)
};


typedef unsigned long int time;

time cur_millis;
unsigned int cur_hour, cur_min, cur_sec;
unsigned int ext_light, soil_humi, air_humi, air_temp;
bool allow_heating = 1;


void pin_init() 
{
  pinMode(EXTERNAL_LIGHT_PIN, INPUT);
  pinMode(SOIL_HUMI_PIN, INPUT);

  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
}


void RTC_set(unsigned int hour_to_set, unsigned int min_to_set, unsigned int sec_to_set)
{
  cur_hour = hour_to_set;
  cur_min= min_to_set;
  cur_sec = sec_to_set;
}


void RTC_count()
{
  static time last_millis = cur_millis;

  if ((cur_millis - last_millis) >= 1000)
  {
    cur_sec = cur_sec + (cur_millis - last_millis) / 1000;
    last_millis = cur_millis;
  }
  
  if (cur_sec >= 60)
  {
    cur_min++;
    cur_sec = cur_sec - 60;
  }

  if (cur_min == 60)
  {
    cur_hour++;
    cur_min = 0;
  }

  if (cur_hour == 24)
  {
    cur_hour = 0;
  }
}


void RTC_print()
{
  Serial.println("------------------------------");
  Serial.println("Time: " + String(cur_hour) + ":" + String(cur_min) + ":" + String(cur_sec));
}


void on(int PIN) 
{
  digitalWrite(PIN, 1);
}


void off(int PIN) 
{
  digitalWrite(PIN, 0);
}


void lighting_check()
{
  static time t_light;

  if ((cur_hour < currentClimate.sleep_end) || (cur_hour >= currentClimate.sleep_start))
  {
    off(LED_LIGHT_PIN);
    //Serial.println("NIGHT"); // For debugging
    return;
  }

  if ((ext_light >= currentClimate.light_min) && (digitalRead(LED_LIGHT_PIN) == 0)) {
    on(LED_LIGHT_PIN);
    t_light = cur_millis;
  }

  if ((cur_millis - t_light >= currentClimate.light_duration) && (ext_light <= currentClimate.light_max)) {
    off(LED_LIGHT_PIN);
  }
}


void watering_check() 
{
  static time p_time;
  static time dead_time_start;

  if ((soil_humi >= currentClimate.soil_humi_min) && (digitalRead(WATER_PUMP_PIN) == 0) && (cur_millis - dead_time_start >= currentClimate.pump_dead_time)) {
    allow_heating = 0;
    on(WATER_PUMP_PIN);
    p_time = cur_millis;
  }

  if ((digitalRead(WATER_PUMP_PIN) == 1) && (cur_millis - p_time >= currentClimate.pump_work_time) && (soil_humi <= currentClimate.soil_humi_max)) {
    off(WATER_PUMP_PIN);
    allow_heating = 1;
    dead_time_start = cur_millis;
  }
}


void heating_check() 
{
  static time t_heat;

  if (!allow_heating) {
    off(FAN_PIN);
    off(HEAT_PIN);
    //Serial.println("HEAT OFF"); // For debugging
    return;
  }

  if ((air_temp <= currentClimate.temp_min) && (digitalRead(FAN_PIN) == 0)) {
    on(HEAT_PIN);
    on(FAN_PIN);
    t_heat = cur_millis;
    //Serial.println("HEAT ON"); // For debugging
  }

  if ((cur_millis - t_heat >= currentClimate.ventilation_duration) && (air_temp >= currentClimate.temp_max)) {
    off(FAN_PIN);
    off(HEAT_PIN);
  }
}


void airing_check()
{
  static time t_air;

  if ((air_humi >= currentClimate.air_humi_max) && (digitalRead(FAN_PIN) == 0)) {
    on(FAN_PIN);
    t_air = cur_millis;
  }

  if ((cur_millis - t_air >= currentClimate.ventilation_duration) && (air_humi <= currentClimate.air_humi_min)) {
    off(FAN_PIN);
  }
}

void pump_temp_print()
{
  Serial.println("------------------------------");
  Serial.println("Status temp: " + String(digitalRead(HEAT_PIN)));
  Serial.println("Status pump: " + String(digitalRead(WATER_PUMP_PIN)));

  Serial.println("Allow heating: " + String(allow_heating));
}

void sensor_print() 
{
  Serial.println("------------------------------");
  Serial.print("Light: "); Serial.println(ext_light);
  Serial.print("Soil humidity: "); Serial.println(soil_humi);
  Serial.print("Air humidity: "); Serial.println(air_humi);
  Serial.print("Air temperature: "); Serial.println(air_temp);
}


void setup() 
{
  //currentClimate = tomatoes; // Comment to use standard settings
  pin_init();
  RTC_set(19,59,16);

  Serial.begin(9600); // For debugging
}


void loop() 
{
  //Sensors_info
  ext_light = analogRead(EXTERNAL_LIGHT_PIN);
  soil_humi = analogRead(SOIL_HUMI_PIN);
  air_humi = dht11.readHumidity();
  air_temp = dht11.readTemperature();
  cur_millis = millis();
  
  RTC_count();

  // For debugging 
  RTC_print();
  sensor_print();
  pump_temp_print();
  //

  //Actuators_actions
  lighting_check();
  watering_check();
  airing_check();
  heating_check();

  delay(1000);
}
