#include "Microclimat.hpp"
#include "DHT11.h"


#define PIN_LIGHT_SENSOR 13
#define PIN_HYGROMETER A1
#define PIN_DHT_SENSOR 9


#define PIN_BACKLIGHT 6
#define PIN_PUMP 5
#define PIN_HEATER 4
#define PIN_VENTILATION 7


static Climate climate; 
static DateTime date_time(15, 30); 
static DHT11 dht(PIN_DHT_SENSOR);

void pins_init() {
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    pinMode(PIN_HYGROMETER, INPUT);

    pinMode(PIN_BACKLIGHT, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_HEATER, OUTPUT);
    pinMode(PIN_VENTILATION, OUTPUT);
}


void set_target_params(Climate& climate) {
    climate.target_lighting_level = 50; 
    climate.target_soil_humidity_level = 40; 
    climate.target_air_humidity_level = 30; 
    climate.target_temp_level = 30; 
}


void setup() {
    pins_init();
    set_target_params(climate);
    Serial.begin(9600);
}
 

void loop() {
    static Backlight backlight(PIN_BACKLIGHT, PIN_LIGHT_SENSOR); 
    static Pump pump(PIN_PUMP, PIN_HYGROMETER); 
    static Heater heater(PIN_HEATER, dht); 
    static Ventilator ventilator(PIN_VENTILATION, dht); 

    control_temperature(climate, heater, ventilator, pump); 
    control_soil_humidity(climate, pump, heater);
    control_humidity_air(climate, ventilator);
    control_lighting(climate, date_time, backlight); 

    do_device(heater);
    do_device(pump);
    do_device(ventilator);
    do_device(backlight);

    print_enviroment_params(date_time, ventilator, backlight, heater, pump);
}
