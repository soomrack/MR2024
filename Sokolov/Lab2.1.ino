#include <DFRobot_DHT11.h>

#define FAN_PIN 7
#define LIGHT_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define LIGHT_SENSOR_PIN A0
#define SOIL_SENSOR_PIN A1
#define DHT11_PIN 13


struct {
    int light_threshold = 70;
    int soil_humidity_threshold = 40;
    int air_humidity_threshold = 70;
    int temp_high_threshold = 25;
    int temp_low_threshold = 20;
    int sunrise_time = 7;
    int sunset_time = 20;
} config;

struct {
    int light_level;
    int soil_humidity;
    int air_humidity;
    int air_temperature;
    int current_hour;
} sensor_data;

DFRobot_DHT11 dht;


void setup_devices() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
}


void read_sensors() {
    dht.read(DHT11_PIN);
    sensor_data.air_humidity = dht.humidity;
    sensor_data.air_temperature = dht.temperature;

    int raw_light = analogRead(LIGHT_SENSOR_PIN);
    sensor_data.light_level = map(raw_light, 980, 38, 0, 100);

    int raw_soil = analogRead(SOIL_SENSOR_PIN);
    sensor_data.soil_humidity = map(raw_soil, 1023, 0, 0, 100);

    unsigned long elapsed_hours = millis() / 3600000;
    sensor_data.current_hour = (config.sunrise_time + elapsed_hours) % 24;
}


void control_light() {
    bool is_daytime = (sensor_data.current_hour >= config.sunrise_time) && 
                     (sensor_data.current_hour < config.sunset_time);
    
    bool need_light = sensor_data.light_level < config.light_threshold;
    digitalWrite(LIGHT_PIN, is_daytime && need_light ? HIGH : LOW);
}


void control_fan() {
    bool high_humidity = sensor_data.air_humidity > config.air_humidity_threshold;
    bool high_temp = sensor_data.air_temperature > config.temp_high_threshold;
    digitalWrite(FAN_PIN, high_humidity || high_temp ? HIGH : LOW);
}


void control_heater() {
    bool low_temp = sensor_data.air_temperature < config.temp_low_threshold;
    digitalWrite(HEATER_PIN, low_temp ? HIGH : LOW);
    if(low_temp) digitalWrite(FAN_PIN, HIGH);
}


void control_pump() {
    bool dry_soil = sensor_data.soil_humidity < config.soil_humidity_threshold;
    digitalWrite(PUMP_PIN, dry_soil ? HIGH : LOW);
}


void print_status() {
    static unsigned long last_print = 0;
    if(millis() - last_print < 2000) return;
    
    Serial.println("Текущее состояние:");
    Serial.print("Освещенность: "); Serial.print(sensor_data.light_level); Serial.println("%");
    Serial.print("Влажность почвы: "); Serial.print(sensor_data.soil_humidity); Serial.println("%");
    Serial.print("Влажность воздуха: "); Serial.print(sensor_data.air_humidity); Serial.println("%");
    Serial.print("Температура: "); Serial.print(sensor_data.air_temperature); Serial.println("°C");
    Serial.print("Текущее время: "); Serial.print(sensor_data.current_hour); Serial.println(":00");
    Serial.println("----------------------------------");
    
    last_print = millis();
}


void setup() {
    Serial.begin(9600);
    setup_devices();
}

void loop() {
    read_sensors();
    control_light();
    control_fan();
    control_heater();
    control_pump();
    print_status();
    
    delay(100);
}