#include "Microclimat.hpp"
#include "DHT11.h"


#define PIN_LIGHT_SENSOR 13
#define PIN_HYGROMETER A1
#define PIN_DHT_SENSOR 9


#define PIN_BACKLIGHT 6
#define PIN_PUMP 5
#define PIN_HEATER 4
#define PIN_VENTILATION 7


DateTime date_time(15, 30); 
ClimateParams climate_params; 


void pins_init() {
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    pinMode(PIN_HYGROMETER, INPUT);

    pinMode(PIN_BACKLIGHT, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_HEATER, OUTPUT);
    pinMode(PIN_VENTILATION, OUTPUT);
}


void set_target_params(ClimateParams& climate_params) {
    climate_params.temp_min = 30;
    climate_params.temp_max = 30;

    climate_params.soil_humidity_min = 40;
    climate_params.soil_humidity_max = 45;

    climate_params.air_humidity_min = 50;
    climate_params.air_humidity_max = 60;

    climate_params.lighting_min = 40;
    climate_params.lighting_max = 60;
}


/*void print_enviroment_params(DateTime& date_time, Ventilator& ventilator, Backlight& backlight, Heater& heater, Pump& pump) {
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
}*/


void setup() {
    pins_init();
    set_target_params(climate_params);
    Serial.begin(9600);
}
 

void loop() {
    static Photoresistor photoresistor(PIN_LIGHT_SENSOR);
    static Backlight backlight(PIN_BACKLIGHT);
    static DHT11 dht_array[] = {
        DHT11(PIN_DHT_SENSOR),
        DHT11(PIN_DHT_SENSOR),
    };
    static MultiDHT multi_dht(dht_array, 2);
    static Hygrometer hygrometer_array[] = {
        Hygrometer(PIN_HYGROMETER),
        Hygrometer(PIN_HYGROMETER),
        Hygrometer(PIN_HYGROMETER),
    };
    static MultiHygrometer multi_hygrometer(hygrometer_array, 3);
    static Pump pump1(PIN_PUMP, 1000);
    static Pump pump2(PIN_PUMP, 1500);
    static Heater heater(PIN_HEATER, 1000); 
    static Ventilator ventilator(PIN_VENTILATION); 

    control_temperature<MultiDHT>(climate_params, heater, multi_dht, ventilator, pump1);
    //control_temperature<MultiDHT>(climate_params, heater, multi_dht, ventilator, pump2);
    control_soil_humidity<MultiHygrometer>(climate_params, pump1, multi_hygrometer, heater);
    control_soil_humidity<MultiHygrometer>(climate_params, pump2, multi_hygrometer, heater);
    control_humidity_air<MultiDHT>(climate_params, ventilator, multi_dht);
    control_lighting<Photoresistor>(climate_params, date_time, backlight, photoresistor);

    heater.do_device();
    pump1.do_device();
    pump2.do_device();
    ventilator.do_device();
    backlight.do_device();

    //print_enviroment_params(date_time, ventilator, backlight, heater, pump);
}