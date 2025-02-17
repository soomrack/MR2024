//SOUTHGARDEN

#include <Arduino.h>
#include "config.h"
#include <DFRobot_DHT11.h>

DFRobot_DHT11 DHT_TEMP_WET_SENSOR;

struct Climate
{
  int LIGHT_THRESHOLD;
  int EARTH_HUMIDITY_THRESHOLD;
  int AIR_HUMIDITY_THRESHOLD;
  int TEMP_HIGH_THRESHOLD;
  int TEMP_LOW_THRESHOLD;
  int TIME_SUNRISE;
  int TIME_SUNSET;
  int VENTILATION_TIMER;
  int PUMP_ON_SECONDS;
  int PUMP_OFF_SECONDS;
};


struct Fan_states
{
  bool fan_humidity_state = 0;
  bool fan_temperature_state = 0;
  bool fan_timer_state = 0;
};


struct Light_states
{
  bool light_sensor_state = 0;
  bool light_timer_state = 0;
};


struct Pump_states
{
  bool pump_humidity_state = 0;
  bool pump_timer_state = 0;
};


struct Heater_states
{
  bool heater_state = 0;
};


struct Data
{
  int air_humidity;
  int air_temperature;
  int light_level;
  int earth_humidity;
  int current_time;
};


Climate Garden;
Fan_states Garden_fan_states;
Light_states Garden_light_states;
Pump_states Garden_pump_states;
Heater_states Garden_heater_states;
Data Garden_data;

unsigned long int milliseconds = 0;
bool flag_heat_light_work = 0;

void initialize_climate()
{
  Garden.LIGHT_THRESHOLD = 70;
  Garden.EARTH_HUMIDITY_THRESHOLD = 20;
  Garden.AIR_HUMIDITY_THRESHOLD = 70;
  Garden.TEMP_HIGH_THRESHOLD = 30;
  Garden.TEMP_LOW_THRESHOLD = 17;
  Garden.TIME_SUNRISE = 8;
  Garden.TIME_SUNSET = 18;
  Garden.VENTILATION_TIMER = 3;
  Garden.PUMP_ON_SECONDS = 2;
  Garden.PUMP_OFF_SECONDS = 3;
}

void data_print()
{
  static long old_time = 0;

  if (milliseconds - old_time > TIME_CHECK_SENSORS)  {
    Serial.print("Light level: "); Serial.print(Garden_data.light_level,'\n');
    Serial.println();
    Serial.print("Soil Humidity: "); Serial.print(Garden_data.earth_humidity,'\n');
    Serial.println();
    Serial.print("Air Humidity: "); Serial.print(Garden_data.air_humidity,'\n');
    Serial.println();
    Serial.print("Temperature: "); Serial.print(Garden_data.air_temperature,'\n');
    Serial.println();
    Serial.print("Current time: "); Serial.print(Garden_data.current_time,'\n');
    Serial.println();
    
    old_time = milliseconds;
  }
}

void read_dht11(int *air_humidity, int *air_temperature)
{
  static long old_time = 0;
  if (milliseconds - old_time > TIME_CHECK_SENSORS) {
    DHT_TEMP_WET_SENSOR.read(PIN_TEMP_WET_SENSOR);
    *air_humidity = DHT_TEMP_WET_SENSOR.humidity;
    *air_temperature = DHT_TEMP_WET_SENSOR.temperature;

    old_time = milliseconds;
  }
}

void read_light(int *light_level) {
    static long old_time = 0;

    if (millis() - old_time > TIME_CHECK_SENSORS) {
        old_time = millis();

        int light_sensors[3] = {
            map(analogRead(PIN_LIGHT_SENSOR), 1023, 0, 0, 100),
            map(analogRead(PIN_LIGHT_SENSOR), 1023, 0, 0, 100),
            map(analogRead(PIN_LIGHT_SENSOR), 1023, 0, 0, 100)
        };
        
        int sum = light_sensors[0] + light_sensors[1] + light_sensors[2];
        int average = sum / 3;

        int deviations[3] = { //отклонение
            abs(light_sensors[0] - average),
            abs(light_sensors[1] - average),
            abs(light_sensors[2] - average)
        };
        
        short max_dev_index = 0;
        for (int i = 1; i < 3; i++) {
            if (deviations[i] > deviations[max_dev_index]) {
                max_dev_index = i;
            }
        }
        
        if (deviations[max_dev_index] > 20) {
            sum = sum-light_sensors[max_dev_index];
            *light_level = sum / 2; 
        } else {
            *light_level = average;
        }
    }
}

void read_earth_humidity(int *earth_humidity)
{
  static long old_time = 0;
  if (milliseconds - old_time > TIME_CHECK_SENSORS) {
    *earth_humidity = map(analogRead(PIN_EARTH_WET_SENSOR), 1023, 0, 0, 100);

    old_time = milliseconds;
  }

}

void read_time(int *current_time)
{
  *current_time = milliseconds / 2000 % 24;
}

