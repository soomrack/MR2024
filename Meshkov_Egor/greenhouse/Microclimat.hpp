#pragma once


#include <DHT11.h>


struct ClimateParams {
    int temp_min;
    int temp_max;

    int soil_humidity_min;
    int soil_humidity_max;

    int air_humidity_min;
    int air_humidity_max;

    int lighting_min;
    int lighting_max;
};


class DateTime {
private:
    uint16_t time_start_day;
    uint16_t time_end_day;
public:
    explicit DateTime(uint16_t time_start_day, uint16_t time_end_day) noexcept 
        : time_start_day(time_start_day), time_end_day(time_end_day) {}
    
    // In minutes
    uint16_t get_time_start_day() const noexcept { return time_start_day; }
    uint16_t get_time_end_day() const noexcept { return time_end_day; }
    uint16_t get_current_time() const noexcept;
private:
    DateTime() = delete;
    DateTime(const DateTime&) = delete;
    DateTime(DateTime&&) = delete;
};


class Photoresistor {
private:
    const uint8_t PIN_PHOTORESISTOR;
public:
    explicit Photoresistor(uint8_t pin_photoresistor) noexcept
        : PIN_PHOTORESISTOR(pin_photoresistor) {}

    uint8_t get_lighting_level() const noexcept { return analogRead(PIN_PHOTORESISTOR); }
protected:
    Photoresistor() : PIN_PHOTORESISTOR(0) {}
};


class MultiPhotoresistor : Photoresistor {
private:
    const Photoresistor* sensors;
    const uint8_t count;
public:
    explicit MultiPhotoresistor(const Photoresistor* sensors_array, uint8_t sensor_count) noexcept
        : sensors(sensors_array), count(sensor_count) {}

    uint8_t get_lighting_level() const noexcept;
private:
    MultiPhotoresistor() = delete;
    MultiPhotoresistor(const MultiPhotoresistor&) = delete;
    MultiPhotoresistor(MultiPhotoresistor&&) = delete;
};


class Backlight {
private:
    const uint8_t PIN_BACKLIGHT;
public: 
    bool is_need_to_light = false;
public:
    explicit Backlight(uint8_t pin_backlight)
        : PIN_BACKLIGHT(pin_backlight) {}
    
    void turn_on() const noexcept { digitalWrite(PIN_BACKLIGHT, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_BACKLIGHT, LOW); }
    void do_device() const noexcept;    
private:
    Backlight() = delete;
    Backlight(const Backlight&) = delete;
    Backlight(Backlight&&) = delete;
};


class Hygrometer {
private:
    const uint8_t PIN_HYGROMETER;
public:
    int prev_soil_humidity = 0;
public:
    explicit Hygrometer(uint8_t pin_hygrometer) noexcept
        : PIN_HYGROMETER(pin_hygrometer) {}

    int get_soil_humidity() noexcept;
protected:
    Hygrometer() : PIN_HYGROMETER(0) {}
};


class MultiHygrometer {
private:
    const Hygrometer* sensors;
    const uint8_t count;
public:
    explicit MultiHygrometer(const Hygrometer* sensors_array, uint8_t sensor_count) noexcept
        : sensors(sensors_array), count(sensor_count) {}

    uint8_t get_soil_humidity() noexcept;
private:
    MultiHygrometer() = delete;
    MultiHygrometer(const MultiHygrometer&) = delete;
    MultiHygrometer(MultiHygrometer&&) = delete;
};


class MultiDHT : DHT11 {
private:
    const DHT11* sensors;
    const uint8_t count;
public:
    explicit MultiDHT(const DHT11* sensors_array, uint8_t sensor_count) noexcept
        : DHT11(0), sensors(sensors_array), count(sensor_count) {}

    int readTemperature() const noexcept;
    int readHumidity() const noexcept;
private:
    MultiDHT() = delete;
    MultiDHT(const MultiHygrometer&) = delete;
    MultiDHT(MultiHygrometer&&) = delete;
};


class Pump {
private:
    const uint8_t PIN_PUMP;
public:
    const uint16_t dead_time;
    bool is_active = false;
public:
    explicit Pump(uint8_t pin_pump, const uint16_t dead_time) noexcept
        : PIN_PUMP(pin_pump), dead_time(dead_time) {}

    void turn_on() const noexcept { digitalWrite(PIN_PUMP, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_PUMP, LOW); }
    void do_device() const noexcept;
private:
    Pump() = delete;
    Pump(const Pump&) = delete;
    Pump(Pump&&) = delete;
};


class Heater {
private:
    const uint8_t PIN_HEATER;
public:
    const uint16_t dead_time;
    bool is_active = false;
public:
    explicit Heater(uint8_t pin_heater, const uint16_t dead_time) noexcept
        : PIN_HEATER(pin_heater), dead_time(dead_time) {}

    void turn_on() const noexcept { digitalWrite(PIN_HEATER, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_HEATER, LOW); }
    void do_device() const noexcept;
private:
    Heater() = delete;
    Heater(const Heater&) = delete;
    Heater(Heater&&) = delete;
};


class Ventilator {
private:
    const uint8_t PIN_VENTILATOR;
    uint16_t time_start_air_out;
    uint16_t time_end_day_air_out;
public:
    bool is_need_to_heater = false;
    bool is_need_to_plan_air_out = false;
    bool is_need_to_control_air_humidity = false;
public:
    explicit Ventilator(uint8_t pin_ventilator) noexcept 
        : PIN_VENTILATOR(pin_ventilator) {}  

    void turn_on() const noexcept { digitalWrite(PIN_VENTILATOR, HIGH); }
    void turn_off() const noexcept { digitalWrite(PIN_VENTILATOR, LOW); }
    void set_plan_air_out(uint8_t start_hour, uint8_t start_minut, uint8_t end_hour, uint8_t end_minut) noexcept;
    void do_device() const noexcept;
private:
    Ventilator() = delete;
    Ventilator(const Ventilator&) = delete;
    Ventilator(Ventilator&&) = delete;
};


template<typename DHT11Type>
void control_temperature(const ClimateParams& climate_params, Heater& heater, DHT11Type& dht, Ventilator& ventilator, Pump& pump); 


template<typename HygrometerType>
void control_soil_humidity(const ClimateParams& climate_params, Pump& pump, HygrometerType& hygrometer, Heater& heater);


template<typename DHT11Type>
void control_humidity_air(const ClimateParams& climate_params, Ventilator& ventilator, DHT11Type& dht);


template<typename PhotoresistorType>
void control_lighting(const ClimateParams& climate_params, DateTime& date_time, Backlight& backlight, PhotoresistorType& photoresistor);