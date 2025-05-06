#include <stdio.h>

#define DHTTYPE DHT11

#define DHT_PIN 12  // влажность и тепмератуура воздуха 
#define LAMP_PIN 6   // лампа      
#define FAN_PIN 7   // вентилятор
#define NAGREV_PIN 4   // наргреватель
#define PUMP_PIN 5    // помпа  
#define HUM_SOIL_PIN A1    // влажность почвы
#define ILLUMINATION_PIN A5   // датчик света

DHT dht(DHT_PIN, DHTTYPE); // Инициализация датчика DHT


class Climate {
public:
    int temperature_min;
    int temperature_max;
    int humidity_earth_min;
    int humidity_earth_max;
    int illumination_min;
    unsigned int watering_interval;
public:
    void print();
};

void set_climate_tomato(Climate &climate)
{
    climate.temperature_max = 30;
    climate.temperature_min = 20;
    climate.humidity_earth_max = 10;
    climate.humidity_earth_min = 5; 
    climate.illumination_min = 30;
    climate.watering_interval =5000;

}


class Thermometer {
public:
    int temperature;
public:
    void get_temperature()
    {int temperature = dht.readTemperature();};  
};


class Heater {
public:
    bool on_temperature;

public:
    Heater();
public:
    void start() {digitalWrite(NAGREV_PIN, true)};  
    void stop() {digitalWrite(NAGREV_PIN, false)};   
};


Heater::Heater()
{
    on_temperature = false;
}


class Ventilator {
public:
    bool on_temperature;
    bool on_schedule;
public:
    Ventilator();
public:
    void start() {digitalWrite(FAN_PIN, true)};  
    void stop() {digitalWrite(FAN_PIN, false);};   
};


Ventilator::Ventilator()
{
    on_schedule = false;
    on_temperature = false;
}

class Humidity_earth {
public:
    int humidity_earth;
public:
   void get_humidity_earth() {humidity_soil = analogRead(pin)}; 

};


class Humidity_air {
public:
    float humidity_air;
public:
     void get_humidity_air() {air_humidity = dht.readHumidity()}; 
};



class Light {
  private:
    bool on;
    unsigned int l_pin;
  public:
    Light();
  public:
      void start() {digitalWrite(pin, true);}  
      void stop() { digitalWrite(pin, false);} 
};


Light::Light()
{
    on = false;
}


class Pump
{
public:
    bool on = false;
    Pump();
public:
    void start() {digitalWrite(pin, true)};
    void stop() {digitalWrite(pin, false)};
};

Pump::Pump()
{
    on = false;
}


class Datatime {
public:
    int minutes = 0;
    int seconds = 0;
    unsigned long long timing=0;
public:
    void upd_datatime();
    void set_time(int m, int s); // Добавлен метод для получения времени
};


void Climate::print()
{
    printf("Climate:\n");
    printf("Temperature max: %d\n", temperature_max);
    printf("Temperature min: %d\n", temperature_min);
    printf("Humidity dirt max: %d\n", humidity_dirt_max);
    printf("Humidity dirt min: %d\n", humidity_dirt_min);
}


void Datatime::set_time(int m, int s) {
    hours = h;
    minutes = m;
    seconds = s;
    timing = millis();  // Сброс времени последнего обновления
}


void Datatime::upd_datatime()
{
    if (millis() - timing >= 1000) 
    {
      timing = millis();
      seconds++;
         
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
       }

}



void control_temperature(Climate climate, Thermometer thermometer, Heater &heater,
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


void control_ventilation(Datatime datatime,  Ventilator &ventilator)
{
    // Not ventilate at night (23:00 -- 06:00)
    if(datatime.hours < 6 || datatime.hours > 23) {
        ventilator.on_schedule = false;
        return;
    }
    
    // Ventilation every 4 hours for one hour
    if(datatime.hours % 4 == 0) {  
        ventilator.on_schedule = true;
    }
    else {
        ventilator.on_schedule = false;
    }
}


void control_light(const Datatime &datatime, Light &light)
{
   if(datatime.hours < 5 || datatime.hours > 20) {
      light.on = false;
      return;
  }
  else{
      light.on = true;
      }
  }
}


void control_earth_humidity(Climate mclimate, Pump mpump, Humidity_earth mhumidity_earth)
{
  static unsigned long int Hum_Time = 0;
  mhumidity_earth.get_humidity_earth();  // Обновляем показания датчика
  
  if (mhumidity_earth.humidity_earth > mclimate.humidity_earth_min && 
        mpump.on == false && 
        millis() - Hum_Time > 2 * mclimate.watering_interval) {
        mpump.on = true;      // Включаем полив
        Hum_Time = millis();    // Фиксируем время включения
    }
    // Выключение после истечения интервала
    else if (millis() - Hum_Time > mclimate.watering_interval) {
        mpump.on = false; 
    }
}


void control_air_humidity(Humidity_air mhumidity_air, Climate mclimate, 
Ventilator mventilator, Heater mheater) { 
    // Если влажность выше порога - включаем все вентиляторы
    if (mhumidity_air.humidity_air > mclimate.humidity_earth_min) {
            mventilator.ventilator.on = true;
        
    }
    // Иначе выключаем (если не работает обогрев)
    else { 
        if(mheater.heater.on == false) {
            for mventilator.ventilator.on = false;
        }
    }
}


void do_heat(Heater &heater)
{
    if(heater.on_temperature) {
        heater.start();
    }
    else{
        heater.stop();
    }
}


void do_ventilation(Ventilator &ventilator)
{
    if(ventilator.on_temperature || ventilator.on_schedule) {
        ventilator.start();
    }
    else {
        ventilator.stop();
    }
}

void do_light(Light &light)
{
    if(light.on) {
      light.start();
    }
    else{
      light.stop();
    }
}


void do_pump(Pump &pump)
{
    if(pump.on) {
        pump.start();
    }
    else{
        pump.stop();
    }
}



void setup() {
    Serial.begin(9600);
    dht.begin();
    datatime.set_time(22, 0);


    pinMode(LAMP_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(NAGREV_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(HUM_SOIL_PIN, INPUT);
    pinMode(ILLUMINATION_PIN, INPUT);
}

int loop()
{
    Climate climate_tomato;
    set_climate_tomato(climate_tomato);

    Datatime datatime;
    Thermometer mthermometer;
    Light light;
    Heater mheater;
    Humidity_Earth soil_sensor;
    HumidityAirSensor air_sensor;
    Ventilator mventilator;

    datatime.get_datatime();
    thermometer_first.get_temperature();
    humidity_air.get_humidity();
    light.get_light();
    
    control_temperature(climate_tomato, thermometer_second, heater_first,
     ventilator_first);
    control_earth_humidity(mclimate, mpump, mhumidity_earth)
    control_ventilation(datatime, ventilator_second);
    control_air_humidity(mhumidity_air, mclimate,mventilator, mheater)

    do_heat(mheater);
    do_ventilation(mventilator);
    do_pump(mpump);
    do_light(mlight);
    
    climate_tomato.print();

    delay(10000); // Задержка 10 секунд между циклами
}