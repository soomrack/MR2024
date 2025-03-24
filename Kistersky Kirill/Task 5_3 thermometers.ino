#include <DHT11.h>

// Sensors:
#define EXTERNAL_LIGHT_PIN A0
#define SOIL_HUMI_PIN A1
#define AIR_PIN 12

DHT11 dht11(AIR_PIN);

// Actuators:
#define WATER_PUMP_PIN 5
#define LED_LIGHT_PIN 6
#define FAN_PIN 7
#define HEAT_PIN 4

// Структура для группировки параметров Climate
struct ClimateParams 
{
    int temp_min;
    int temp_max;

    int soil_humi_min;
    int soil_humi_max;

    int air_humi_min;
    int air_humi_max;

    int light_min;
    int light_max;

    int pump_work_time;
    int pump_dead_time;

    int light_duration;
    int ventilation_duration;

    int sleep_start;
    int sleep_end;
};


class Climate {
private:
    int temp_min;
    int temp_max;

    int soil_humi_min;
    int soil_humi_max;

    int air_humi_min;
    int air_humi_max;

    int light_min;
    int light_max;

    int pump_work_time;
    int pump_dead_time;

    int light_duration;
    int ventilation_duration;

    int sleep_start;
    int sleep_end;

public:
    // Конструктор с использованием структуры ClimateParams
    Climate(const ClimateParams& params): 
          temp_min(params.temp_min), 
          temp_max(params.temp_max),

          soil_humi_min(params.soil_humi_min), 
          soil_humi_max(params.soil_humi_max),

          air_humi_min(params.air_humi_min), 
          air_humi_max(params.air_humi_max),

          light_min(params.light_min), 
          light_max(params.light_max),

          pump_work_time(params.pump_work_time), 
          pump_dead_time(params.pump_dead_time),

          light_duration(params.light_duration), 
          ventilation_duration(params.ventilation_duration),

          sleep_start(params.sleep_start), 
          sleep_end(params.sleep_end) {}

    // Геттеры
    int getTempMin() const { return temp_min; }
    int getTempMax() const { return temp_max; }
    int getSoilHumiMin() const { return soil_humi_min; }
    int getSoilHumiMax() const { return soil_humi_max; }
    int getAirHumiMin() const { return air_humi_min; }
    int getAirHumiMax() const { return air_humi_max; }
    int getLightMin() const { return light_min; }
    int getLightMax() const { return light_max; }
    int getPumpWorkTime() const { return pump_work_time; }
    int getPumpDeadTime() const { return pump_dead_time; }
    int getLightDuration() const { return light_duration; }
    int getVentilationDuration() const { return ventilation_duration; }
    int getSleepStart() const { return sleep_start; }
    int getSleepEnd() const { return sleep_end; }
};

// Создание объектов Climate с использованием структуры ClimateParams
Climate currentClimate({
    240, // TEMP_MIN
    260, // TEMP_MAX

    550, // SOIL_HUMI_MIN
    540, // SOIL_HUMI_MAX

    20, // AIR_HUMI_MIN
    30, // AIR_HUMI_MAX

    600, // LIGHT_MIN
    700, // LIGHT_MAX

    2000, // Pump work time (2 sec)
    3000, // Pump dead time (3 sec)

    1000, // Light duration (1 sec)
    60000, // Ventilation duration (1 minute)

    21, // Sleep start (hours)
    8 // Sleep end (hours)
});


Climate tomatoes({
    320, // TEMP_MIN
    340, // TEMP_MAX

    450, // SOIL_HUMI_MIN
    420, // SOIL_HUMI_MAX

    15, // AIR_HUMI_MIN
    20, // AIR_HUMI_MAX

    650, // LIGHT_MIN
    710, // LIGHT_MAX

    120000, // Pump work time (2 min)
    300000, // Pump dead time (5 min)

    300000, // Light duration (5 min)
    180000, // Ventilation duration (3 min)

    21, // Sleep start (hours)
    7 // Sleep end (hours)
});


Climate cucumbers({
    280, // TEMP_MIN
    300, // TEMP_MAX

    580, // SOIL_HUMI_MIN
    550, // SOIL_HUMI_MAX

    10, // AIR_HUMI_MIN
    15, // AIR_HUMI_MAX

    600, // LIGHT_MIN
    700, // LIGHT_MAX

    120000, // Pump work time (2 min)
    300000, // Pump dead time (5 min)

    420000, // Light duration (7 min)
    600000, // Ventilation duration (10 min)

    19, // Sleep start (hours)
    6 // Sleep end (hours)
});


typedef unsigned long int time;


time cur_millis;
unsigned int cur_hour, cur_min, cur_sec;
unsigned int ext_light, soil_humi, air_humi, air_temp, air_temp_1, air_temp_2, air_temp_3;
bool allow_heating = 1;


