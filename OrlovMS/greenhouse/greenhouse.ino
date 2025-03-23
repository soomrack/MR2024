#include "config.h"
#include "sensors.h"
#include "actuators.h"


DHT11 g_dht(PIN_DHT11_SENS);

Termometer g_air_temp(g_dht);
Hygrometer g_air_hum(g_dht);
SoilHygrometer g_soil_hum(PIN_SOIL_SENS);
LightSensor g_light_sens(PIN_LIGHT_SENS);

DayTime g_day_time;
WorkTime g_work_time(g_day_time, CONTROL_WORK_TIME, CONTROL_TIMEOUT);


Heater g_heater(PIN_HEAT_CTRL);
Vent g_vent(PIN_VENT_CTRL);
Pump g_pump(PIN_PUMP_CTRL);
Light g_light(PIN_LIGHT_CTRL);


Climate climate_1 = {
    .air_temp = 35,  // °C
    .air_hum = 50,  // %
    .soil_hum = 50,  // parrots
    .light_day_start = 8,  // hours
    .light_day_end = 21,  // hours
    .plan_vent_start = 12,  // hours
    .plan_vent_end = 13,  // hours
};


void setup()
{
    Serial.begin(9600);
}


void control_air_temp(Climate& config, Termometer& sensor, WorkTime& work_time, Heater& heater, Vent& vent)
{
    if(work_time.is_work_time()) {
        if(sensor.get_temp() < config.air_temp - TEMP_HYST) {
            heater.request_enable();
            vent.request_enable();
        }

        if(sensor.get_temp() > config.air_temp + TEMP_HYST) {
            vent.request_enable();
        }
    }
}


void control_air_hum(Climate& config, Hygrometer& sensor, WorkTime& work_time, Vent& vent)
{
    if(work_time.is_work_time()) {
        if(sensor.get_hum() > config.air_hum) {
            vent.request_enable();
        }
    }
}


void control_soil_hum(Climate& config, SoilHygrometer& sensor, WorkTime& work_time, Pump& pump)
{
    if(work_time.is_work_time()) {
        if(sensor.get_hum() < config.soil_hum) {
            pump.request_enable();
        }
    }
}


void plan_vent(Climate& config, DayTime& day_time, Vent& ctrl)
{
    if(day_time.is_in_interval(config.plan_vent_start * 60, config.plan_vent_end * 60)) {
        ctrl.request_enable();
    }
}


void control_light(Climate& config, LightSensor& sensor, DayTime& day_time, Light& ctrl)
{
    if(day_time.is_in_interval(config.light_day_start * 60, config.light_day_end * 60)) {
        if(!sensor.get_light()) {
            ctrl.request_enable();
        }
    }
}


void print_data()
{
    Serial.println("Sensors data:");
    Serial.print("air temp: ");
    Serial.print(g_air_temp.get_temp());
    Serial.print(", air hum: ");
    Serial.print(g_air_hum.get_hum());
    Serial.print(", soil hum: ");
    Serial.print(g_soil_hum.get_hum());
    Serial.print(", low light: ");
    Serial.println(g_light_sens.get_light());
    Serial.print("time, hour: ");
    Serial.println(g_day_time.get_day_minute() / 60.0f);
}


void loop()
{
    g_air_temp.update();
    g_air_hum.update();
    g_soil_hum.update();
    g_light_sens.update();
    g_day_time.update();
    g_work_time.update();

    control_air_temp(climate_1, g_air_temp, g_work_time, g_heater, g_vent);
    control_air_hum(climate_1, g_air_hum, g_work_time, g_vent);
    control_soil_hum(climate_1, g_soil_hum, g_work_time, g_pump);
    plan_vent(climate_1, g_day_time, g_vent);
    control_light(climate_1, g_light_sens, g_day_time, g_light);

    g_heater.apply();
    g_vent.apply();
    g_pump.apply();
    g_light.apply();

    print_data();

    delay(100);
}
