#include <DFRobot_DHT11.h>

#define FAN_PIN 7
#define LIGHT_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define LIGHT_SENSOR_PIN A0
#define SOIL_SENSOR_PIN1 A1
#define SOIL_SENSOR_PIN2 A2
#define SOIL_SENSOR_PIN3 A3
#define DHT11_PIN 13

DFRobot_DHT11 dht;


struct SystemConfig {
    int light_threshold;
    int soil_humidity_threshold;
    int air_humidity_threshold;
    int temp_high_threshold;
    int temp_low_threshold;
    int sunrise_time;
    int sunset_time;
    int pump_on_time;
    int pump_off_time;
};


struct SensorData {
    int light_level;
    int soil_humidity;
    int air_humidity;
    int air_temperature;
    int current_hour;
};

// Состояние помпы
struct PumpState {
    bool is_running;
    unsigned long last_change;
};

class Device {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

class Fan : public Device {
public:
    void start() override {
        digitalWrite(FAN_PIN, HIGH);
    }
    
    void stop() override {
        digitalWrite(FAN_PIN, LOW);
    }
};

class Light : public Device {
public:
    void start() override {
        digitalWrite(LIGHT_PIN, HIGH);
    }
    
    void stop() override {
        digitalWrite(LIGHT_PIN, LOW);
    }
};

class Pump : public Device {
public:
    void start() override {
        digitalWrite(PUMP_PIN, HIGH);
    }
    
    void stop() override {
        digitalWrite(PUMP_PIN, LOW);
    }
};

class Heater : public Device {
public:
    void start() override {
        digitalWrite(HEATER_PIN, HIGH);
    }
    
    void stop() override {
        digitalWrite(HEATER_PIN, LOW);
    }
};

class LightSensor {
public:
    void read(SensorData &data) {
        int raw_light = analogRead(LIGHT_SENSOR_PIN);
        data.light_level = map(raw_light, 980, 38, 0, 100);
    }
};

class SoilSensor {
public:
    void read(SensorData &data) {
        int soil1 = analogRead(SOIL_SENSOR_PIN1);
        int soil2 = analogRead(SOIL_SENSOR_PIN2);
        int soil3 = analogRead(SOIL_SENSOR_PIN3);

        int soil_humidity1 = map(soil1, 1023, 0, 0, 100);
        int soil_humidity2 = map(soil2, 1023, 0, 0, 100);
        int soil_humidity3 = map(soil3, 1023, 0, 0, 100);

        bool sensor1_ok = true;
        bool sensor2_ok = true;
        bool sensor3_ok = true;

        if (abs(soil_humidity1 - soil_humidity2) > 20 || 
            abs(soil_humidity1 - soil_humidity3) > 20) {
            Serial.println("Ошибка: Датчик почвы 1 неисправен!");
            sensor1_ok = false;
        }
        if (abs(soil_humidity2 - soil_humidity1) > 20 || 
            abs(soil_humidity2 - soil_humidity3) > 20) {
            Serial.println("Ошибка: Датчик почвы 2 неисправен!");
            sensor2_ok = false;
        }
        if (abs(soil_humidity3 - soil_humidity1) > 20 || 
            abs(soil_humidity3 - soil_humidity2) > 20) {
            Serial.println("Ошибка: Датчик почвы 3 неисправен!");
            sensor3_ok = false;
        }

        int total = 0;
        int count = 0;
        
        if(sensor1_ok) { total += soil_humidity1; count++; }
        if(sensor2_ok) { total += soil_humidity2; count++; }
        if(sensor3_ok) { total += soil_humidity3; count++; }
        
        data.soil_humidity = (count > 0) ? total/count : 0;
    }
};

class DHTSensor {
public:
    DHTSensor() {
        dht.begin();
    }
    
