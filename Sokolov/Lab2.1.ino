#include <DFRobot_DHT11.h>
#include <Wire.h>
#include <RTClib.h>

#define FAN_PIN 7
#define LIGHT_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define LIGHT_SENSOR_PIN A0
#define SOIL_SENSOR_PIN A1
#define DHT11_PIN 13

class greenhouse_controller {
public:
    greenhouse_controller() {
        pinMode(FAN_PIN, OUTPUT);
        pinMode(LIGHT_PIN, OUTPUT);
        pinMode(PUMP_PIN, OUTPUT);
        pinMode(HEATER_PIN, OUTPUT);
        
        if (!rtc.begin()) {
            Serial.println("Error initializing RTC");
            while(true);
        }
    }

    void update() {
        read_sensors();
        control_systems();
        print_status();
    }

private:
    DFRobot_DHT11 dht;
    RTC_DS3231 rtc;

    struct {
        int light_level;
        int soil_humidity;
        int air_humidity;
        int air_temperature;
        int current_hour;
    } sensor_data;

    struct {
        int light_threshold = 70;
        int soil_humidity_threshold = 40;
        int air_humidity_threshold = 70;
        int temp_high_threshold = 25;
        int temp_low_threshold = 20;
        int sunrise_time = 7;
        int sunset_time = 20;
    } config;

    void read_sensors() {
        dht.read(DHT11_PIN);
        sensor_data.air_humidity = dht.humidity;
        sensor_data.air_temperature = dht.temperature;

        int rawLight = analogRead(LIGHT_SENSOR_PIN);
        sensor_data.light_level = map(rawLight, 980, 38, 0, 100);

        int rawSoil = analogRead(SOIL_SENSOR_PIN);
        sensor_data.soil_humidity = map(rawSoil, 1023, 0, 0, 100);

        DateTime now = rtc.now();
        sensor_data.current_hour = now.hour();
    }

    void control_systems() {
        control_light();
        control_ventilation();
        control_heater();
        control_pump();
    }

    void control_light() {
        bool isDaytime = (sensor_data.current_hour >= config.sunrise_time) && 
                        (sensor_data.current_hour < config.sunset_time);
        bool lightNeeded = sensor_data.light_level < config.light_threshold;
        digitalWrite(LIGHT_PIN, isDaytime && lightNeeded ? HIGH : LOW);
    }

    void control_ventilation() {
        bool highHumidity = sensor_data.air_humidity > config.air_humidity_threshold;
        bool highTemp = sensor_data.air_temperature > config.temp_high_threshold;
        digitalWrite(FAN_PIN, highHumidity || highTemp ? HIGH : LOW);
    }

    void control_heater() {
        bool lowTemp = sensor_data.air_temperature < config.temp_low_threshold;
        digitalWrite(HEATER_PIN, lowTemp ? HIGH : LOW);
        if(lowTemp) digitalWrite(FAN_PIN, HIGH);
    }

    void control_pump() {
        bool drySoil = sensor_data.soil_humidity < config.soil_humidity_threshold;
        digitalWrite(PUMP_PIN, drySoil ? HIGH : LOW);
    }

    void print_status() {
        static unsigned long last_print = 0;
        if(millis() - last_print < 2000) return;
        
        Serial.println("Current Status:");
        Serial.print("Light Level: "); Serial.println(sensor_data.light_level);
        Serial.print("Soil Humidity: "); Serial.println(sensor_data.soil_humidity);
        Serial.print("Air Humidity: "); Serial.println(sensor_data.air_humidity);
        Serial.print("Temperature: "); Serial.println(sensor_data.air_temperature);
        Serial.print("Time: "); Serial.println(sensor_data.current_hour);
        Serial.println("-------------------");
        
        last_print = millis();
    }
};

greenhouse_controller greenhouse;

void setup() {
    Serial.begin(9600);
}

void loop() {
    greenhouse.update();
    delay(100);
}