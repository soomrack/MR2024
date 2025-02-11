#include <DFRobot_DHT11.h>

#define FAN_PIN 7
#define LIGHT_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define LIGHT_SENSOR_PIN A0
#define SOIL_SENSOR_PIN A1
#define DHT11_PIN 13

DFRobot_DHT11 DHT;

struct Climate
{
  int LIGHT_THRESHOLD;
  int SOIL_HUMIDITY_THRESHOLD;
  int AIR_HUMIDITY_THRESHOLD;
  int TEMP_HIGH_THRESHOLD;
  int TEMP_LOW_THRESHOLD;
  int TIME_SUNRISE;
  int TIME_SUNSET;
  int VENTILATION_TIMER;
  int PUMP_ON_SECONDS;
  int PUMP_OFF_SECONDS;
};
typedef struct Climate Climate;

Climate Greenhouse;

struct Fan_states
{
  bool fan_humidity_state = 0;
  bool fan_temperature_state = 0;
  bool fan_timer_state = 0;
};
typedef struct Fan_states Fan_states;

Fan_states Greenhouse_fan_states;


struct Light_states
{
  bool light_sensor_state = 0;
  bool light_timer_state = 0;
};
typedef struct Light_states Light_states;

Light_states Greenhouse_light_states;

struct Pump_states
{
  bool pump_humidity_state = 0;
  bool pump_timer_state = 0;
};
typedef struct Pump_states Pump_states;

Pump_states Greenhouse_pump_states;

struct Heater_states
{
  bool heater_state = 0;
};
typedef struct Heater_states Heater_states;

Heater_states Greenhouse_heater_states;

struct Data
{
  int air_humidity;
  int air_temperature;
  int light_level;
  int soil_humidity;
  int current_time;
};
typedef struct Data Data;

Data Greenhouse_data;

long milliseconds = 0;

void initialize_climate_tomatoes()
{
  Greenhouse.LIGHT_THRESHOLD = 70;
  Greenhouse.SOIL_HUMIDITY_THRESHOLD = 40;
  Greenhouse.AIR_HUMIDITY_THRESHOLD = 70;
  Greenhouse.TEMP_HIGH_THRESHOLD = 25;
  Greenhouse.TEMP_LOW_THRESHOLD = 20;
  Greenhouse.TIME_SUNRISE = 7;
  Greenhouse.TIME_SUNSET = 20;
  Greenhouse.VENTILATION_TIMER = 3;
  Greenhouse.PUMP_ON_SECONDS = 2;
  Greenhouse.PUMP_OFF_SECONDS = 3;
}

void data_print()
{
  static long old_time = 0;
  if (milliseconds - old_time > 2000)  {
    Serial.print("Light level: "); Serial.println(Greenhouse_data.light_level);
    Serial.print("Soil Humidity: "); Serial.println(Greenhouse_data.soil_humidity);
    Serial.print("Air Humidity: "); Serial.println(Greenhouse_data.air_humidity);
    Serial.print("Temperature: "); Serial.println(Greenhouse_data.air_temperature);
    Serial.print("Current time: "); Serial.println(Greenhouse_data.current_time);
    Serial.println();
    
    old_time = milliseconds;
  }
}

void dht11_read(int *air_humidity, int *air_temperature)
{
  static long old_time = 0;
  if (milliseconds - old_time > 500) {
    DHT.read(DHT11_PIN);
    *air_humidity = DHT.humidity;
    *air_temperature = DHT.temperature;

    old_time = milliseconds;
  }
}

void light_read(int *light_level)
{
  static long old_time = 0;
  if (milliseconds - old_time > 500) {
    *light_level = map(analogRead(LIGHT_SENSOR_PIN), 980, 38, 0, 100);

    old_time = milliseconds;
  }
}

void soil_humidity_read(int *soil_humidity)
{
  static long old_time = 0;
  if (milliseconds - old_time > 500) {
    *soil_humidity = map(analogRead(SOIL_SENSOR_PIN), 1023, 0, 0, 100);

    old_time = milliseconds;
  }

}

void time_read(int *current_time)
{
  *current_time = milliseconds / 2000 % 24;
}