    void read(SensorData &data) {
        if(dht.read(DHT11_PIN) == 0) {
            data.air_humidity = dht.humidity;
            data.air_temperature = dht.temperature;
        } else {
            Serial.println("Ошибка: Показания DHT11 недостоверны!");
        }
    }
};

class TimeManager {
public:
    void update(SensorData &data, SystemConfig &config) {
        unsigned long elapsed_seconds = millis() / 1000UL;
        data.current_hour = (config.sunrise_time + elapsed_seconds) % 24;
    }
};

class ClimateController {
public:
    void controlAir(SensorData data, SystemConfig config, Fan &fan, Heater &heater) {
        bool need_fan = false;
        bool need_heater = false;

        
        bool high_humidity = data.air_humidity > config.air_humidity_threshold;
        bool high_temp = data.air_temperature > config.temp_high_threshold;
        
        
        bool low_temp = data.air_temperature < config.temp_low_threshold;

        if(low_temp) {
            need_heater = true;
            need_fan = true;
        } 
        else if(high_humidity || high_temp) {
            need_fan = true;
        }

        if(need_fan) fan.start();
        else fan.stop();
        
        if(need_heater) heater.start();
        else heater.stop();
    }
    
    void controlLight(SensorData data, SystemConfig config, Light &light) {
        bool is_daytime = (data.current_hour >= config.sunrise_time) && 
                         (data.current_hour < config.sunset_time);
        
        bool need_light = data.light_level < config.light_threshold;
        if(is_daytime && need_light) light.start();
        else light.stop();
    }
};

class PumpController {
public:
    void control(SensorData data, SystemConfig config, Pump &pump, PumpState &state) {
        bool dry_soil = data.soil_humidity < config.soil_humidity_threshold;

        if (dry_soil) {
            unsigned long current_time = millis();

            
            if (state.is_running) {
                if (current_time - state.last_change >= config.pump_on_time) {
                    state.is_running = false;
                    state.last_change = current_time;
                    pump.stop();
                    Serial.println("Помпа выключена");
                }
            } else {
                if (current_time - state.last_change >= config.pump_off_time) {
                    state.is_running = true;
                    state.last_change = current_time;
                    pump.start();
                    Serial.println("Помпа включена");
                }
            }
        } else {
            if (state.is_running) {
                state.is_running = false;
                pump.stop();
                Serial.println("Помпа выключена (почва достаточно влажная)");
            }
        }
    }
};

class StatusPrinter {
public:
    void print(SensorData data) {
        static unsigned long last_print = 0;
        if(millis() - last_print < 2000) return;
        
        Serial.println("[СИСТЕМНЫЙ СТАТУС]");
        Serial.print("Время: "); Serial.print(data.current_hour); Serial.println(":00");
        Serial.print("Освещенность: "); Serial.print(data.light_level); Serial.println("%");
        Serial.print("Влажность почвы: "); Serial.print(data.soil_humidity); Serial.println("%");
        Serial.print("Температура: "); Serial.print(data.air_temperature); Serial.println("°C");
        Serial.print("Влажность воздуха: "); Serial.print(data.air_humidity); Serial.println("%");
        Serial.println("-------------------------------");
        
        last_print = millis();
    }
};

void setup() {
    Serial.begin(9600);
    
    pinMode(FAN_PIN, OUTPUT);
    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
    
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(LIGHT_PIN, LOW);
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(HEATER_PIN, LOW);
}

void loop() {
    static SystemConfig config = {
        70,       // light_threshold
        40,       // soil_humidity_threshold
        70,       // air_humidity_threshold
        25,       // temp_high_threshold
        20,       // temp_low_threshold
        7,        // sunrise_time
        20,       // sunset_time
        5000,     // pump_on_time
        5000      // pump_off_time
    };
    
    static SensorData sensor_data = {};
    static PumpState pump_state = {false, 0};
    
    LightSensor light_sensor;
    SoilSensor soil_sensor;
    DHTSensor dht_sensor;
    TimeManager time_manager;
    
    Fan fan;
    Light light;
    Pump pump;
    Heater heater;
    
    ClimateController climate_controller;
    PumpController pump_controller;
    StatusPrinter status_printer;
    
    
    light_sensor.read(sensor_data);
    soil_sensor.read(sensor_data);
    dht_sensor.read(sensor_data);
    time_manager.update(sensor_data, config);
    
    
    climate_controller.controlAir(sensor_data, config, fan, heater);
    climate_controller.controlLight(sensor_data, config, light);
    pump_controller.control(sensor_data, config, pump, pump_state);
    
    
    status_printer.print(sensor_data);
    
    delay(100);
}