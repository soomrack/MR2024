#include <DHT11.h>

#define PIN_LIGHT_SENS A0
#define PIN_SOIL_SENS A1
#define PIN_HEAT_CTRL 4
#define PIN_PUMP_CTRL 5
#define PIN_LIGHT_CTRL 6
#define PIN_FAN_CTRL 7
#define PIN_DHT11_SENS 12

DHT11 dht(PIN_DHT11_SENS);


struct Terrarium_stat {
  int temp;
  int air_hum;
  int soil_hum;
  bool light_sens;
  long last_pump_enable;
};


class Climate {
  public:
    int min_temp;
    int max_temp;
    int min_air_hum;
    int max_air_hum;
    int min_soil_hum;
    int max_soil_hum;
    int fan_enable_time;
    int fan_disable_time;
  public:
    void print_stat();
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
    int hour;
    unsigned long seconds;
  public:
    unsigned long get_seconds();
    int get_hour();
};


unsigned long Datatime::get_seconds() {
  seconds = round(millis() / 1000);
  seconds *= 1;
  return seconds;
}


int Datatime::get_hour() {
  hour = (get_seconds() / 60 / 60) % 24;
  return hour;
}


void set_climate_plant_1(Climate &climate) {
  climate.min_temp = 20;  //C
  climate.max_temp = 25;  //C
  climate.min_air_hum = 50;  //%
  climate.max_air_hum = 80;  //%
  climate.min_soil_hum = 10;  //%
  climate.max_soil_hum = 30;  //%
  climate.fan_enable_time = 7;  //hours
  climate.fan_disable_time = 21;  //hours
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
Terrarium_stat terrarium_stat;
Datatime data;
Termometer termometr_1(dht);


void pump_control(Pump& pump, Climate& climate, Heater& heater) {
  if (pump.on_floating == 0 && heater.on_temperature == 0) {
    if (terrarium_stat.soil_hum < climate.min_soil_hum && terrarium_stat.last_pump_enable + 20 * 60 < data.get_seconds()) {
      pump.on_floating = 1;
      terrarium_stat.last_pump_enable = data.get_seconds();
    }
  }
  if (pump.on_floating == 1 && data.get_seconds() - terrarium_stat.last_pump_enable > 15) {
    pump.on_floating = 0;
  }
}


void heat_control(Pump& pump, Climate& climate, Heater& heater, Fan& fan) {
  if (terrarium_stat.temp < climate.min_temp && pump.on_floating == 0) {
    fan.on_blowing = 1;
    heater.on_temperature = 1;
  }
  if (terrarium_stat.temp > climate.min_temp + 3) {
    if (terrarium_stat.temp > climate.max_temp)fan.on_blowing = 1;
    else fan.on_blowing = 0;
    heater.on_temperature = 0;
  }
}


void fan_control(Climate& climate, Fan& fan) {
  if (data.get_hour() > 8 && data.get_hour() < 21)fan.on_blowing = 1;
  else fan.on_blowing = 0;
  if (terrarium_stat.air_hum > climate.max_air_hum)fan.on_blowing = 1;
}


void light_control(Light& light, Light_sensor& light_sensor) {
  if (light_sensor.get_light() == 1)light.on_ligt = 1;
  else light.on_ligt = 0;
}


void setup() {
  set_climate_plant_1(plant_1);
  Serial.begin(9600);
  pin_enable();
  terrarium_stat.last_pump_enable = 0;
}

void loop() {
  if (data.get_seconds() % 60 == 0) {
    terrarium_stat.temp = termometr_1.get_temp();
    terrarium_stat.air_hum = termometr_1.get_hum();
  }
  terrarium_stat.soil_hum = soil_hygrometer_1.get_hum();
  terrarium_stat.light_sens = light_sensor_1.get_light();

  pump_control(pump_1, plant_1, heater_1);
  heat_control(pump_1, plant_1, heater_1, fan_1);
  fan_control(plant_1, fan_1);
  light_control(light_1, light_sensor_1);


  pump_1.change(pump_1.on_floating);
  //heat_1.change(heat_1.on_temperature);
  fan_1.change(fan_1.on_blowing);
  light_1.change(light_1.on_ligt);

  Serial.print("temperature: ");
  Serial.print(terrarium_stat.temp);
  Serial.print(" Humidity: ");
  Serial.print(terrarium_stat.air_hum);
  Serial.print(" Soil Humidity: ");
  Serial.print(terrarium_stat.soil_hum);
  Serial.print(" Light status: ");
  Serial.print(terrarium_stat.light_sens);
  Serial.print(" Pump status: ");
  Serial.print(pump_1.on_floating);
  Serial.print(" Heat status: ");
  Serial.print(heater_1.on_temperature);
  Serial.print(" Fan status: ");
  Serial.print(fan_1.on_blowing);
  Serial.print(" Time seconds: ");
  Serial.println(data.get_seconds());
}
