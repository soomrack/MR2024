#include <DHT.h>
#define DHTTYPE DHT11

#define DHT_PIN 12  // Датчик температуры и влажности
#define LAMP_PIN 6  // Лампа
#define FAN_PIN 7  // Вентилятор
#define HEATER_PIN 4  // Нагреватель
#define PUMP_PIN 5  // Помпа
#define HUM_SOIL_PIN A1  // Влажность почвы
#define LIGHTING_PIN A9  // Датчик освещенности

DHT dht(DHT_PIN, DHTTYPE);


class Climate {
public:
    int temperature_min;
    int temperature_max;
    
    int humidity_soil_min;
    int humidity_soil_max;

    int air_humidity_min;
    int air_humidity_max;

    int lighting_min;
    int lighting_max;
    
    int watering_interval;
};


class Soil_Humidifier 
{
public:
    int soil_humidity;
    uint8_t hum_soil_pin;
public:
    Soil_Humidifier(uint8_t pin) : hum_soil_pin(pin) {}
public:
    int get_soil_humidity();
};


class Air_Humidifier 
{
public:
    float air_humidity = NAN;
    uint8_t air_hum_pin;
public:
    Air_Humidifier(uint8_t pin) : air_hum_pin(pin) {};
public:
    void get_air_humidity();
};


class Thermometer {
public:
    float temperature = NAN;
    uint8_t thermometer_pin;
public:
    Thermometer(uint8_t pin) : thermometer_pin(pin) {};
public:
    void get_temperature();
};


class Heater {
public:
    bool on_temperature = false;
    uint8_t heater_pin = false;
public:
    Heater(uint8_t pin) : heater_pin(pin) {};
public:
    void start();
    void stop();
};


class Ventilator {
public:
    bool on_temperature = false;
    bool on_schedule = false;
    uint8_t fan_pin;
public:
    Ventilator(uint8_t pin) : fan_pin(pin){};
public:
    void start();
    void stop();
};


class Pump
{
public:
    bool is_active = false;
    int dead_time;
    uint8_t pump_pin;
public:
    Pump(uint8_t pin, int dead_time) : pump_pin(pin), dead_time(dead_time){};
public:
    void start();
    void stop();
};


class Light
{
public:
    bool is_need_to_light = false;
    uint8_t lamp_pin;
public:
    Light(uint8_t pin) : lamp_pin(pin){};
public:
    void start();
    void stop();
};



class Photoresistor {
public:
    int lighting_level = 0;
    uint8_t lighting_pin;
public:
    Photoresistor(uint8_t pin) : lighting_pin(pin){};
public:
    void get_lighting_level();
};


class Datatime {
public:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    unsigned long long lastUpdate = 0;
public:
    void update_time();
    void set_time(int h, int m, int s);
};



void Air_Humidifier::get_air_humidity()
{
    air_humidity = dht.readHumidity();
}


int Soil_Humidifier::get_soil_humidity()
{
  return analogRead(hum_soil_pin);
}


void Thermometer::get_temperature()
{
    temperature = dht.readTemperature();
}


void Datatime::set_time(int h, int m, int s) {
    hours = h;
    minutes = m;
    seconds = s;
    lastUpdate = millis();  // Сброс времени последнего обновления
}


void Datatime::update_time()
{
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    seconds++;
    
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
    }
    if (minutes >= 60) {
        minutes = 0;
        hours++;
    }
    if (hours >= 24) {
        hours = 0;
    }
  }
}


void set_climate_tomato(Climate &climate) {
    climate.temperature_max = 30;
    climate.temperature_min = 20;
    
    climate.humidity_soil_max = 10;
    climate.humidity_soil_min = 500;
    
    climate.lighting_min = 100;
    climate.lighting_max = 400;
    
    climate.watering_interval = 5000;
}


void control_temperature(Climate climate,
                         Thermometer thermometer,
                         Heater &heater,
                         Ventilator &ventilator)
{
    if(thermometer.temperature < climate.temperature_min) {
        heater.on_temperature = true;
        ventilator.on_temperature = true;
    }

    if(thermometer.temperature > climate.temperature_max) {
        heater.on_temperature = false;
        ventilator.on_temperature = true;
    }
}


void control_ventilation(Datatime datatime,
                         Ventilator &ventilator, Heater &heater)
{
    if(datatime.hours < 6 || datatime.hours > 23) {
        ventilator.on_schedule = false;
        return;
    }

    if(datatime.hours % 4 == 0) {  
        ventilator.on_schedule = true;
    }
    else {
        ventilator.on_schedule = false;
    }
}


