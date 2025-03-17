#ifndef MICROCLIMATE_CONTROLLER_H
#define MICROCLIMATE_CONTROLLER_H


#include <DHT11.h>
//#include <cstdint>


#define PIN_LIGHT_SENSOR 9
#define PIN_SOIL_HUMIDITY_SENSOR A1
#define PIN_DHT_SENSOR 12


#define PIN_BACKLIGHT 6
#define PIN_PUMP 5
#define PIN_HEATER 4
#define PIN_VENTILATION 7


class MicroclimateController {
private:
    MicroclimateController();
    MicroclimateController(const MicroclimateController&) = delete;
    MicroclimateController& operator=(const MicroclimateController&) = delete;

    DHT11 dht;
    
    bool lighting_is_active = false;
    bool heater_is_active = false;
    bool pump_is_active = false;
    bool ventilation_is_active = false;
    bool is_set_plan_air_out = false;

    uint16_t air_out_start_time = 0; // In minutes
    uint16_t air_out_end_time = 0; // In minutes

    uint8_t current_lighting_level = 0;
    uint8_t current_soil_humidity = 0;
    int current_temperature = 0;
    uint8_t current_air_humidity = 0;

    uint8_t time_day_start = 7; // hours
    uint8_t time_day_end = 23; // hours
    uint8_t target_soil_humidity_level = 40; // %
    int target_temp_level = 26; // °C
    uint8_t target_air_humidity_level = 30; // %

    unsigned long last_pump_toggle_time = 0;
    unsigned long last_heater_toggle_time = 0;

    uint16_t get_current_time() const; // In minutes
    void read_sensor_data();
    void evaluate_conditions(unsigned long deadtime);
    void control_devices();
public:
    static MicroclimateController& get_instance() {
        static MicroclimateController instance;
        return instance;
    }

    void set_time_day_start(uint8_t start);
    void set_time_day_end(uint8_t end);
    void set_target_soil_humidity_level(uint8_t level);
    void set_target_temp_level(int level);
    void set_target_air_humidity_level(uint8_t level);
    void set_air_out_time(uint8_t start_hour, uint8_t start_minute, uint8_t end_hour, uint8_t end_minute);
    void update(unsigned long deadtime = 1000);
    void print_current_environment_params() const;
};


#endif // MICROCLIMATE_CONTROLLER_H
