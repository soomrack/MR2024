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


void control_humidity_air(const Climate& climate, Ventilator& ventilator) {
    if(ventilator.get_air_humidity() >= climate.target_air_humidity_level) {
        ventilator.is_need_to_control_air_humidity = true;
    } else {
        ventilator.is_need_to_control_air_humidity = false;  
    }
}


void control_lighting(const Climate& climate, DateTime& date_time, Backlight& backlight) {
    uint16_t current_time = date_time.get_current_time();
    uint16_t time_start_day = date_time.get_time_start_day();
    uint16_t time_end_day = date_time.get_time_end_day();
    
    backlight.is_need_to_light = false;
    if(current_time >= time_start_day && current_time << time_end_day) {
        if(backlight.get_lighting_level() >= climate.target_lighting_level) {
            backlight.is_need_to_light = true;
        }
    } 
}


void control_temperature(const Climate& climate, Heater& heater, Ventilator& ventilator, Pump& pump) {
    static unsigned long long heater_last_toggle_time = 0;

    if (millis() - heater_last_toggle_time > 1000) {
        int max_temp = climate.target_temp_level + 2;
        int min_temp = climate.target_temp_level - 2;

        if (heater.get_temperature() < min_temp && !pump.is_active) {
            heater.is_active = true;
            ventilator.is_need_to_heater = true;
            heater_last_toggle_time = millis();
        } else if (heater.get_temperature() > max_temp) {
            heater.is_active = false;
            ventilator.is_need_to_heater = true;
            heater_last_toggle_time = millis();
        } else {
            ventilator.is_need_to_heater = false;
        }
    }

}


void control_soil_humidity(const Climate& climate, Pump& pump, Heater& heater) {
    static unsigned long long pump_last_toggle_time = 0;

    if (millis() - pump_last_toggle_time > 1000) {
        uint8_t max_soil_humidity = climate.target_soil_humidity_level + 5;
        uint8_t min_soil_humidity = climate.target_soil_humidity_level - 5;

        if (pump.get_soil_humidity() < min_soil_humidity) {
            if(heater.is_active) heater.is_active = false;
            pump.is_active = true;
            pump_last_toggle_time = millis();
        } else if (pump.get_soil_humidity() > max_soil_humidity) {
            pump.is_active = false;
            pump_last_toggle_time = millis();
        }
    }
}


void do_device(const Heater& heater) {
    if(heater.is_active) {
        heater.turn_on();
    } else {
        heater.turn_off();
    }
}


void do_device(const Pump& pump) {
    if(pump.is_active) {
        pump.turn_on();
    } else {
        pump.turn_off();
    }
}


void do_device(const Ventilator& ventilator) {
    if((ventilator.is_need_to_plan_air_out) || (ventilator.is_need_to_control_air_humidity) || (ventilator.is_need_to_heater)) {
        ventilator.turn_on();
    } else {
        ventilator.turn_off();
    }
}   


void do_device(const Backlight& backlight) {
    if(backlight.is_need_to_light) {
        backlight.turn_on();
    } else {
        backlight.turn_off();
    }
}


void print_enviroment_params(DateTime& date_time, Ventilator& ventilator, Backlight& backlight, Heater& heater, Pump& pump) {
    Serial.print("Time: ");
    Serial.println(date_time.get_current_time());
    Serial.print("Ventilation: ");
    Serial.println(ventilator.get_air_humidity());
    Serial.print(", Backlight: ");
    Serial.println(backlight.get_lighting_level());
    Serial.print("Heater: ");
    Serial.println(heater.get_temperature());
    Serial.print("Pump: ");
    Serial.println(pump.get_soil_humidity());
    Serial.println(); // For better formating
}
