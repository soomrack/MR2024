#include <DHT11.h>

#define DHTPIN 12 // влажность и тепмератуура воздуха
#define LAMP_PIN 6 // лампа
#define FAN_PIN 7 // вентилятор
#define NAGREV_PIN 4 // наргреватель
#define POMP_PIN 5 // помпа
#define HUM_SOIL_PIN A1 // влажность почвы
#define ILLUMINATION_PIN 9 // датчик света

DHT11 dht11(DHTPIN);


const int ledPinBlue1_pump = 2;
const int ledPinRed_heater = 3;
const int ledPinYellow_lamp = 4;
const int ledPinBlue2_ventilator = 5;

class Climate {
public:
  int temperature_min;
  int temperature_max;
  int humidity_dirt_min;
  int humidity_dirt_max;
};


class Datatime {
private:
  int hour;
public:
  Datatime(int current_time);
  int get_time();
  void set_time(int new_time);
};


class Thermometer {
private:
  int temperature;
public:
  int get_temperature();
  void get_temperature_from_thermometer();
};


class Heater {
public:
  bool on_temperature;
  long long int time_off;

  Heater();
  void start();
  void stop();
};


class Humidity_sensor {
private:
  int humidity;
public:
  Humidity_sensor();
  bool good;
  void check_dood();
  int get_humidity();
  void get_humidity_from_sensor();
};


class Humidity_mean {
private:
  int humidity;
public:
  int get_humidity();
  void set_humidity(Humidity_sensor, Humidity_sensor, Humidity_sensor);
};


class Ventilator {
public:
  bool on_temperature;
  bool on_schedule;
  bool on_after_off_heater;

  Ventilator();
  void start();
  void stop();
};


class Humidity_dirt_sensor {
private:
  int humidity_dirt;
public:
  int get_humidity_dirt();
  void get_humidity_dirt_from_sensor();
};


class Pump {
public:
  bool on_humidity_dirt;
  unsigned long int time_on;
  unsigned long int time_off;

  Pump();

  void start();
  void stop();
};


class Lamp {
public:
  bool on_schedule;

  Lamp();

  void start();
  void stop();
};



int Thermometer::get_temperature()
{
  return temperature;
}


void Thermometer::get_temperature_from_thermometer()
{
  temperature = dht11.readTemperature();
}


Humidity_sensor::Humidity_sensor()
{
  good = true;
}


int Humidity_sensor::get_humidity()
{
  return humidity;
}


void Humidity_sensor::get_humidity_from_sensor()
{
  if(good = false) {
    return;
  }

  int tmp = dht11.readHumidity();

  if(tmp > 100 || tmp < 0) {
    good = false;
    humidity = -1;
    return;
  }
  humidity = tmp;
}


int Humidity_mean::get_humidity() {
  return humidity;
}


void Humidity_mean::set_humidity(Humidity_sensor humidity_sensor_1, Humidity_sensor humidity_sensor_2, Humidity_sensor humidity_sensor_3)
{
  int tmp = 0;
  int count_good_sensors = 0;

  if(humidity_sensor_1.good) {
    count_good_sensors += 1;
    tmp += humidity_sensor_1.get_humidity();
  }

  if(humidity_sensor_2.good) {
    count_good_sensors += 1;
    tmp += humidity_sensor_2.get_humidity();
  }

  if(humidity_sensor_3.good) {
    count_good_sensors += 1;
    tmp += humidity_sensor_3.get_humidity();
  }

  if(count_good_sensors == 0) {
    Serial.println("Все датчики влажности неисправны!");
    return;
  }

  humidity = tmp / count_good_sensors;
}


int Humidity_dirt_sensor::get_humidity_dirt() {
  return humidity_dirt;
}


void Humidity_dirt_sensor::get_humidity_from_sensor()
{
  return analogRead(HUM_SOLI_PIN);
}


Datatime::Datatime(int current_time)
{
  hour = current_time;
}


int Datatime::get_time()
{
  int time = hour + millis() / 3600000;
  return time % 24;
}


void Datatime::set_time(int new_time)
{
  hour = new_time;
}


Heater::Heater()
{
  on_temperature = false;
}


void Heater::start()
{
  digitalWrite(NAGREV_PIN, true);
}


void Heater::stop()
{
  digitalWrite(NAGREV_PIN, false);
}


Ventilator::Ventilator()
{
  on_schedule = false;
  on_temperature = false;
  on_after_off_heater = false;
}


void Ventilator::start()
{
  digitalWrite(FAN_PIN, true);
}


void Ventilator::stop()
{
  digitalWrite(FAN_PIN, false);
}


Lamp::Lamp()
{
  on_schedule = false;
}


void Lamp::start()
{
  digitalWrite(LAMP_PIN, true);
}


void Lamp::stop()
{
  digitalWrite(LAMP_PIN, false);
}


Pump::Pump()
{
  on_humidity_dirt = false;
  time_on = 0;
  time_off = 0;
}

void Pump::start()
{
  digitalWrite(POMP_PIN, true);
}


