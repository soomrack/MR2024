#pragma once

#define PIN_LIGHT_SENS      9
#define PIN_SOIL_SENS       A1
#define PIN_HEAT_CTRL       4
#define PIN_PUMP_CTRL       5
#define PIN_LIGHT_CTRL      6
#define PIN_VENT_CTRL       7
#define PIN_DHT11_SENS      12

#define TEMP_HYST           5
#define CONTROL_WORK_TIME   7
#define CONTROL_TIMEOUT     3

struct Climate {
    int air_temp;
    int air_hum;
    int soil_hum;
    int light_day_start;
    int light_day_end;
    int plan_vent_start;
    int plan_vent_end;
};