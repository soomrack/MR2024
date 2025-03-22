#pragma once

#include <DHT11.h>


struct Climate {
    uint8_t target_lighting_level = 50; // % 
    uint8_t target_soil_humidity_level = 40; // %
    uint8_t target_air_humidity_level = 30; // %
    int target_temp_level = 30; // °C
};


class DateTime {
private:
    uint16_t time_start_day;
    uint16_t time_end_day;
public:
    explicit DateTime(uint16_t time_start_day, uint16_t time_end_day) noexcept 
        : time_start_day(time_start_day), time_end_day(time_end_day) {}
    ~DateTime() = default;
    
    // In minutes
    uint16_t get_time_start_day() const noexcept { return time_start_day; }
    uint16_t get_time_end_day() const noexcept { return time_end_day; }
    uint16_t get_current_time() const noexcept;
private:
    DateTime() = delete;
    DateTime(const DateTime&) = delete;
    DateTime(DateTime&&) = delete;
};


class Backlight {
private:
    uint8_t PIN_LIGHT_SENSOR; 
    uint8_t PIN_BACKLIGHT;
public: 
    bool is_need_to_light = false;
public:
    explicit Backlight(uint8_t pin_light_sensor, uint8_t pin_backlight)
        : PIN_LIGHT_SENSOR(pin_light_sensor), PIN_BACKLIGHT(pin_backlight) {}
    ~Backlight() = default;
    
    void turn_on() const noexcept { digitalWrite(PIN_BACKLIGHT, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_BACKLIGHT, LOW); }    
    uint8_t get_lighting_level() const noexcept { return digitalRead(PIN_BACKLIGHT); }
private:
    Backlight() = delete;
    Backlight(const Backlight&) = delete;
    Backlight(Backlight&&) = delete;
};


class Pump {
private:
    const uint8_t PIN_PUMP;
    const uint8_t PIN_HYGROMETER;
public:
    bool is_active = false;
public:
    explicit Pump(uint8_t pin_pump, uint8_t pin_hygrometer) noexcept
        : PIN_PUMP(pin_pump), PIN_HYGROMETER(pin_hygrometer) {}
    ~Pump() = default;

    void turn_on() const noexcept { digitalWrite(PIN_PUMP, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_PUMP, LOW); }
    int get_soil_humidity() const noexcept { return map(analogRead(PIN_HYGROMETER), 0, 1023, 100, 0); }
private:
    Pump() = delete;
    Pump(const Pump&) = delete;
    Pump(Pump&&) = delete;
};


class Heater {
private:
    const uint8_t PIN_HEATER;
    DHT11& dht;
public:
    bool is_active = false;
public:
    explicit Heater(uint8_t pin_heater, DHT11& dht) noexcept
        : PIN_HEATER(pin_heater), dht(dht) {}
    ~Heater() = default;

    void turn_on() const noexcept { digitalWrite(PIN_HEATER, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_HEATER, LOW); }
    int get_temperature() const noexcept { return dht.readTemperature(); }
private:
    Heater() = delete;
    Heater(const Heater&) = delete;
    Heater(Heater&&) = delete;
};


class Ventilator {
private:
    const uint8_t PIN_VENTILATOR;
    DHT11& dht;
    uint16_t time_start_air_out;
    uint16_t time_end_day_air_out;
public:
    bool is_need_to_heater = false;
    bool is_need_to_plan_air_out = false;
    bool is_need_to_control_air_humidity = false;
public:
    explicit Ventilator(uint8_t pin_ventilator, DHT11& dht) noexcept 
        : PIN_VENTILATOR(pin_ventilator), dht(dht) {}
    ~Ventilator() = default;
    
    void turn_on() const noexcept { digitalWrite(PIN_VENTILATOR, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_VENTILATOR, LOW); }
    uint8_t get_air_humidity() const noexcept { return dht.readHumidity(); }
    void set_plan_air_out(uint8_t start_hour, uint8_t start_minut, uint8_t end_hour, uint8_t end_minut) noexcept;
private:
    Ventilator() = delete;
    Ventilator(const Ventilator&) = delete;
    Ventilator(Ventilator&&) = delete;
};


void control_temperature(const Climate& climate, Heater& heater, Ventilator& ventilator, Pump& pump); 


void control_soil_humidity(const Climate& climate, Pump& pump, Heater& heater);


void control_humidity_air(const Climate& climate, Ventilator& ventilator);


void control_lighting(const Climate& climate, DateTime& data_time, Backlight& backlight); 


void do_device(const Heater& heater);


void do_device(const Pump& pump);


void do_device(const Ventilator& ventilator);


void do_device(const Backlight& backlight);


void print_enviroment_params(DateTime& date_time, Ventilator& ventilator, Backlight& backlight, Heater& heater, Pump& pump);
