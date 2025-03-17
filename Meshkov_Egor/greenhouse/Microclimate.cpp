#include "Microclimate.hpp"
#include <Arduino.h>


MicroclimateController::MicroclimateController() : dht(PIN_DHT_SENSOR) {
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    pinMode(PIN_SOIL_HUMIDITY_SENSOR, INPUT);
    pinMode(PIN_BACKLIGHT, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_HEATER, OUTPUT);
    pinMode(PIN_VENTILATION, OUTPUT);
}


uint16_t MicroclimateController::get_current_time() const {
    uint16_t minutes = (millis() / 1000) / 60;
    uint16_t count_minutes_in_day = 24 * 60;
    return minutes % count_minutes_in_day;
}


void MicroclimateController::read_sensor_data() {
    current_lighting_level = digitalRead(PIN_LIGHT_SENSOR);
    current_soil_humidity = map(analogRead(PIN_SOIL_HUMIDITY_SENSOR), 0, 1024, 0, 100);
    current_temperature = dht.readTemperature();
    current_air_humidity = dht.readHumidity();
}


void MicroclimateController::evaluate_conditions(unsigned long deadtime) {
    uint16_t current_time = get_current_time();

    if(current_time >= (time_day_start * 60) && current_time <= (time_day_end * 60) && (current_lighting_level == 1)) {
        lighting_is_active = true;
    } else {
        lighting_is_active = false;
    }

    if (millis() - last_pump_toggle_time > deadtime) {
        uint8_t max_soil_humidity = target_soil_humidity_level + 5;
        uint8_t min_soil_humidity = target_soil_humidity_level - 5;

        if (current_soil_humidity < min_soil_humidity && !heater_is_active) {
            pump_is_active = true;
            last_pump_toggle_time = millis();
        } else if (current_soil_humidity > max_soil_humidity) {
            pump_is_active = false;
            last_pump_toggle_time = millis();
        }
    }

    if (millis() - last_heater_toggle_time > deadtime) {
        int max_temp = target_temp_level + 2;
        int min_temp = target_temp_level - 2;

        if (current_temperature < min_temp && !pump_is_active) {
            heater_is_active = true;
            ventilation_is_active = true;
            last_heater_toggle_time = millis();
        } else if (current_temperature > max_temp) {
            heater_is_active = false;
            ventilation_is_active = true;
            last_heater_toggle_time = millis();
        } else {
            ventilation_is_active = false;
        }
    }

    if(current_air_humidity > target_air_humidity_level) {
        ventilation_is_active = true;
    }

    if (is_set_plan_air_out) {
        if (current_time >= air_out_start_time && current_time <= air_out_end_time) {
            ventilation_is_active = true;
        }
    }
}

void MicroclimateController::control_devices() {
    digitalWrite(PIN_BACKLIGHT, lighting_is_active ? HIGH : LOW);
    digitalWrite(PIN_PUMP, pump_is_active ? HIGH : LOW);
    digitalWrite(PIN_HEATER, heater_is_active ? HIGH : LOW);
    digitalWrite(PIN_VENTILATION, ventilation_is_active ? HIGH : LOW);
}


void MicroclimateController::set_time_day_start(uint8_t start) {
    time_day_start = start;
}


void MicroclimateController::set_time_day_end(uint8_t end) {
    time_day_end = end;
}


void MicroclimateController::set_target_soil_humidity_level(uint8_t level) {
    target_soil_humidity_level = level;
}


void MicroclimateController::set_target_temp_level(int level) {
    target_temp_level = level;
}

void MicroclimateController::set_target_air_humidity_level(uint8_t level) {
    target_air_humidity_level = level;
}


template<typename T>
static void inline swap(T& value1, T&value2) {
    T tmp = value1;
    value1 = value2;
    value2 = tmp;
}


void MicroclimateController::set_air_out_time(uint8_t start_hour, uint8_t start_minute, uint8_t end_hour, uint8_t end_minute) {  
    air_out_start_time = min(23, start_hour) * 60 + min(59, start_minute);
    air_out_end_time = min(23, end_hour) * 60 + min(59, end_minute);

    if(air_out_start_time > air_out_end_time) swap(air_out_start_time, air_out_end_time);

    is_set_plan_air_out = true;
}


void MicroclimateController::update(unsigned long deadtime = 1000) {
    read_sensor_data();
    evaluate_conditions(deadtime);
    control_devices();
}


void MicroclimateController::print_current_environment_params() const {
    Serial.print("Current lighting: ");
    Serial.println(current_lighting_level ? "low" : "high");

    Serial.print("Current soil humidity: ");
    Serial.println(current_soil_humidity);

    Serial.print("Current temperature: ");
    Serial.println(current_temperature);

    Serial.print("Current air humidity: ");
    Serial.println(current_air_humidity);

    Serial.println(); // For better format
}
