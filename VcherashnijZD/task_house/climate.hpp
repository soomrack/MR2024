#ifndef CLIMATE_HPP
#define CLIMATE_HPP

#include <DHT11.h>

struct EnvironmentalParams {
    int min_temp;
    int max_temp;
    int min_soil_moisture;
    int max_soil_moisture;
    int min_air_humidity;
    int max_air_humidity;
    int min_light;
    int max_light;
};

class TimeManager {
private:
    uint16_t day_start_time;
    uint16_t day_end_time;
public:
    explicit TimeManager(uint16_t start_time, uint16_t end_time) noexcept
        : day_start_time(start_time), day_end_time(end_time) {}

    uint16_t get_day_start_time() const noexcept { return day_start_time; }
    uint16_t get_day_end_time() const noexcept { return day_end_time; }
    uint16_t get_current_time() const noexcept;
private:
    TimeManager() = delete;
    TimeManager(const TimeManager&) = delete;
    TimeManager(TimeManager&&) = delete;
};

class LightSensor {
private:
    const uint8_t light_sensor_pin;
public:
    explicit LightSensor(uint8_t pin) noexcept
        : light_sensor_pin(pin) {}

    uint8_t get_light_level() const noexcept { return analogRead(light_sensor_pin); }
protected:
    LightSensor() : light_sensor_pin(0) {}
};

class MultiLightSensor {
private:
    const LightSensor* light_sensors;
    const uint8_t sensor_count;
public:
    explicit MultiLightSensor(const LightSensor* sensors, uint8_t count) noexcept
        : light_sensors(sensors), sensor_count(count) {}

    uint8_t get_light_level() const noexcept;
private:
    MultiLightSensor() = delete;
    MultiLightSensor(const MultiLightSensor&) = delete;
    MultiLightSensor(MultiLightSensor&&) = delete;
};

class Illumination {
private:
    const uint8_t illumination_pin;
public:
    bool needs_illumination = false;
public:
    explicit Illumination(uint8_t pin)
        : illumination_pin(pin) {}

    void activate() const noexcept { digitalWrite(illumination_pin, HIGH); }
    void deactivate() const noexcept { digitalWrite(illumination_pin, LOW); }
    void operate_device() const noexcept;
private:
    Illumination() = delete;
    Illumination(const Illumination&) = delete;
    Illumination(Illumination&&) = delete;
};

class SoilMoistureSensor {
private:
    const uint8_t moisture_sensor_pin;
public:
    int previous_soil_moisture = 0;
public:
    explicit SoilMoistureSensor(uint8_t pin) noexcept
        : moisture_sensor_pin(pin) {}

    int get_soil_moisture() noexcept;
protected:
    SoilMoistureSensor() : moisture_sensor_pin(0) {}
};

class MultiSoilMoistureSensor {
private:
    const SoilMoistureSensor* moisture_sensors;
    const uint8_t sensor_count;
public:
    explicit MultiSoilMoistureSensor(const SoilMoistureSensor* sensors, uint8_t count) noexcept
        : moisture_sensors(sensors), sensor_count(count) {}

    uint8_t get_soil_moisture() noexcept;
private:
    MultiSoilMoistureSensor() = delete;
    MultiSoilMoistureSensor(const MultiSoilMoistureSensor&) = delete;
    MultiSoilMoistureSensor(MultiSoilMoistureSensor&&) = delete;
};

class MultiDHT {
private:
    const DHT11* dht_sensors;
    const uint8_t sensor_count;
public:
    explicit MultiDHT(const DHT11* sensors, uint8_t count) noexcept
        : dht_sensors(sensors), sensor_count(count) {}

    int read_temperature() const noexcept;
    int read_humidity() const noexcept;
private:
    MultiDHT() = delete;
    MultiDHT(const MultiDHT&) = delete;
    MultiDHT(MultiDHT&&) = delete;
};

class WaterPump {
private:
    const uint8_t pump_pin;
public:
    const uint16_t dead_time;
    bool is_active = false;
public:
    explicit WaterPump(uint8_t pin, const uint16_t dead_time) noexcept
        : pump_pin(pin), dead_time(dead_time) {}

    void activate() const noexcept { digitalWrite(pump_pin, HIGH); }
    void deactivate() const noexcept { digitalWrite(pump_pin, LOW); }
    void operate_device() const noexcept;
private:
    WaterPump() = delete;
    WaterPump(const WaterPump&) = delete;
    WaterPump(WaterPump&&) = delete;
};

class ThermalUnit {
private:
    const uint8_t heater_pin;
public:
    const uint16_t dead_time;
    bool is_active = false;
public:
    explicit ThermalUnit(uint8_t pin, const uint16_t dead_time) noexcept
        : heater_pin(pin), dead_time(dead_time) {}

    void activate() const noexcept { digitalWrite(heater_pin, HIGH); }
    void deactivate() const noexcept { digitalWrite(heater_pin, LOW); }
    void operate_device() const noexcept;
private:
    ThermalUnit() = delete;
    ThermalUnit(const ThermalUnit&) = delete;
    ThermalUnit(ThermalUnit&&) = delete;
};

class AirVent {
private:
    const uint8_t vent_pin;
    uint16_t air_out_start_time;
    uint16_t air_out_end_time;
public:
    bool needs_heating = false;
    bool needs_air_out_plan = false;
    bool needs_air_humidity_control = false;
public:
    explicit AirVent(uint8_t pin) noexcept
        : vent_pin(pin) {}

    void activate() const noexcept { digitalWrite(vent_pin, HIGH); }
    void deactivate() const noexcept { digitalWrite(vent_pin, LOW); }
    void set_air_out_plan(uint8_t start_hour, uint8_t start_minute, uint8_t end_hour, uint8_t end_minute) noexcept;
    void operate_device() const noexcept;
private:
    AirVent() = delete;
    AirVent(const AirVent&) = delete;
    AirVent(AirVent&&) = delete;
};

template<typename DHT11Type>
void manage_temperature(const EnvironmentalParams& env_params, ThermalUnit& heater, DHT11Type& dht, AirVent& vent, WaterPump& pump);

template<typename SoilMoistureSensorType>
void manage_soil_moisture(const EnvironmentalParams& env_params, WaterPump& pump, SoilMoistureSensorType& moisture_sensor, ThermalUnit& heater);

template<typename DHT11Type>
void manage_air_humidity(const EnvironmentalParams& env_params, AirVent& vent, DHT11Type& dht);

template<typename LightSensorType>
void manage_light(const EnvironmentalParams& env_params, TimeManager& time_manager, Illumination& light, LightSensorType& light_sensor);

#endif // CLIMATE_HPP

