#pragma once

#define PIN_LIGHT_SENS      9
#define PIN_SOIL_SENS       A1
#define PIN_HEAT_CTRL       4
#define PIN_PUMP_CTRL       5
#define PIN_LIGHT_CTRL      6
#define PIN_VENT_CTRL       7
#define PIN_DHT11_SENS      12


#define TEMP_HYST           5   // tempreture hysteresis, °C

#define CONTROL_WORK_TIME     7   // control enable time, seconds
#define CONTROL_TIMEOUT     3   // control disable time, seconds


struct Climate
{
    int air_temp;  // °C
    int air_hum;  // %
    int soil_hum;  // parrots
    int light_day_start;  // hours
    int light_day_end;  // hours
    int plan_vent_start;  // hours
    int plan_vent_end;  // hours
};
