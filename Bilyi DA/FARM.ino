#include <DHT11.h>


#include <DHT11.h> // Подключаем библиотеку для работы с датчиком температуры и влажности DHT11

// Макросы для назначения пинов. Использование макросов делает код читаемее и легче для изменения.
#define PIN_LIGHT_SENS A0   // Аналоговый пин для датчика освещенности
#define PIN_SOIL_SENS A1    // Аналоговый пин для датчика влажности почвы
#define PIN_HEAT_CTRL 4     // Цифровой пин для управления нагревателем
#define PIN_PUMP_CTRL 5     // Цифровой пин для управления помпой
#define PIN_LIGHT_CTRL 6    // Цифровой пин для управления освещением
#define PIN_FAN_CTRL 7      // Цифровой пин для управления вентилятором
#define PIN_DHT11_SENS 12   // Цифровой пин для датчика DHT11

DHT11 dht(PIN_DHT11_SENS); // Создание объекта датчика DHT11 на указанном пине

struct Farm_stat {
  int temp;               // Текущая температура (°C)
  int air_hum;            // Текущая влажность воздуха (%)
  int soil_hum;           // Текущая влажность почвы (%)
  bool light_sens;        // Текущий статус освещенности (1 - темно, 0 - светло)
  long last_pump_enable;  // Время (в секундах) последнего включения помпы (для защиты от частых запусков)
};


class Climate {
  public:
    int min_temp;         // Минимально допустимая температура
    int max_temp;         // Максимально допустимая температура
    int min_air_hum;      // Минимально допустимая влажность воздуха
    int max_air_hum;      // Максимально допустимая влажность воздуха
    int min_soil_hum;     // Минимально допустимая влажность почвы (ниже - нужно поливать)
    int max_soil_hum;     // Максимально допустимая влажность почвы (выше - перелив)
    int fan_enable_time;  // Час начала проветривания (включения вентилятора по расписанию)
    int fan_disable_time; // Час окончания проветривания
};

class Termometer {
  private:
    int temperature;
    int humidity;
    const DHT11 &t_sensor;
  public:
    Termometer(DHT11 &sensor);
    void update_param();
    int get_temp();
    int get_hum();
};


Termometer::Termometer(DHT11 &sensor) : t_sensor(sensor)
{
}


void Termometer::update_param()
{
  temperature = t_sensor.readTemperature();
  humidity = t_sensor.readHumidity();
}


int Termometer::get_temp()
{
  update_param();
  return temperature;
}


int Termometer::get_hum()
{
  update_param();
  return humidity;
}


class Soil_hygrometer {
  private:
    int soil_humidity;
    unsigned int h_pin;
  public:
    Soil_hygrometer(unsigned int pin);
    void update_param();
    int get_hum();
};


Soil_hygrometer::Soil_hygrometer(unsigned int pin) : h_pin(pin)
{
}


void Soil_hygrometer::update_param()
{
  soil_humidity = map(analogRead(h_pin), 0, 1023, 100, 0);
}


int Soil_hygrometer::get_hum()
{
  update_param();
  return soil_humidity;
}


class Light_sensor {
  private:
    bool light;
    unsigned int l_pin;
  public:
    Light_sensor(unsigned int pin);
    void update_param();
    int get_light();
};


Light_sensor::Light_sensor(unsigned int pin) : l_pin(pin)
{
}


void Light_sensor::update_param()
{
  int l = map(analogRead(l_pin), 0, 1023, 100, 0);
  if (l > 50) light = 0;
  else light = 1;
}


int Light_sensor::get_light()
{
  update_param();
  return light;
}


class Heater {
  private:
    unsigned int pin;
  public:
    bool on_temperature;

  public:
    Heater(unsigned int pin);
    void change(bool stat);
};


Heater::Heater(unsigned int pin) : pin(pin)
{
  on_temperature = false;
}

void Heater::change(bool stat)
{
  digitalWrite(pin, stat);
  on_temperature = stat;
}


class Fan {
  private:
    unsigned int f_pin;
  public:
    bool on_blowing;
  public:
    Fan(unsigned int pin);
    void change(bool stat);
};


Fan::Fan(unsigned int pin) : f_pin(pin)
{
  on_blowing = false;
}

void Fan::change(bool stat)
{
  digitalWrite(f_pin, stat);
  on_blowing = stat;
}


class Pump {
  private:
    unsigned int pin;
  public:
    bool on_floating;
  public:
    Pump(unsigned int pin);
    void change(bool stat);
};


Pump::Pump(unsigned int pin) : pin(pin)
{
  on_floating = false;
}

void Pump::change(bool stat)
{
  digitalWrite(pin, stat);
  on_floating = stat;
}


class Light {
  private:
    unsigned int l_pin;
  public:
    bool on_ligt;
  public:
    Light(unsigned int pin);
    void change(bool stat);
};


Light::Light(unsigned int pin) : l_pin(pin)
{
  on_ligt = false;
}

