#include "climate.hpp"

uint16_t TimeManager::get_current_time() const noexcept {
    uint16_t minutes = ((millis() / 60) / 1000);
    minutes %= 24 * 60;
    return minutes;
}

void AirVent::set_air_out_plan(uint8_t start_hour, uint8_t start_minute, uint8_t end_hour, uint8_t end_minute) noexcept {
    needs_air_out_plan = true;
    air_out_start_time = min(start_hour, 23) * 60 + min(start_minute, 59);
    air_out_end_time = min(end_hour, 23) * 60 + min(end_minute, 59);
}

uint8_t MultiLightSensor::get_light_level() const noexcept {
    if (sensor_count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < sensor_count; ++idx) {
        sum += light_sensors[idx].get_light_level();
    }
    return static_cast<uint8_t>(sum / sensor_count);
}

int SoilMoistureSensor::get_soil_moisture() noexcept {
    int current_soil_moisture = map(analogRead(moisture_sensor_pin), 0, 1023, 100, 0);
    previous_soil_moisture = current_soil_moisture;
    return current_soil_moisture;
}

uint8_t MultiSoilMoistureSensor::get_soil_moisture() noexcept {
    if (sensor_count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < sensor_count; ++idx) {
        uint8_t tmp = moisture_sensors[idx].get_soil_moisture();
        if(abs(tmp - moisture_sensors[idx].previous_soil_moisture) < 30) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / sensor_count);
}

int MultiDHT::read_temperature() const noexcept {
    if (sensor_count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < sensor_count; ++idx) {
        uint8_t tmp = dht_sensors[idx].readTemperature();
        if(tmp != DHT11::ERROR_CHECKSUM && tmp != DHT11::ERROR_TIMEOUT) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / sensor_count);
}

int MultiDHT::read_humidity() const noexcept {
    if (sensor_count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < sensor_count; ++idx) {
        uint8_t tmp = dht_sensors[idx].readHumidity();
        if(tmp != DHT11::ERROR_CHECKSUM && tmp != DHT11::ERROR_TIMEOUT) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / sensor_count);
}

template<typename DHT11Type>
void manage_air_humidity(const EnvironmentalParams& env_params, AirVent& vent, DHT11Type& dht) {
    if(dht.readHumidity() >= env_params.max_air_humidity) {
        vent.needs_air_humidity_control = true;
    } else {
        vent.needs_air_humidity_control = false;
    }
}

template<typename LightSensorType>
void manage_light(const EnvironmentalParams& env_params, TimeManager& time_manager, Illumination& light, LightSensorType& light_sensor) {
    uint16_t current_time = time_manager.get_current_time();
    uint16_t day_start_time = time_manager.get_day_start_time();
    uint16_t day_end_time = time_manager.get_day_end_time();

    if(current_time >= day_start_time && current_time <= day_end_time) {
        if(light_sensor.get_light_level() >= env_params.min_light) {
            light.needs_illumination = true;
        } else if (light_sensor.get_light_level() >= env_params.max_light) {
            light.needs_illumination = false;
        }
    } else {
        light.needs_illumination = false;
    }
}

template<typename DHT11Type>
void manage_temperature(const EnvironmentalParams& env_params, ThermalUnit& heater, DHT11Type& dht, AirVent& vent, WaterPump& pump) {
    static unsigned long long heater_last_toggle_time = 0;

    if (millis() - heater_last_toggle_time > heater.dead_time) {
        int max_temp = env_params.max_temp;
        int min_temp = env_params.min_temp;

        if (dht.readTemperature() < min_temp && !pump.is_active) {
            heater.is_active = true;
            vent.needs_heating = true;
            heater_last_toggle_time = millis();
        } else if (dht.readTemperature() > max_temp) {
            heater.is_active = false;
            vent.needs_heating = true;
            heater_last_toggle_time = millis();
        } else {
            vent.needs_heating = false;
        }
    }
}

template<typename SoilMoistureSensorType>
void manage_soil_moisture(const EnvironmentalParams& env_params, WaterPump& pump, SoilMoistureSensorType& moisture_sensor, ThermalUnit& heater) {
    static unsigned long long pump_last_toggle_time = 0;

    if (millis() - pump_last_toggle_time > pump.dead_time) {
        uint8_t max_soil_moisture = env_params.min_soil_moisture;
        uint8_t min_soil_moisture = env_params.max_soil_moisture;

        if (moisture_sensor.get_soil_moisture() < min_soil_moisture) {
            if(heater.is_active) heater.is_active = false;
            pump.is_active = true;
            pump_last_toggle_time = millis();
        } else if (moisture_sensor.get_soil_moisture() > max_soil_moisture) {
            pump.is_active = false;
            pump_last_toggle_time = millis();
        }
    }
}

void ThermalUnit::operate_device() const noexcept {
    if(is_active) {
        activate();
    } else {
        deactivate();
    }
}

void WaterPump::operate_device() const noexcept {
    if(is_active) {
        activate();
    } else {
        deactivate();
    }
}

void AirVent::operate_device() const noexcept {
    if((needs_air_out_plan) || (needs_air_humidity_control) || (needs_heating)) {
        activate();
    } else {
        deactivate();
    }
}

void Illumination::operate_device() const noexcept {
    if(needs_illumination) {
        activate();
    } else {
        deactivate();
    }
}

template void manage_temperature<DHT11>(const EnvironmentalParams&, ThermalUnit&, DHT11&, AirVent&, WaterPump&);
template void manage_temperature<MultiDHT>(const EnvironmentalParams&, ThermalUnit&, MultiDHT&, AirVent&, WaterPump&);
template void manage_soil_moisture<SoilMoistureSensor>(const EnvironmentalParams&, WaterPump&, SoilMoistureSensor&, ThermalUnit&);
template void manage_soil_moisture<MultiSoilMoistureSensor>(const EnvironmentalParams&, WaterPump&, MultiSoilMoistureSensor&, ThermalUnit&);
template void manage_air_humidity<DHT11>(const EnvironmentalParams&, AirVent&, DHT11&);
template void manage_air_humidity<MultiDHT>(const EnvironmentalParams&, AirVent&, MultiDHT&);
template void manage_light<LightSensor>(const EnvironmentalParams&, TimeManager&, Illumination&, LightSensor&);
template void manage_light<MultiLightSensor>(const EnvironmentalParams&, TimeManager&, Illumination&, MultiLightSensor&);

