#include <DHT11.h>

// --- Пины ---
#define EXTERNAL_LIGHT_PIN A0 //фоторезистор
#define SOIL_HUMI_PIN      A1 //влажность почвы
#define AIR1_PIN           12 //влажность воздуха

#define WATER_PUMP_PIN     5 //помпа
#define LED_LIGHT_PIN      6 //светодиод
#define FAN_PIN            7 //вентилятор
#define HEAT_PIN           4 //нагреватель

// --- Класс климата ---
struct Climate {
  int temp_min, temp_max;
  int soil_humidity_min, soil_humidity_max;
  int air_humidity_min, air_humidity_max;
  int light_min, light_max;
  unsigned long pump_work_time, pump_dead_time;
  unsigned long light_duration;
  int start_ventilation, end_ventilation;
  unsigned long ventilation_duration;
  int sleep_start, sleep_end;
};

// --- Настройки климата ---
const Climate CLIMATE_DEFAULT = {
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
const Climate CLIMATE_TOMATOES = {
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
const Climate CLIMATE_CUCUMBERS = {
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

// --- RTC (часы реального времени) ---
struct RTC {
  unsigned long cur_millis;
  unsigned int hour, min, sec;

  void set(unsigned int h, unsigned int m, unsigned int s) {
    hour = h; min = m; sec = s;
  }
  void update() {
    static unsigned long last_millis = 0;
    cur_millis = millis();
    if ((cur_millis - last_millis) >= 1000) {
      sec += (cur_millis - last_millis) / 1000;
      last_millis = cur_millis;
    }
    if (sec >= 60) { min++; sec -= 60; }
    if (min == 60) { hour++; min = 0; }
    if (hour == 24) hour = 0;
  }
  void print() {
    Serial.println("------------------------------");
    Serial.print("Time: ");
    Serial.print(hour); Serial.print(":");
    Serial.print(min); Serial.print(":");
    Serial.println(sec);
  }
};

// --- Сенсоры ---
class SensorManager {
  DHT11 dht1;
public:
  unsigned int ext_light = 0, soil_humidity = 0, air_humidity = 0;
  unsigned int air_temp = 0;
  SensorManager() : dht1(AIR1_PIN) {}

  void readAll() {
    ext_light = analogRead(EXTERNAL_LIGHT_PIN);
    soil_humidity = analogRead(SOIL_HUMI_PIN);
    air_humidity = dht1.readHumidity();
    air_temp = dht1.readTemperature();
  }
  void print() {
    Serial.println("------------------------------");
    Serial.print("Light: "); Serial.println(ext_light);
    Serial.print("Soil humidity: "); Serial.println(soil_humidity);
    Serial.print("Air humidity: "); Serial.println(air_humidity);
    Serial.print("Air temperature: "); Serial.println(air_temp);
  }
};

// --- Актуаторы ---
class ActuatorManager {
public:
  bool light = false, watering = false, airing = false, heating = false;
  bool allow_heating = true;

  void apply() {
    digitalWrite(LED_LIGHT_PIN, light ? HIGH : LOW);
    digitalWrite(WATER_PUMP_PIN, watering ? HIGH : LOW);
    digitalWrite(FAN_PIN, airing ? HIGH : LOW);
    digitalWrite(HEAT_PIN, heating ? HIGH : LOW);
  }
  void printStatus() {
    Serial.println("------------------------------");
    Serial.print("Status temp: "); Serial.println(digitalRead(HEAT_PIN));
    Serial.print("Status pump: "); Serial.println(digitalRead(WATER_PUMP_PIN));
    Serial.print("Allow heating: "); Serial.println(allow_heating);
  }
};

// --- Контроллер климата ---
class ClimateController {
  const Climate& climate;
  SensorManager& sensors;
  ActuatorManager& actuators;
  RTC& rtc;

  unsigned long t_light = 0, p_time = 0, dead_time_start = 0, t_air = 0, t_heat = 0;
public:
  ClimateController(const Climate& c, SensorManager& s, ActuatorManager& a, RTC& r)
    : climate(c), sensors(s), actuators(a), rtc(r) {}

  void control() {
    controlLight();
    controlWater();
    controlAir();
    controlTemp();
  }

  void controlLight() {
    if ((rtc.hour < climate.sleep_end) || (rtc.hour >= climate.sleep_start)) {
      actuators.light = false;
      return;
    }
    if ((sensors.ext_light >= climate.light_min) && !digitalRead(LED_LIGHT_PIN)) {
      actuators.light = true;
      t_light = rtc.cur_millis;
    }
    if ((rtc.cur_millis - t_light >= climate.light_duration) && (sensors.ext_light <= climate.light_max)) {
      actuators.light = false;
    }
  }

  void controlWater() {
    if ((sensors.soil_humidity >= climate.soil_humidity_min) && !digitalRead(WATER_PUMP_PIN)
        && (rtc.cur_millis - dead_time_start > climate.pump_dead_time)) {
      actuators.allow_heating = false;
      actuators.watering = true;
      p_time = rtc.cur_millis;
    }
    if (digitalRead(WATER_PUMP_PIN) && (rtc.cur_millis - p_time >= climate.pump_work_time)
        && (sensors.soil_humidity <= climate.soil_humidity_max)) {
      actuators.allow_heating = true;
      actuators.watering = false;
      dead_time_start = rtc.cur_millis;
    }
  }

  void controlAir() {
    if ((rtc.hour >= climate.start_ventilation) && (rtc.hour < climate.end_ventilation)) {
      actuators.airing = true;
      return;
    }
    if ((sensors.air_humidity >= climate.air_humidity_max) && !digitalRead(FAN_PIN)) {
      actuators.airing = true;
      t_air = rtc.cur_millis;
      return;
    }
    if ((rtc.cur_millis - t_air >= climate.ventilation_duration) && (sensors.air_humidity <= climate.air_humidity_min)) {
      actuators.airing = false;
      return;
    }
  }

  void controlTemp() {
    if (!actuators.allow_heating) {
      actuators.airing = false;
      actuators.heating = false;
      return;
    }
    if ((sensors.air_temp <= climate.temp_min) && !digitalRead(FAN_PIN)) {
      actuators.airing = true;
      actuators.heating = true;
      t_heat = rtc.cur_millis;
    }
    if ((rtc.cur_millis - t_heat >= climate.ventilation_duration) && (sensors.air_temp >= climate.temp_max)) {
      actuators.airing = false;
      actuators.heating = false;
    }
  }
};

// --- Инициализация пинов ---
void pin_init() {
  pinMode(EXTERNAL_LIGHT_PIN, INPUT);
  pinMode(SOIL_HUMI_PIN, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
}

// --- Глобальные объекты ---
RTC rtc;
SensorManager sensors;
ActuatorManager actuators;
ClimateController controller(CLIMATE_CUCUMBERS, sensors, actuators, rtc);

void setup() {
  pin_init();
  rtc.set(16, 59, 46);
  Serial.begin(9600);
  // controller = ClimateController(CLIMATE_TOMATOES, sensors, actuators, rtc); // Для других культур
}

void loop() {
  sensors.readAll();
  rtc.update();
  controller.control();
  actuators.apply();

  // Debug
  rtc.print();
  actuators.printStatus();
  sensors.print();

  delay(1000);
}