void Light::change(bool stat)
{
  digitalWrite(l_pin, stat);
  on_ligt = stat;
}


class Datatime {
  private:
    int minute;
    unsigned long seconds;
  public:
    unsigned long get_seconds();
    int get_minute();
};


unsigned long Datatime::get_seconds() {
  seconds = round(millis() / 1000);
  seconds *= 1;
  return seconds;
}


int Datatime::get_minute() {
  minute = (get_seconds() / 60);
  return minute;
}


void set_climate_plant_1(Climate &climate) {
  climate.min_temp = 20;  //C
  climate.max_temp = 22;  //C
  climate.min_air_hum = 30;  //%
  climate.max_air_hum = 45;  //%
  climate.min_soil_hum = 60;  //%
  climate.max_soil_hum = 100;  //%
  climate.fan_enable_time = 1;  //minute
  climate.fan_disable_time = 2;  //minute
}


void pin_enable() {
  pinMode(PIN_LIGHT_SENS, INPUT);
  pinMode(PIN_SOIL_SENS, INPUT);
  pinMode(PIN_HEAT_CTRL, OUTPUT);
  pinMode(PIN_PUMP_CTRL, OUTPUT);
  pinMode(PIN_LIGHT_CTRL, OUTPUT);
  pinMode(PIN_FAN_CTRL, OUTPUT);
}




Climate plant_1;

Soil_hygrometer soil_hygrometer_1(PIN_SOIL_SENS);
Light_sensor light_sensor_1(PIN_LIGHT_SENS);
Heater heater_1(PIN_HEAT_CTRL);
Fan fan_1(PIN_FAN_CTRL);
Pump pump_1(PIN_PUMP_CTRL);
Light light_1(PIN_LIGHT_CTRL);
Farm_stat farm_stat;
Datatime data;
Termometer termometr_1(dht);


void pump_control(Pump& pump, Climate& climate, Heater& heater) {
  if (pump.on_floating == 0 && heater.on_temperature == 0) {
    if (farm_stat.soil_hum < climate.min_soil_hum && farm_stat.last_pump_enable + 60 < data.get_seconds()) {
      pump.on_floating = 1;
      farm_stat.last_pump_enable = data.get_seconds();
    }
  }
  if (pump.on_floating == 1 && data.get_seconds() - farm_stat.last_pump_enable > 10) {
    pump.on_floating = 0;
  }
}


void heat_control(Pump& pump, Climate& climate, Heater& heater, Fan& fan) {
  if (farm_stat.temp < climate.min_temp && pump.on_floating == 0) {
    fan.on_blowing = 1;
    heater.on_temperature = 1;
  }
  if (farm_stat.temp > climate.min_temp + 3) {
    if (farm_stat.temp > climate.max_temp)fan.on_blowing = 1;
    else fan.on_blowing = 0;
    heater.on_temperature = 0;
  }
}


void fan_control(Climate& climate, Fan& fan) {
  if (data.get_minute() > 0 && data.get_minute() < 3)fan.on_blowing = 1;
  else fan.on_blowing = 0;
  if (farm_stat.air_hum > climate.max_air_hum)fan.on_blowing = 1;
}


void light_control(Light& light, Light_sensor& light_sensor) {
  if (light_sensor.get_light() == 1)light.on_ligt = 1;
  else light.on_ligt = 0;
}


void setup() {
  set_climate_plant_1(plant_1);
  Serial.begin(9600);
  pin_enable();
  farm_stat.last_pump_enable = 0;
}

void loop() {
  if (data.get_seconds() % 60 == 0) {
    farm_stat.temp = termometr_1.get_temp();
    farm_stat.air_hum = termometr_1.get_hum();
  }
  farm_stat.soil_hum = soil_hygrometer_1.get_hum();
  farm_stat.light_sens = light_sensor_1.get_light();

  pump_control(pump_1, plant_1, heater_1);
  heat_control(pump_1, plant_1, heater_1, fan_1);
  fan_control(plant_1, fan_1);
  light_control(light_1, light_sensor_1);


  pump_1.change(pump_1.on_floating);
  heater_1.change(heater_1.on_temperature);
  fan_1.change(fan_1.on_blowing);
  light_1.change(light_1.on_ligt);

  Serial.print("temperature: ");
  Serial.print(farm_stat.temp);
  Serial.print(" Humidity: ");
  Serial.print(farm_stat.air_hum);
  Serial.print(" Soil Humidity: ");
  Serial.print(farm_stat.soil_hum);
  Serial.print(" Light status: ");
  Serial.print(farm_stat.light_sens);
  Serial.print(" Pump status: ");
  Serial.print(pump_1.on_floating);
  Serial.print(" Heat status: ");
  Serial.print(heater_1.on_temperature);
  Serial.print(" Fan status: ");
  Serial.print(fan_1.on_blowing);
  Serial.print(" Time seconds: ");
  Serial.println(data.get_seconds());
}
