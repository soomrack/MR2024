#include <TroykaDHT.h>

#define PIN_LUMINOSITY_SENSOR 0
#define PIN_SOIL_HUMIDITY_SENSOR 1
#define PIN_DHT_SENSOR 8
#define PIN_LED 6
#define PIN_WATER_PUMP 5
#define PIN_HEAT 4
#define PIN_FAN 7


DHT dht_sensor(PIN_DHT_SENSOR, DHT11);


struct TargetValues {
  private:
    float min_temp = 25.0;  // ºC
    float max_temp = 35.0;  // ºC
    float air_humidity = 70.0;  // percent
    int soil_humidity = 350;  // Oms – the resistance of re
    int luminosity = 400;  //  a value in range 0 - 1023 (lighter - darker)
    const int airing_period = 30;  // minutes
  public:
    float get_min_temp() const {return min_temp;}
    float get_max_temp() const {return max_temp;}
    float get_air_humidity() const {return air_humidity;}
    int get_soil_humidity() const {return soil_humidity;}
    int get_luminosity() const {return luminosity;}
    int get_airing_period() const {return airing_period;}
};


TargetValues targets;


class State {
  private:
    unsigned long last_airing_time;
  public:
    State() : temp1(0), temp2(0), temp3(0), soil_humidity(0), air_humidity(0), luminosity(0), last_airing_time(0) {}
    float temp1;
    float temp2;
    float temp3;
    int soil_humidity;
    float air_humidity;
    int luminosity;
    float get_temp() const {return (temp1 + temp2 + temp3) / 3;};
    int get_luminosity() const {return luminosity;};
    unsigned long get_last_airing_time() const {return last_airing_time;};
    void set_airing_time() {last_airing_time = millis();};
};


State state;


unsigned long time() {
    return millis() / 60000;
}


bool is_night() {
    return (time() % 3 == 2);
}


bool is_airing_time() {
  return (millis() - state.get_last_airing_time() >= targets.get_airing_period() * 2000);
}


class Device {
  protected:
    int pin;
  public:
    Device(int pin) : pin(pin) {pinMode(pin, OUTPUT); digitalWrite(pin, LOW);}
    virtual void turn_on() { digitalWrite(pin, HIGH); }
    virtual void turn_off() { digitalWrite(pin, LOW); }
};


class Fan : public Device {
  public:
    Fan(int pin) : Device(pin) {}
};


class Heater : public Device {
  public:
    Heater(int pin) : Device(pin) {}
};


class WaterPump : public Device {
  private:
    bool is_on;
  public:
    unsigned long stop_time;
    unsigned long start_time;
    unsigned int duration;
    unsigned long timeout;
    WaterPump(int pin) : Device(pin), stop_time(0), start_time(0), duration(10 * 1000), timeout(10 * 1000), is_on(false) {}
    void turn_on () override {
      Device::turn_on();

      if (!is_on) {
        start_time = millis();
        is_on = true;
      }
    }
    void turn_off () override {
      Device::turn_off();

      if (is_on) {
        stop_time = millis();
        is_on = false;
      }
    }
    bool get_state() const {return is_on;}
};


class Light : public Device {
  public:
    Light(int pin) : Device(pin) {}
};


Fan fan(PIN_FAN);
Heater heater(PIN_HEAT);
WaterPump water_pump(PIN_WATER_PUMP);
Light light(PIN_LED);


class Task {
  public:
    virtual void execute() = 0;
};


class LightingTask : public Task {
  public:
    virtual void execute() override;
};


void LightingTask::execute() {
  if (is_night()) {
    light.turn_off();
    return;
  }
  if (state.luminosity < targets.get_luminosity()) {
    light.turn_off();
  } else {
    light.turn_on();
  }
}


class AiringTask : public Task {
  public:
    virtual void execute() override;
};


void AiringTask::execute() {
  if (is_airing_time()) {
    state.set_airing_time();
    fan.turn_on();
    return;
  }
  if (state.air_humidity > targets.get_air_humidity() || state.get_temp()  >= targets.get_max_temp()) {
    fan.turn_on();
  } else if (state.get_temp()  <= targets.get_min_temp()) {
    fan.turn_on();
  } else {
    fan.turn_off();
  }
}


class HeatingTask : public Task {
  public:
    virtual void execute() override;
};


void HeatingTask::execute() {
  if (water_pump.get_state()) {
    heater.turn_off();
    return;
  }
  if (state.get_temp()  <= targets.get_min_temp()) {
    heater.turn_on();
  } else {
    heater.turn_off();
  }
}


class WateringTask : public Task {
  public:
    void execute() override;
};


void WateringTask::execute() {
  if (state.soil_humidity > 350 && water_pump.stop_time + water_pump.timeout <= millis()){
    water_pump.turn_on();
  } else if (water_pump.start_time + water_pump.duration <= millis()){
    water_pump.turn_off();
  }
}


LightingTask light_task;
AiringTask airing_task;
HeatingTask heating_task;
WateringTask watering_task;


void setup() {
  Serial.begin(9600);
  dht_sensor.begin();

  pinMode(PIN_LUMINOSITY_SENSOR, INPUT);
  pinMode(PIN_SOIL_HUMIDITY_SENSOR, INPUT);
  pinMode(PIN_DHT_SENSOR, INPUT);
}


bool dht_has_errors() {
  switch (dht_sensor.getState()) {
    case DHT_OK:
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.println("DHT Checksum error");
      break;
    case DHT_ERROR_TIMEOUT:
      Serial.println("DHT Timeout error");
      break;
    case DHT_ERROR_NO_REPLY:
      Serial.println("DHT Connection error");
      break;
    default:
      Serial.println("DHT Unknown error");
      break;
  }
}


void loop() {
  dht_sensor.read();
  state.temp1 = dht_sensor.getTemperatureC();
  state.temp2 = dht_sensor.getTemperatureC();
  state.temp3 = dht_sensor.getTemperatureC();
  state.air_humidity = dht_sensor.getHumidity();
  state.soil_humidity = analogRead(PIN_SOIL_HUMIDITY_SENSOR);
  state.luminosity = analogRead(PIN_LUMINOSITY_SENSOR);

  dht_has_errors();

  light_task.execute();
  airing_task.execute();
  heating_task.execute();
  watering_task.execute();

  delay(100);
}