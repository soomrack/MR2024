#include <DHT11.h>

// Sensors:
#define EXTERNAL_LIGHT_PIN A3
#define SOIL_HUMI_PIN A1
#define AIR_PIN 12

DHT11 dht11(AIR_PIN);

// Actuators:
#define WATER_PUMP_PIN 5
#define LED_LIGHT_PIN 6
#define FAN_PIN 7
#define HEAT_FAN_PIN 4

typedef unsigned long int time;

unsigned int cur_hour, cur_min, cur_sec;
unsigned int ext_light, soil_humi, air_humi, air_temp;
bool is_heating, is_airing;
bool allow_heating = 1;




void on(int PIN) 
{
  digitalWrite(PIN, 1);
}


void off(int PIN) 
{
  digitalWrite(PIN, 0);
}




class Climate 
{
  int desired_temp;
  int desired_soil_humi;
  int desired_air_humi;
  int desired_light;
  int pump_work_time;
  int light_duration;
  int ventilation_duration;
  int sleep_start;
  int sleep_end;
  
  void lighting_check()
  {
    static time t_light;

    if ((cur_hour < sleep_end) || (cur_hour >= sleep_start))
    {
      off(LED_LIGHT_PIN);
      Serial.println("NIGHT");
      return;
    }

    if ((ext_light >= desired_light) && (digitalRead(LED_LIGHT_PIN) == 0)) {
      on(LED_LIGHT_PIN);
      t_light = millis();
    }

    if ((millis() - t_light >= light_duration) && (ext_light < desired_light)) {
      off(LED_LIGHT_PIN);
    }
  }

  void watering_check() 
  {
    static time p_time;

    if ((soil_humi >= desired_soil_humi) && (digitalRead(WATER_PUMP_PIN) == 0)) {
      allow_heating = 0;
      on(WATER_PUMP_PIN);
      p_time = millis();
    }

    if ((millis() - p_time >= pump_work_time) && (soil_humi < desired_soil_humi)) {
      off(WATER_PUMP_PIN);
      allow_heating = 1;
    }
  }

  void airing_check() 
  {
    if (air_humi >= desired_air_humi) {
      is_airing = 1;
    } else {
      is_airing = 0;
    }
  }


  void heating_check() 
  { 
    if (air_temp <= desired_temp) 
    {
      is_heating = 1;
    } 
    else {
      is_heating = 0;
    }
  }


  void fan_check() 
  {
    static time t_fan;

    airing_check();
    heating_check();

    if (is_heating || is_airing) {
      if (is_heating && allow_heating) {
        on(HEAT_FAN_PIN);
        Serial.println("Heat on");
      } else {
        off(HEAT_FAN_PIN);
        Serial.println("HEAT OFF");
      }

      if (digitalRead(FAN_PIN) == 0) {
        on(FAN_PIN);
        t_fan = millis();
      }
    }

    if (millis() - t_fan >= ventilation_duration) {
      off(FAN_PIN);
      off(HEAT_FAN_PIN);
    }
  }

  void pump_temp_check()
  {
    Serial.print("------------------------------\n");
    Serial.println("Status temp: " + String(digitalRead(HEAT_FAN_PIN)));
    Serial.println("Status pump: " + String(digitalRead(WATER_PUMP_PIN)));

    Serial.println("Allow heating: " + String(allow_heating));
    Serial.println("Is airing: " + String(is_airing));
    Serial.println("IS heating: " + String(is_heating));

  }
};

void alice_init()
{
    alice = (Person){
        .bank = 1000 * 1000 * 100,
        .salary = 250 * 1000 * 100,
        .home = 13 * 1000 * 1000 * 100,
        .food_expenses = 15 * 1000 * 100,
        .utility_expenses = 8000 * 100,
        .other_expenses = 17000 * 100,
        .deposit_rate = 0.2,
        .rent = 0
    };
    strcpy(alice.name, "Alice");
}

Climate currentClimate = { // Default climate
  250,   // TEMP
  550,   // SOIL_HUMI
  30,    // AIR_HUMI
  700,   // LIGHT
  1/20 * 60 * 1000, // Pump 1 min
  1/60 * 60 * 1000, // Light 1 min
  1 * 60 * 1000,  // Vent 1 min
  20, // Sleep in
  8 // Wake up in
};


Climate tomatoes = 
{
  360,   // TEMP
  550,   // SOIL_HUMI
  10,    // AIR_HUMI
  700,   // LIGHT
  2 * 60 * 1000, // Pump 2 min
  5 * 60 * 1000, // Light 5 min
  3 * 60 * 1000,  // Vent 3 min
  21, // Sleep in
  7 // Wake up in
};


Climate cucumbers = 
{
  340,   // TEMP
  500,   // SOIL_HUMI
  15,    // AIR_HUMI
  650,   // LIGHT
  2 * 60 * 1000,  // Punp 2 min
  7 * 60 * 1000,  // Light 7 min
  10 * 60 * 1000,   // Vent 10 
  19, // Sleep in
  6 // Wake up in
};

void pin_init() 
{
  pinMode(EXTERNAL_LIGHT_PIN, INPUT);
  pinMode(SOIL_HUMI_PIN, INPUT);

  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_FAN_PIN, OUTPUT);
}


void RTC_set(unsigned int hour_to_set, unsigned int min_to_set, unsigned int sec_to_set)
{
  cur_hour = hour_to_set;
  cur_min= min_to_set;
  cur_sec = sec_to_set;
}


void RTC_count()
{
  static time last_millis = millis();

  if ((millis() - last_millis) >= 1000)
  {
    cur_sec = cur_sec + (millis() - last_millis) / 1000;
    Serial.println("tek" + String(millis()));
    Serial.println("Last"+String(last_millis));
    last_millis = millis();
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
  Serial.print("------------------------------\n");
  Serial.println("Time: " + String(cur_hour) + ":" + String(cur_min) + ":" + String(cur_sec));
}




void get_info() 
{
  ext_light = analogRead(EXTERNAL_LIGHT_PIN);
  soil_humi = analogRead(SOIL_HUMI_PIN);
  air_humi = dht11.readHumidity();
  air_temp = dht11.readTemperature();
}



void sensor_print() 
{
  Serial.print("------------------------------\n");
  Serial.print("Light: "); Serial.println(ext_light);
  Serial.print("Soil humidity: "); Serial.println(soil_humi);
  Serial.print("Air humidity: "); Serial.println(air_humi);
  Serial.print("Air temperature: "); Serial.println(air_temp);
}


void setup() 
{
  //currentClimate = tomatoes;
  pin_init();
  RTC_set(19,59,16);

  Serial.begin(9600); // For debugging
}


void loop() 
{
  get_info();
  RTC_count();

  // For debugging 
  RTC_print();
  sensor_print();
  pump_temp_check();
  //

  lighting_check();
  watering_check();

  fan_check();

  delay(1000);
}
