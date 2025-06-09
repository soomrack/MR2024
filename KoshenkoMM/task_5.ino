#include <DHT11.h>

// Конфигурация пинов
#define EXTERNAL_LIGHT_PIN A0  // Фоторезистор
#define SOIL_HUMIDITY_PIN A1  // Датчик влажности почвы
#define AIR_SENSOR_PIN 12      // Датчик влажности воздуха

#define WATER_PUMP_PIN 5       // Реле водяной помпы
#define LED_GROW_LIGHT_PIN 6   // Светодиодная лампа
#define FAN_PIN 7              // Вентилятор
#define HEATER_PIN 4           // Нагревательный элемент

// Параметры климатических режимов
struct ClimateSettings {
  int min_temperature;
  int max_temperature;
  int min_soil_humidity;
  int max_soil_humidity;
  int min_air_humidity;
  int max_air_humidity;
  int min_light_level;
  int max_light_level;
  unsigned long pump_active_duration;
  unsigned long pump_inactive_duration;
  unsigned long light_duration;
  int ventilation_start_hour;
  int ventilation_end_hour;
  unsigned long ventilation_duration;
  int sleep_start_hour;
  int sleep_end_hour;
};

const ClimateSettings DEFAULT_CLIMATE = {
  240, 260, 
  550, 540, 
  20, 30, 
  600, 700,
  20000, 20000, 
  1000, 
  16, 17, 
  60000, 
  20, 8
};

const ClimateSettings TOMATO_CLIMATE = {
  320, 340, 
  450, 420, 
  15, 20, 
  650, 710,
  120000, 300000, 
  300000, 
  16, 17, 
  180000, 
  21, 7
};

const ClimateSettings CUCUMBER_CLIMATE = {
  280, 300, 
  580, 550, 
  10, 15, 
  600, 700,
  120000, 300000, 
  420000, 
  16, 17, 
  600000, 
  19, 6
};

// Системные часы
class SystemClock {
private:
  unsigned long last_update = 0;
  
public:
  unsigned long current_millis = 0;
  unsigned int hours = 0;
  unsigned int minutes = 0;
  unsigned int seconds = 0;

  void setTime(unsigned int h, unsigned int m, unsigned int s) {
    hours = h;
    minutes = m;
    seconds = s;
  }

  void update() {
    current_millis = millis();
    if (current_millis - last_update >= 1000) {
      seconds += (current_millis - last_update) / 1000;
      last_update = current_millis;
    }
    
    if (seconds >= 60) {
      minutes++;
      seconds -= 60;
    }
    if (minutes >= 60) {
      hours++;
      minutes = 0;
    }
    if (hours >= 24) {
      hours = 0;
    }
  }

  void print() const {
    Serial.println("----------------------------");
    Serial.print("Current time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
  }
};

// Управление датчиками
class SensorSystem {
private:
  DHT11 air_sensor;

public:
  int light_level = 0;
  int soil_moisture = 0;
  int air_humidity = 0;
  int air_temperature = 0;

  SensorSystem() : air_sensor(AIR_SENSOR_PIN) {}

  void readSensors() {
    light_level = analogRead(EXTERNAL_LIGHT_PIN);
    soil_moisture = analogRead(SOIL_HUMIDITY_PIN);
    air_humidity = air_sensor.readHumidity();
    air_temperature = air_sensor.readTemperature();
  }

  void printReadings() const {
    Serial.println("----------------------------");
    Serial.print("Light intensity: ");
    Serial.println(light_level);
    Serial.print("Soil moisture: ");
    Serial.println(soil_moisture);
    Serial.print("Air humidity: ");
    Serial.println(air_humidity);
    Serial.print("Temperature: ");
    Serial.println(air_temperature);
  }
};

// Управление исполнительными устройствами
class ActuatorSystem {
public:
  bool grow_light_on = false;
  bool water_pump_on = false;
  bool fan_on = false;
  bool heater_on = false;
  bool heating_allowed = true;

  void updateDevices() {
    digitalWrite(LED_GROW_LIGHT_PIN, grow_light_on ? HIGH : LOW);
    digitalWrite(WATER_PUMP_PIN, water_pump_on ? HIGH : LOW);
    digitalWrite(FAN_PIN, fan_on ? HIGH : LOW);
    digitalWrite(HEATER_PIN, heater_on ? HIGH : LOW);
  }

  void printStatus() const {
    Serial.println("----------------------------");
    Serial.print("Heater state: ");
    Serial.println(heater_on);
    Serial.print("Pump state: ");
    Serial.println(water_pump_on);
    Serial.print("Heating allowed: ");
    Serial.println(heating_allowed);
  }
};

// Контроллер микроклимата
class ClimateControlSystem {
private:
  const ClimateSettings& settings;
  SensorSystem& sensors;
  ActuatorSystem& actuators;
  SystemClock& clock;