void control_light()
{
  if (CFG_FLOWER_SLEEP_MODE) {
    if (Garden.TIME_SUNRISE < Garden_data.current_time && Garden_data.current_time < Garden.TIME_SUNSET && !flag_heat_light_work) {
      Garden_light_states.light_timer_state = 1;
    } 
    else Garden_light_states.light_timer_state = 0;
  }
  if (!CFG_FLOWER_SLEEP_MODE) {
    Garden_light_states.light_timer_state = 1;
  }

  if (Garden_data.light_level < Garden.LIGHT_THRESHOLD) Garden_light_states.light_sensor_state = 1;
  else Garden_light_states.light_sensor_state = 0;
}

void control_temperature()
{
  if (Garden_data.air_temperature < Garden.TEMP_LOW_THRESHOLD && !flag_heat_light_work)
  {
    flag_heat_light_work =1;
    Garden_heater_states.heater_state = 1;
    Garden_fan_states.fan_temperature_state = 1;
  }
  else if (Garden_data.air_temperature > Garden.TEMP_HIGH_THRESHOLD)
  {
    Garden_heater_states.heater_state = 0;
    Garden_fan_states.fan_temperature_state = 1;
  }
  else
  {
    Garden_heater_states.heater_state = 0;
    Garden_fan_states.fan_temperature_state = 0;
    flag_heat_light_work =0;
  }
}

void control_regular_ventilation()
{
  if (Garden_data.current_time % Garden.VENTILATION_TIMER == 0) Garden_fan_states.fan_timer_state = 1;
  else Garden_fan_states.fan_timer_state = 0;
}

void control_air_humidity()
{
  if (Garden_data.air_humidity > Garden.AIR_HUMIDITY_THRESHOLD) Garden_fan_states.fan_humidity_state = 1;
  else Garden_fan_states.fan_humidity_state = 0;
}

void control_earth_watering()
{
  if (Garden_data.earth_humidity < Garden.EARTH_HUMIDITY_THRESHOLD) Garden_pump_states.pump_humidity_state = 1;
  else Garden_pump_states.pump_humidity_state = 0;

  static long old_time;
  if (milliseconds - old_time >= TIME_CHECK_SENSORS && milliseconds - old_time < Garden.PUMP_ON_SECONDS) {
    Garden_pump_states.pump_timer_state = 1;
  }
  else if (milliseconds - old_time >= Garden.PUMP_ON_SECONDS && milliseconds - old_time < Garden.PUMP_ON_SECONDS + Garden.PUMP_OFF_SECONDS) {
    Garden_pump_states.pump_timer_state = 0;
  }
  else old_time = milliseconds;
}

void actuate_light()
{
  (Garden_light_states.light_sensor_state && Garden_light_states.light_timer_state) ? 
  digitalWrite(PIN_UV_LED_LIGHT, HIGH) : digitalWrite(PIN_UV_LED_LIGHT, LOW);
}

void actuate_heat()
{
  Garden_heater_states.heater_state? 
  digitalWrite(PIN_HEAT, HIGH) : digitalWrite(PIN_HEAT, LOW);
}

void actuate_fan()
{
  (Garden_fan_states.fan_humidity_state || Garden_fan_states.fan_temperature_state || Garden_fan_states.fan_timer_state) ? 
  digitalWrite(PIN_COOLER, HIGH) : digitalWrite(PIN_COOLER, LOW);
}

void actuate_pump()
{
  Garden_pump_states.pump_timer_state && Garden_pump_states.pump_humidity_state ? 
  digitalWrite(PIN_WATER_PUMP, HIGH) : digitalWrite(PIN_WATER_PUMP, LOW);
}


void pin_initialization() {
  pinMode(PIN_LIGHT_SENSOR, INPUT);
  pinMode(PIN_EARTH_WET_SENSOR, INPUT);
  pinMode(PIN_HEAT, OUTPUT);
  pinMode(PIN_WATER_PUMP, OUTPUT);
  pinMode(PIN_UV_LED_LIGHT, OUTPUT);
  pinMode(PIN_COOLER, OUTPUT);
  pinMode(PIN_TEMP_WET_SENSOR, INPUT);
}

void setup() {
  pin_initialization();
  initialize_climate();

  Serial.begin(9600);
  milliseconds = millis();

  read_earth_humidity(&Garden_data.earth_humidity);
  read_light(&Garden_data.light_level);
  read_time(&Garden_data.current_time);
  read_dht11(&Garden_data.air_humidity, &Garden_data.air_temperature);

  data_print();

  delay(500);
}

void loop() {
    milliseconds = millis();

    read_earth_humidity(&Garden_data.earth_humidity);
    read_light(&Garden_data.light_level);
    read_time(&Garden_data.current_time);
    read_dht11(&Garden_data.air_humidity, &Garden_data.air_temperature);

    data_print();

    control_light();
    control_temperature();
    control_regular_ventilation();
    control_air_humidity();
    control_earth_watering();

    actuate_light();
    //actuate_fan();
    actuate_heat();
    actuate_pump();

    
    delay(1);


  //возможно сделать delay; //
  // разести actuate в отдельные функции //
  // read_time с т.з. названия //
  // regular_ventilation //
  // watering -> earth watering || dirt_watering //
  // + control_air_humidity если высокая влажность // 
  // добавить пару датчиков освещенности чтобы они работали в каскаде

}