void Pump::stop()
{
  digitalWrite(POMP_PIN, false);
}


void set_climate(Climate &climate)
{
  climate.temperature_max = 30;
  climate.temperature_min = 20;
  climate.humidity_dirt_max = 10;
  climate.humidity_dirt_min = 5;
}


void control_temperature(const Climate &climate,
                          const Thermometer &thermometer,
                          Heater &heater,
                          Ventilator &ventilator)
{
  if(thermometer.get_temperature() < climate.temperature_min) {
    heater.on_temperature = true;
    ventilator.on_temperature = true;
    ventilator.on_after_off_heater = false;
  }
  else if(thermometer.get_temperature() > climate.temperature_max) {
    if(heater.on_temperature) {
      ventilator.on_after_off_heater = true;
      heater.time_off = millis();
    }
    heater.on_temperature = false;
    ventilator.on_temperature = false;
  }
}

void control_ventilation(const Datatime &datatime, Ventilator &ventilator)
{
  if(datatime.get_time() < 6 || datatime.get_time() > 23) {
    ventilator.on_schedule = false;
    return;
  }

  if(datatime.get_time() % 4 == 0) {
    ventilator.on_schedule = true;
  }
  else {
    ventilator.on_schedule = false;
  }
}


void control_lighting(const Datatime &datatime, Lamp &lamp)
{
  if(datatime.get_time() > 17 && datatime.get_time() < 22) {
    lamp.on_schedule = true;
  }
  else {
    lamp.on_schedule = false;
  }
}


void control_watering(const Climate &climate, const Humidity_dirt_sensor &humidity_dirt_sensor, Pump &pump, int time_wait, int time_watering)
{
  if((humidity_dirt_sensor.get_humidity_dirt() < climate.humidity_dirt_max) && (millis() - pump.time_off > time_wait)) {
    if(pump.on_humidity_dirt == false) {
      pump.time_on = millis();
    }
    pump.on_humidity_dirt = true;
  }

  if(humidity_dirt_sensor.get_humidity_dirt() > climate.humidity_dirt_max) {
    pump.on_humidity_dirt = false;
    return;
  }

  if(millis() - pump.time_on > time_watering) {
    if(pump.on_humidity_dirt == true) {
      pump.time_off = millis();
    }
    pump.on_humidity_dirt = false;
  }
}


void control_ventilation_after_off_heater(Ventilator &ventilator, Heater &heater, int time_ventilation)
{
  if(ventilator.on_after_off_heater && (millis() - heater.time_off > time_ventilation)) {
    ventilator.on_after_off_heater = false;
  }
}


void do_heat(const Heater &heater)
{
  if(heater.on_temperature) {
    heater.start();
  }
  else {
    heater.stop();
  }
}


void do_ventilation(const Ventilator &ventilator)
{
  if(ventilator.on_schedule || ventilator.on_temperature || ventilator.on_after_off_heater) {
    ventilator.start();
  }
  else {
    ventilator.stop();
  }
}


void do_ventilation_after_off_heater(Ventilator &ventilator)
{
  if(ventilator.on_after_off_heater == true) {
    ventilator.start();
  }
  else {
    ventilator.stop();
  }
}


void do_lighting(const Lamp &lamp)
{
  if(lamp.on_schedule) {
    lamp.start();
  }
  else {
    lamp.stop();
  }
}


void do_watering(const Pump &pump, const Heater &heater)
{
  if(pump.on_humidity_dirt && heater.on_temperature == false) {
    pump.start();
  }
  else {
    pump.stop();
  }
}


Climate climate_1;
Datatime datatime(13);
Humidity_mean humidity_mean;

Thermometer thermometer_1;
Humidity_sensor humidity_sensor_1;
Humidity_sensor humidity_sensor_2;
Humidity_sensor humidity_sensor_3;
Humidity_dirt_sensor humidity_dirt_sensor_1;

Heater heater_1;
Ventilator ventilator_1;
Lamp lamp_1;
Pump pump_1;


void setup() {
  Serial.begin(9600);
  //Serial.setTimeout(10);
	
	pinMode(ledPinBlue1_pump, OUTPUT);
  pinMode(ledPinRed_heater, OUTPUT);
	pinMode(ledPinYellow_lamp, OUTPUT);
  pinMode(ledPinBlue2_ventilator, OUTPUT);
  set_climate(climate_1);
}

void loop() {
  humidity_mean.set_humidity(humidity_sensor_1, humidity_sensor_2, humidity_sensor_3);

  control_temperature(climate_1, thermometer_1, heater_1, ventilator_1);
  control_ventilation(datatime, ventilator_1);
  control_lighting(datatime, lamp_1);
  control_watering(climate_1, humidity_dirt_sensor_1, pump_1, 6000, 3000);
  control_ventilation_after_off_heater(ventilator_1, heater_1, 2000);

  do_heat(heater_1);
  do_ventilation(ventilator_1);
  do_lighting(lamp_1);
  do_watering(pump_1, heater_1);
}