  unsigned long light_timer = 0;
  unsigned long pump_timer = 0;
  unsigned long pump_cooldown_start = 0;
  unsigned long ventilation_timer = 0;
  unsigned long heating_timer = 0;

public:
  ClimateControlSystem(const ClimateSettings& s, SensorSystem& sen, ActuatorSystem& act, SystemClock& clk)
    : settings(s), sensors(sen), actuators(act), clock(clk) {}

  void manageClimate() {
    controlLighting();
    controlIrrigation();
    controlVentilation();
    controlTemperature();
  }

private:
  void controlLighting() {
    if (isSleepTime()) {
      actuators.grow_light_on = false;
      return;
    }

    if (needsArtificialLight() && !actuators.grow_light_on) {
      actuators.grow_light_on = true;
      light_timer = clock.current_millis;
    }

    if (shouldTurnOffLight()) {
      actuators.grow_light_on = false;
    }
  }

  bool isSleepTime() const {
    return (clock.hours < settings.sleep_end_hour) || (clock.hours >= settings.sleep_start_hour);
  }

  bool needsArtificialLight() const {
    return sensors.light_level >= settings.min_light_level;
  }

  bool shouldTurnOffLight() const {
    return (clock.current_millis - light_timer >= settings.light_duration) && 
           (sensors.light_level <= settings.max_light_level);
  }

  void controlIrrigation() {
    if (shouldStartWatering()) {
      actuators.heating_allowed = false;
      actuators.water_pump_on = true;
      pump_timer = clock.current_millis;
    }

    if (shouldStopWatering()) {
      actuators.heating_allowed = true;
      actuators.water_pump_on = false;
      pump_cooldown_start = clock.current_millis;
    }
  }

  bool shouldStartWatering() const {
    return (sensors.soil_moisture >= settings.min_soil_humidity) && 
           !actuators.water_pump_on && 
           (clock.current_millis - pump_cooldown_start > settings.pump_inactive_duration);
  }

  bool shouldStopWatering() const {
    return actuators.water_pump_on && 
           (clock.current_millis - pump_timer >= settings.pump_active_duration) && 
           (sensors.soil_moisture <= settings.max_soil_humidity);
  }

  void controlVentilation() {
    if (isScheduledVentilationTime()) {
      actuators.fan_on = true;
      return;
    }

    if (needsHumidityControl() && !actuators.fan_on) {
      actuators.fan_on = true;
      ventilation_timer = clock.current_millis;
      return;
    }

    if (shouldStopVentilation()) {
      actuators.fan_on = false;
    }
  }

  bool isScheduledVentilationTime() const {
    return (clock.hours >= settings.ventilation_start_hour) && 
           (clock.hours < settings.ventilation_end_hour);
  }

  bool needsHumidityControl() const {
    return sensors.air_humidity >= settings.max_air_humidity;
  }

  bool shouldStopVentilation() const {
    return (clock.current_millis - ventilation_timer >= settings.ventilation_duration) && 
           (sensors.air_humidity <= settings.min_air_humidity);
  }

  void controlTemperature() {
    if (!actuators.heating_allowed) {
      actuators.fan_on = false;
      actuators.heater_on = false;
      return;
    }

    if (needsHeating() && !actuators.fan_on) {
      actuators.fan_on = true;
      actuators.heater_on = true;
      heating_timer = clock.current_millis;
    }

    if (shouldStopHeating()) {
      actuators.fan_on = false;
      actuators.heater_on = false;
    }
  }

  bool needsHeating() const {
    return sensors.air_temperature <= settings.min_temperature;
  }

  bool shouldStopHeating() const {
    return (clock.current_millis - heating_timer >= settings.ventilation_duration) && 
           (sensors.air_temperature >= settings.max_temperature);
  }
};

// Инициализация оборудования
void initializeHardware() {
  pinMode(EXTERNAL_LIGHT_PIN, INPUT);
  pinMode(SOIL_HUMIDITY_PIN, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(LED_GROW_LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
}

// Глобальные системы
SystemClock realtime_clock;
SensorSystem sensor_system;
ActuatorSystem actuator_system;
ClimateControlSystem climate_control(DEFAULT_CLIMATE, sensor_system, actuator_system, realtime_clock);

void setup() {
  initializeHardware();
  realtime_clock.setTime(16, 59, 46);
  Serial.begin(9600);
  // Для других культур:
  // climate_control = ClimateControlSystem(TOMATO_CLIMATE, sensor_system, actuator_system, realtime_clock);
}

void loop() {
  sensor_system.readSensors();
  realtime_clock.update();
  climate_control.manageClimate();
  actuator_system.updateDevices();

  // Отладочная информация
  realtime_clock.print();
  actuator_system.printStatus();
  sensor_system.printReadings();

  delay(1000);
}