void control_light()
{
  if (Greenhouse.TIME_SUNRISE < Greenhouse_data.current_time && Greenhouse_data.current_time < Greenhouse.TIME_SUNSET) {
    Greenhouse_light_states.light_timer_state = 1;
  }
  else Greenhouse_light_states.light_timer_state = 0;

  if (Greenhouse_data.light_level < Greenhouse.LIGHT_THRESHOLD) Greenhouse_light_states.light_sensor_state = 1;
  else Greenhouse_light_states.light_sensor_state = 0;
}

void control_temperature()
{
  if (Greenhouse_data.air_temperature < Greenhouse.TEMP_LOW_THRESHOLD)
  {
    Greenhouse_heater_states.heater_state = 1;
    Greenhouse_fan_states.fan_temperature_state = 1;
  }
  else if (Greenhouse_data.air_temperature > Greenhouse.TEMP_HIGH_THRESHOLD)
  {
    Greenhouse_heater_states.heater_state = 0;
    Greenhouse_fan_states.fan_temperature_state = 1;
  }
  else
  {
    Greenhouse_heater_states.heater_state = 0;
    Greenhouse_fan_states.fan_temperature_state = 0;
  }
}

void control_ventilation()
{
  if (Greenhouse_data.current_time % Greenhouse.VENTILATION_TIMER == 0) Greenhouse_fan_states.fan_timer_state = 1;
  else Greenhouse_fan_states.fan_timer_state = 0;

  if (Greenhouse_data.air_humidity > Greenhouse.AIR_HUMIDITY_THRESHOLD) Greenhouse_fan_states.fan_humidity_state = 1;
  else Greenhouse_fan_states.fan_humidity_state = 0;
}

void control_watering()
{
  if (Greenhouse_data.soil_humidity < Greenhouse.SOIL_HUMIDITY_THRESHOLD) Greenhouse_pump_states.pump_humidity_state = 1;
  else Greenhouse_pump_states.pump_humidity_state = 0;

  static long old_time;
  if (milliseconds - old_time >= 0 && milliseconds - old_time < Greenhouse.PUMP_ON_SECONDS) {
    Greenhouse_pump_states.pump_timer_state = 1;
  }
  else if (milliseconds - old_time >= Greenhouse.PUMP_ON_SECONDS && milliseconds - old_time < Greenhouse.PUMP_ON_SECONDS + Greenhouse.PUMP_OFF_SECONDS) {
    Greenhouse_pump_states.pump_timer_state = 0;
  }
  else old_time = milliseconds;
}

void actuate()
{
  if (Greenhouse_light_states.light_sensor_state && Greenhouse_light_states.light_timer_state) digitalWrite(LIGHT_PIN, HIGH);
  else digitalWrite(LIGHT_PIN, LOW);

  if (Greenhouse_heater_states.heater_state) digitalWrite(HEATER_PIN, HIGH);
  else digitalWrite(HEATER_PIN, LOW);

  if (Greenhouse_fan_states.fan_humidity_state || Greenhouse_fan_states.fan_temperature_state || Greenhouse_fan_states.fan_timer_state){
    digitalWrite(FAN_PIN, HIGH);
  }
  else digitalWrite(FAN_PIN, LOW);

  if (Greenhouse_pump_states.pump_timer_state && Greenhouse_pump_states.pump_humidity_state) digitalWrite(PUMP_PIN, HIGH);
  else digitalWrite(PUMP_PIN, LOW);
}

void setup()
{
  initialize_climate_tomatoes();

  Serial.begin(9600);

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);

  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(DHT11_PIN, INPUT);
  
  soil_humidity_read(&Greenhouse_data.soil_humidity);
  light_read(&Greenhouse_data.light_level);
  time_read(&Greenhouse_data.current_time);
  dht11_read(&Greenhouse_data.air_humidity, &Greenhouse_data.air_temperature);

  data_print();

  delay(500);
}

void loop()
{
  milliseconds = millis();
  
  soil_humidity_read(&Greenhouse_data.soil_humidity);
  light_read(&Greenhouse_data.light_level);
  time_read(&Greenhouse_data.current_time);
  dht11_read(&Greenhouse_data.air_humidity, &Greenhouse_data.air_temperature);

  data_print();

  control_light();
  control_temperature();
  control_ventilation();
  control_watering();

  actuate();

  delay(1);
}