void pin_init() {
    pinMode(EXTERNAL_LIGHT_PIN, INPUT);
    pinMode(SOIL_HUMI_PIN, INPUT);

    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(LED_LIGHT_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(HEAT_PIN, OUTPUT);
}


void RTC_set(unsigned int hour_to_set, unsigned int min_to_set, unsigned int sec_to_set) {
    cur_hour = hour_to_set;
    cur_min = min_to_set;
    cur_sec = sec_to_set;
}


void RTC_count() {
    static time last_millis = cur_millis;

    if ((cur_millis - last_millis) >= 1000) {
        cur_sec = cur_sec + (cur_millis - last_millis) / 1000;
        last_millis = cur_millis;
    }

    if (cur_sec >= 60) {
        cur_min++;
        cur_sec = cur_sec - 60;
    }

    if (cur_min == 60) {
        cur_hour++;
        cur_min = 0;
    }

    if (cur_hour == 24) {
        cur_hour = 0;
    }
}


void RTC_print() {
    Serial.println("------------------------------");
    Serial.println("Time: " + String(cur_hour) + ":" + String(cur_min) + ":" + String(cur_sec));
}


void on(int PIN) {
    digitalWrite(PIN, 1);
}


void off(int PIN) {
    digitalWrite(PIN, 0);
}


void lighting_check() {
    static time t_light;

    if ((cur_hour < currentClimate.getSleepEnd()) || (cur_hour >= currentClimate.getSleepStart())) {
        off(LED_LIGHT_PIN);
        return;
    }

    if ((ext_light >= currentClimate.getLightMin()) && (digitalRead(LED_LIGHT_PIN) == 0)) {
        on(LED_LIGHT_PIN);
        t_light = cur_millis;
    }

    if ((cur_millis - t_light >= currentClimate.getLightDuration()) && (ext_light <= currentClimate.getLightMax())) {
        off(LED_LIGHT_PIN);
    }
}


void watering_check() {
    static time p_time;
    static time dead_time_start;

    if ((soil_humi >= currentClimate.getSoilHumiMin()) && (digitalRead(WATER_PUMP_PIN) == 0) && (cur_millis - dead_time_start >= currentClimate.getPumpDeadTime())) {
        allow_heating = 0;
        on(WATER_PUMP_PIN);
        p_time = cur_millis;
    }

    if ((digitalRead(WATER_PUMP_PIN) == 1) && (cur_millis - p_time >= currentClimate.getPumpWorkTime()) && (soil_humi <= currentClimate.getSoilHumiMax())) {
        off(WATER_PUMP_PIN);
        allow_heating = 1;
        dead_time_start = cur_millis;
    }
}


void heating_check() {
    static time t_heat;
    static int diff_1, diff_2, diff_3;

    air_temp_1 = analogRead(PIN_SEN_HEAT_EARTH_1);
    air_temp_2 = analogRead(PIN_SEN_HEAT_EARTH_2);
    air_temp_3 = analogRead(PIN_SEN_HEAT_EARTH_3);

    diff_1 = abs(air_temp_2 - air_temp_1);
    diff_2 = abs(air_temp_3 - air_temp_2);
    diff_3 = abs(air_temp_3 - air_temp_1);

    if ((diff_1 > diff_2) && (diff_1 > diff_3)) {
        Serial.println("Датчик 1 может быть дефективным.");
        air_temp = ((air_temp_2 + air_temp_3)/2);
    } else if ((diff_2 > diff_1) && (diff_2 > diff_3)) {
        Serial.println("Датчик 2 может быть дефективным.");
        air_temp = ((air_temp_1 + air_temp_3)/2);
    } else if ((diff_3 > diff_1) && (diff_3 > diff_2)) {
        Serial.println("Датчик 3 может быть дефективным.");
        air_temp = ((air_temp_1 + air_temp_2)/2);
    } else {
        Serial.println("Все датчики работают нормально.");
        air_temp = ((air_temp_1 + air_temp_2 + air_temp_3)/3);
    }

    if (!allow_heating) {
        off(FAN_PIN);
        off(HEAT_PIN);
        return;
    }

    if ((air_temp <= currentClimate.getTempMin()) && (digitalRead(FAN_PIN) == 0)) {
        on(HEAT_PIN);
        on(FAN_PIN);
        t_heat = cur_millis;
    }

    if ((cur_millis - t_heat >= currentClimate.getVentilationDuration()) && (air_temp >= currentClimate.getTempMax())) {
        off(FAN_PIN);
        off(HEAT_PIN);
    }
}


void airing_check() {
    static time t_air;

    if ((air_humi >= currentClimate.getAirHumiMax()) && (digitalRead(FAN_PIN) == 0)) {
        on(FAN_PIN);
        t_air = cur_millis;
    }

    if ((cur_millis - t_air >= currentClimate.getVentilationDuration()) && (air_humi <= currentClimate.getAirHumiMin())) {
        off(FAN_PIN);
    }
}


void pump_temp_print() {
    Serial.println("------------------------------");
    Serial.println("Status temp: " + String(digitalRead(HEAT_PIN)));
    Serial.println("Status pump: " + String(digitalRead(WATER_PUMP_PIN)));

    Serial.println("Allow heating: " + String(allow_heating));
}


void sensor_print() {
    Serial.println("------------------------------");
    Serial.print("Light: "); Serial.println(ext_light);
    Serial.print("Soil humidity: "); Serial.println(soil_humi);
    Serial.print("Air humidity: "); Serial.println(air_humi);
    Serial.print("Air temperature: "); Serial.println(air_temp);
}


void setup() {
    //currentClimate = tomatoes; // Раскомментируйте для использования настроек для томатов
    pin_init();
    RTC_set(19, 59, 16);

    Serial.begin(9600); // Для отладки
}


void loop() {
    // Чтение данных с датчиков
    ext_light = analogRead(EXTERNAL_LIGHT_PIN);
    soil_humi = analogRead(SOIL_HUMI_PIN);
    air_humi = dht11.readHumidity();
    //air_temp = dht11.readTemperature();
    cur_millis = millis();

    RTC_count();

    // Для отладки
    RTC_print();
    sensor_print();
    pump_temp_print();

    // Управление устройствами
    lighting_check();
    watering_check();
    airing_check();
    heating_check();

    delay(1000);
}
