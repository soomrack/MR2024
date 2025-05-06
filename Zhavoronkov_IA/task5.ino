#include <DHT11.h>


// Sensors:
#define EXTERNAL_LIGHT_PIN A0
#define SOIL_HUMI_PIN A1
#define AIR1_PIN 12
#define AIR2_PIN 13
#define AIR3_PIN 14

DHT11 dht11_1(AIR1_PIN);
DHT11 dht11_2(AIR2_PIN);
DHT11 dht11_3(AIR3_PIN);


// Actuators:
#define WATER_PUMP_PIN 5
#define LED_LIGHT_PIN 6
#define FAN_PIN 7
#define HEAT_PIN 4


struct Climate 
{
  int temperature_min;
  int temperature_max;

  int soil_humidity_min;
  int soil_humidity_max;
  
  int air_humidity_min;
  int air_humidity_max;
  
  int light_min;
  int light_max;
  
  int pump_work_time;
  int pump_dead_time;
  
  int light_duration;

  int start_ventilation;
  int end_ventilation;
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
  1/3 * 60 * 1000, // Pump dead 20 sec
  
  1/60 * 60 * 1000, // Light min 1 sec

  16, // Start ventilation
  17, // End ventilation
  1/60 * 60 * 1000,  // Vent min 1 minute

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

  16, // Start ventilation
  17, // End ventilation
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

  16, // Start ventilation
  17, // End ventilation
  10 * 60 * 1000,   // Vent 10 min

  19, // Sleep in (hours)
  6 // Wake up in (hours)
};


typedef unsigned long int time;

time cur_millis;
unsigned int cur_hour, cur_min, cur_sec;
unsigned int ext_light, soil_humidity, air_humidity, average_temperature, air_temperature_1, air_temperature_2, air_temperature_3;
bool is_light, is_watering, is_airing, is_heating; 
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


void pump_temperature_print()
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
  Serial.print("Soil humidity: "); Serial.println(soil_humidity);
  Serial.print("Air humidity: "); Serial.println(air_humidity);
  Serial.print("Air temperature: "); Serial.println(average_temperature);
}


void get_light()
{
  ext_light = analogRead(EXTERNAL_LIGHT_PIN);
}


void get_soil_humidity()
{
  soil_humidity = analogRead(SOIL_HUMI_PIN);
}


void get_air_humidity()
{
  air_humidity = dht11_1.readHumidity();
}


void get_temperature()
{
  static int delta_1, delta_2, delta_3, delta;

  air_temperature_1 = dht11_1.readTemperature();
  air_temperature_2 = dht11_2.readTemperature();
  air_temperature_3 = dht11_3.readTemperature();

  delta_1 = abs(air_temperature_1 - air_temperature_2);    
  delta_2 = abs(air_temperature_1 - air_temperature_3);
  delta_3 = abs(air_temperature_2 - air_temperature_3);
  delta = min(delta_1, min(delta_2, delta_3));

  if (delta == delta_1) {average_temperature = (air_temperature_1 + air_temperature_2) / 2;}
  if (delta == delta_2) {average_temperature = (air_temperature_1 + air_temperature_3) / 2;}
  if (delta == delta_3) {average_temperature = (air_temperature_2 + air_temperature_3) / 2;}
}


void get_time()
{
  cur_millis = millis();
}


void control_light()
{
  static time t_light;

  if ((cur_hour < currentClimate.sleep_end) || (cur_hour >= currentClimate.sleep_start)) {
    is_light = 0;
    return;
  }

  if ((ext_light >= currentClimate.light_min) && (digitalRead(LED_LIGHT_PIN) == 0)) {
    is_light = 1;
    t_light = cur_millis;
  }

  if ((cur_millis - t_light >= currentClimate.light_duration) && (ext_light <= currentClimate.light_max)) {is_light = 0;}
}


void control_water()
{
  static time p_time;
  static time dead_time_start;

  if ((soil_humidity >= currentClimate.soil_humidity_min) && (digitalRead(WATER_PUMP_PIN) == 0) && (cur_millis - dead_time_start > currentClimate.pump_dead_time)) {
    allow_heating = 0;
    is_watering = 1;
    p_time = cur_millis;
  }

  if ((digitalRead(WATER_PUMP_PIN) == 1) && (cur_millis - p_time >= currentClimate.pump_work_time) && (soil_humidity <= currentClimate.soil_humidity_max)) {
    allow_heating = 1;    
    is_watering = 0;
    dead_time_start = cur_millis;
  }
}


void control_air()
{
  static time t_air = cur_millis;

  if((cur_hour >= currentClimate.start_ventilation) && (cur_hour < currentClimate.end_ventilation)) {
    is_airing = 1;
    return;
  }

  if ((air_humidity >= currentClimate.air_humidity_max) && (digitalRead(FAN_PIN) == 0)) {
    is_airing = 1;
    t_air = cur_millis;
    return;
  }

  if ((cur_millis - t_air >= currentClimate.ventilation_duration) && (air_humidity <= currentClimate.air_humidity_min)) {
    is_airing = 0;
    return;
  }
}


void control_temperature()
{
  static time t_heat;

  if (!allow_heating) {
    is_airing = 0;
    is_heating = 0;
    return;
  }

  if ((average_temperature <= currentClimate.temperature_min) && (digitalRead(FAN_PIN) == 0)) {
    is_airing = 1;
    is_heating = 1;
    t_heat = cur_millis;
  }

  if ((cur_millis - t_heat >= currentClimate.ventilation_duration) && (average_temperature >= currentClimate.temperature_max)) {
    is_airing = 0;
    is_heating = 0;
  }
}


void do_light()
{
  if (is_light) {
      digitalWrite(LED_LIGHT_PIN, 1);
  } else {
      digitalWrite(LED_LIGHT_PIN, 0);
  }
}


void do_water()
{
  if (is_watering) {
      digitalWrite(WATER_PUMP_PIN, 1);
  } else {
      digitalWrite(WATER_PUMP_PIN, 0);
  }
}


void do_air()
{
  if (is_airing) {
      digitalWrite(FAN_PIN, 1);
  } else {
      digitalWrite(FAN_PIN, 0);
  }
}


void do_heat()
{
  if (is_heating) {
      digitalWrite(HEAT_PIN, 1);
  } else {
      digitalWrite(HEAT_PIN, 0);
  }
}

void setup() 
{
  //currentClimate = tomatoes; // Comment to use standard settings
  pin_init();
  RTC_set(16,59,46);

  Serial.begin(9600); // For debugging
}


void loop() 
{
  //Get sensors
  get_light();
  get_soil_humidity();
  get_air_humidity();
  get_temperature();
  get_time();


  // Control
  control_light();
  control_water();
  control_air();
  control_temperature();
  RTC_count();


  // Actuators
  do_light();
  do_water();
  do_air();
  do_heat();


  // Debug
  RTC_print();
  pump_temperature_print();
  sensor_print();
  //

  delay(1000);
}
