#include <DFRobot_DHT11.h>

#define FAN_PIN 7
#define LIGHT_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define LIGHT_SENSOR_PIN A0
#define SOIL_SENSOR_PIN1 A1
#define SOIL_SENSOR_PIN2 A1
#define SOIL_SENSOR_PIN3 A1
#define DHT11_PIN 13

struct {
    int light_threshold = 70;
    int soil_humidity_threshold = 40;
    int air_humidity_threshold = 70;
    int temp_high_threshold = 25;
    int temp_low_threshold = 20;
    int sunrise_time = 7;    // Начальное время (7:00)
    int sunset_time = 20;    // 20:00
    int pump_on_time = 5000; // Время работы помпы (5 секунд)
    int pump_off_time = 5000; // Время ожидания помпы (5 секунд)
} config;

struct {
    int light_level;
    int soil_humidity;
    int air_humidity;
    int air_temperature;
    int current_hour;
} sensor_data;

struct {
    bool is_running = false;
    unsigned long last_change = 0;
} pump_state;

DFRobot_DHT11 dht;

void setup_devices() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
}

void read_light_sensor() {
    int raw_light = analogRead(LIGHT_SENSOR_PIN);
    sensor_data.light_level = map(raw_light, 980, 38, 0, 100);
}

void read_soil_sensors() {
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
    
    sensor_data.soil_humidity = (count > 0) ? total/count : 0;
}

void read_dht_sensor() {
    dht.read(DHT11_PIN);
    sensor_data.air_humidity = dht.humidity;
    sensor_data.air_temperature = dht.temperature;

    if(dht.humidity == 0 || dht.temperature == 0) {
        Serial.println("Ошибка: Показания DHT11 недостоверны!");
    }
}

void update_time() {
    unsigned long elapsed_seconds = millis() / 1000UL; // 1 секунда = 1 час
    sensor_data.current_hour = (config.sunrise_time + elapsed_seconds) % 24;
}

void control_air() {
    bool need_fan = false;
    bool need_heater = false;

    bool high_humidity = sensor_data.air_humidity > config.air_humidity_threshold;
    bool high_temp = sensor_data.air_temperature > config.temp_high_threshold;
    
    bool low_temp = sensor_data.air_temperature < config.temp_low_threshold;

    if(low_temp) {
        need_heater = true;
        need_fan = true;
    } 
    else if(high_humidity || high_temp) {
        need_fan = true;
    }

    digitalWrite(FAN_PIN, need_fan ? HIGH : LOW);
    digitalWrite(HEATER_PIN, need_heater ? HIGH : LOW);
}

void control_light() {
    bool is_daytime = (sensor_data.current_hour >= config.sunrise_time) && 
                     (sensor_data.current_hour < config.sunset_time);
    
    bool need_light = sensor_data.light_level < config.light_threshold;
    digitalWrite(LIGHT_PIN, is_daytime && need_light ? HIGH : LOW);
}

void control_pump() {
    bool dry_soil = sensor_data.soil_humidity < config.soil_humidity_threshold;

    if (dry_soil) {
        unsigned long current_time = millis();

        if (pump_state.is_running) {
            if (current_time - pump_state.last_change >= config.pump_on_time) {
                pump_state.is_running = false;
                pump_state.last_change = current_time;
                digitalWrite(PUMP_PIN, LOW);
                Serial.println("Помпа выключена");
            }
        } else {
            if (current_time - pump_state.last_change >= config.pump_off_time) {
                pump_state.is_running = true;
                pump_state.last_change = current_time;
                digitalWrite(PUMP_PIN, HIGH);
                Serial.println("Помпа включена");
            }
        }
    } else {
        if (pump_state.is_running) {
            pump_state.is_running = false;
            digitalWrite(PUMP_PIN, LOW);
            Serial.println("Помпа выключена (почва достаточно влажная)");
        }
    }
}

void print_status() {
    static unsigned long last_print = 0;
    if(millis() - last_print < 2000) return;
    
    Serial.println("[СИСТЕМНЫЙ СТАТУС]");
    Serial.print("Время: "); Serial.print(sensor_data.current_hour); Serial.println(":00");
    Serial.print("Освещенность: "); Serial.print(sensor_data.light_level); Serial.println("%");
    Serial.print("Влажность почвы: "); Serial.print(sensor_data.soil_humidity); Serial.println("%");
    Serial.print("Температура: "); Serial.print(sensor_data.air_temperature); Serial.println("°C");
    Serial.print("Влажность воздуха: "); Serial.print(sensor_data.air_humidity); Serial.println("%");
    Serial.println("-------------------------------");
    
    last_print = millis();
}

void setup() {
    Serial.begin(9600);
    setup_devices();
}

void loop() {
    read_light_sensor();
    read_soil_sensors();
    read_dht_sensor();
    update_time();

    control_light();
    control_air();
    control_pump();

    print_status();
    
    delay(100);
}