void control_soil_humidity(Climate climate, Pump &pump, Soil_Humidifier &soil_humidifier, Heater &heater)
{
  static unsigned long long pump_last_toggle_time = 0;
  if (millis() - pump_last_toggle_time > pump.dead_time) {
        uint8_t max_soil_humidity = climate.humidity_soil_min;;
        uint8_t min_soil_humidity = climate.humidity_soil_max;;

        if (soil_humidifier.get_soil_humidity() < min_soil_humidity) {
            if(heater.on_temperature) heater.on_temperature = false;
            pump.is_active = true;
            pump_last_toggle_time = millis();
        } else if (soil_humidifier.get_soil_humidity() > max_soil_humidity) {
            pump.is_active = false;
            pump_last_toggle_time = millis();
        }
    }
}



void control_lighting(Datatime datatime, Climate &climate, Light &light, Photoresistor& photoresistor)
{
  if(datatime.hours < 6 || datatime.hours > 23) {
      light.is_need_to_light = false;
  }
  else{
      photoresistor.get_lighting_level();
      if (photoresistor.lighting_level < climate.lighting_min){
        light.is_need_to_light = true;
      }
  }
}


void do_ventilation(Ventilator ventilator)
{
    if(ventilator.on_temperature || ventilator.on_schedule) {
        ventilator.start();
    }
    else {
        ventilator.stop();
    }
}


void do_light(Light light)
{
    if(light.is_need_to_light) {
        light.start();
    }
    else{
        light.stop();
    }
}


void do_heat(Heater heater)
{
    if(heater.on_temperature) {
        heater.start();
    }
    else{
        heater.stop();
    }
}


void do_pump(Pump pump)
{
    if(pump.is_active) {
        pump.start();
    }
    else{
        pump.stop();
    }
}


void Photoresistor::get_lighting_level()
{
    lighting_level = analogRead(LIGHTING_PIN);
}


void Heater::start()
{
  digitalWrite(HEATER_PIN, true);
}


void Heater::stop()
{
  digitalWrite(HEATER_PIN, false);
}


void Ventilator::start()
{
  digitalWrite(FAN_PIN, true);
}


void Ventilator::stop()
{
  digitalWrite(FAN_PIN, false);
}


void Pump::start()
{
  digitalWrite(PUMP_PIN, true);
}


void Pump::stop()
{
  digitalWrite(PUMP_PIN, false);
}


void Light::start()
{
  digitalWrite(LAMP_PIN, true);
}


void Light::stop()
{
  digitalWrite(LAMP_PIN, false);
}


void pins_init()
{
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LIGHTING_PIN, INPUT);
}


Datatime datatime;


void setup() {
    Serial.begin(9600);
    
    dht.begin();

    datatime.set_time(15, 30, 0);

    pins_init();
}



void loop() {
    datatime.update_time();
    
    Climate climate_tomato;

    Thermometer thermometer_first(DHT_PIN);
    Thermometer thermometer_second(DHT_PIN);

    Heater heater_first(HEATER_PIN);
    Heater heater_second(HEATER_PIN);

    Ventilator ventilator_first(FAN_PIN);
    Ventilator ventilator_second(FAN_PIN);

    Soil_Humidifier soil_humidifier_first(HUM_SOIL_PIN);
    Soil_Humidifier soil_humidifier_second(HUM_SOIL_PIN);

    Air_Humidifier air_humidifier_first(DHT_PIN);
    Air_Humidifier air_humidifier_second(DHT_PIN);

    Pump pump_first(PUMP_PIN, 5000);
    Pump pump_second(PUMP_PIN, 7000);

    Light light_first(LAMP_PIN);
    Light light_second(LAMP_PIN);

    Photoresistor photoresistor_first(LIGHTING_PIN);
    Photoresistor photoresistor_second(LIGHTING_PIN);

    control_ventilation(datatime, ventilator_first, heater_first);
    control_temperature(climate_tomato, thermometer_first, heater_first, ventilator_first);
    control_soil_humidity(climate_tomato, pump_first, soil_humidifier_first, heater_first);
    control_lighting(datatime, climate_tomato, light_first, photoresistor_first);
    
    do_ventilation(ventilator_first);
    do_light(light_first);
    do_heat(heater_first);
    do_pump(pump_first);
}
