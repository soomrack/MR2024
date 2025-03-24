#include "Microclimat.hpp"


uint16_t DateTime::get_current_time() const noexcept {
    uint16_t minutes = ((millis() / 60) / 1000);
    minutes %= 24 * 60;
    return minutes;
}


void Ventilator::set_plan_air_out(uint8_t start_hour, uint8_t start_minut, uint8_t end_hour, uint8_t end_minut) noexcept {
    is_need_to_plan_air_out = true;
    time_start_air_out = min(start_hour, 23) * 60 + min(start_minut, 59);
    time_end_day_air_out = min(start_hour, 23) * 60 + min(start_minut, 59);
}


uint8_t MultiPhotoresistor::get_lighting_level() const noexcept {
    if (count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < count; ++idx) {
        sum += sensors[idx].get_lighting_level();
    }
    return static_cast<uint8_t>(sum / count);
}


int Hygrometer::get_soil_humidity() noexcept { 
    int curr_soil_humidity = map(analogRead(PIN_HYGROMETER), 0, 1023, 100, 0);
    prev_soil_humidity = curr_soil_humidity;
    return curr_soil_humidity;
}


uint8_t MultiHygrometer::get_soil_humidity() noexcept {
    if (count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < count; ++idx) {
        uint8_t tmp = sensors[idx].get_soil_humidity();
        if(abs(tmp - sensors[idx].prev_soil_humidity) < 30) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / count);
}


int MultiDHT::readTemperature() const noexcept {
    if (count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < count; ++idx) {
        uint8_t tmp = sensors[idx].readTemperature();
        if(tmp != DHT11::ERROR_CHECKSUM && tmp != DHT11::ERROR_TIMEOUT) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / count);
}


int MultiDHT::readHumidity() const noexcept {
    if (count == 0) return 0;
    uint16_t sum = 0;
    for (size_t idx = 0; idx < count; ++idx) {
        uint8_t tmp = sensors[idx].readHumidity();
        if(tmp != DHT11::ERROR_CHECKSUM && tmp != DHT11::ERROR_TIMEOUT) {
            sum += tmp;
        }
    }
    return static_cast<uint8_t>(sum / count);
}


template<typename DHT11Type>
void control_humidity_air(const ClimateParams& climate_params, Ventilator& ventilator, DHT11Type& dht) {
    if(dht.readHumidity() >= climate_params.air_humidity_max) {
        ventilator.is_need_to_control_air_humidity = true;
    } else {
        ventilator.is_need_to_control_air_humidity = false;  
    }
}


template<typename PhotoresistorType>
void control_lighting(const ClimateParams& climate_params, DateTime& date_time, Backlight& backlight, PhotoresistorType& photoresistor) {
    uint16_t current_time = date_time.get_current_time();
    uint16_t time_start_day = date_time.get_time_start_day();
    uint16_t time_end_day = date_time.get_time_end_day();

    if(current_time >= time_start_day && current_time <= time_end_day) {
        if(photoresistor.get_lighting_level() >= climate_params.lighting_min) {
            backlight.is_need_to_light = true;
        } else if (photoresistor.get_lighting_level() >= climate_params.lighting_max) {
            backlight.is_need_to_light = false;
        }
    } else {
        backlight.is_need_to_light = false;
    }
}


template<typename DHT11Type>
void control_temperature(const ClimateParams& climate_params, Heater& heater, DHT11Type& dht, Ventilator& ventilator, Pump& pump) {
    static unsigned long long heater_last_toggle_time = 0;

    if (millis() - heater_last_toggle_time > heater.dead_time) {
        int temp_max = climate_params.temp_max;
        int temp_min = climate_params.temp_min;

        if (dht.readTemperature() < temp_min && !pump.is_active) {
            heater.is_active = true;
            ventilator.is_need_to_heater = true;
            heater_last_toggle_time = millis();
        } else if (dht.readTemperature() > temp_max) {
            heater.is_active = false;
            ventilator.is_need_to_heater = true;
            heater_last_toggle_time = millis();
        } else {
            ventilator.is_need_to_heater = false;
        }
    }

}


template<typename HygrometerType>
void control_soil_humidity(const ClimateParams& climate_params, Pump& pump, HygrometerType& hygrometer, Heater& heater) {
    static unsigned long long pump_last_toggle_time = 0;

    if (millis() - pump_last_toggle_time > pump.dead_time) {
        uint8_t max_soil_humidity = climate_params.soil_humidity_min;
        uint8_t min_soil_humidity = climate_params.soil_humidity_max;

        if (hygrometer.get_soil_humidity() < min_soil_humidity) {
            if(heater.is_active) heater.is_active = false;
            pump.is_active = true;
            pump_last_toggle_time = millis();
        } else if (hygrometer.get_soil_humidity() > max_soil_humidity) {
            pump.is_active = false;
            pump_last_toggle_time = millis();
        }
    }
}


void Heater::do_device() const noexcept {
    if(is_active) {
        turn_on();
    } else {
        turn_off();
    }
}


void Pump::do_device() const noexcept {
    if(is_active) {
        turn_on();
    } else {
        turn_off();
    }
}


void Ventilator::do_device() const noexcept {
    if((is_need_to_plan_air_out) || (is_need_to_control_air_humidity) || (is_need_to_heater)) {
        turn_on();
    } else {
        turn_off();
    }
}   


void Backlight::do_device() const noexcept {
    if(is_need_to_light) {
        turn_on();
    } else {
        turn_off();
    }
}


template void control_temperature<DHT11>(
    const ClimateParams&, Heater&, DHT11&, Ventilator&, Pump&);
    
template void control_temperature<MultiDHT>(
    const ClimateParams&, Heater&, MultiDHT&, Ventilator&, Pump&);

template void control_soil_humidity<Hygrometer>(
    const ClimateParams&, Pump&, Hygrometer&, Heater&);

template void control_soil_humidity<MultiHygrometer>(
    const ClimateParams&, Pump&, MultiHygrometer&, Heater&);

template void control_humidity_air<DHT11>(
    const ClimateParams&, Ventilator&, DHT11&);

template void control_humidity_air<MultiDHT>(
    const ClimateParams&, Ventilator&, MultiDHT&);

template void control_lighting<Photoresistor>(
    const ClimateParams&, DateTime&, Backlight&, Photoresistor&);

template void control_lighting<MultiPhotoresistor>(
    const ClimateParams&, DateTime&, Backlight&, MultiPhotoresistor&);