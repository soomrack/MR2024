#include <DHT11.h>


#define PIN_LIGHT_SENS      11
#define PIN_SOIL_SENS       A1
#define PIN_HEAT_CTRL       4
#define PIN_PUMP_CTRL       5
#define PIN_LIGHT_CTRL      6
#define PIN_VENT_CTRL       7
#define PIN_DHT11_SENS      12


// TODO: configure system parameters
#define SET_TEMP            26  // °C
#define TEMP_HYST           5   // tempreture hysteresis, °C
#define LIGHT_START         8   // hour
#define LIGHT_END           21  // hour
#define PLAN_VENT_START     12  // hour
#define PLAN_VENT_END       13  // hour
#define SET_AIR_HUM         50  // %

#define CONTROL_ON_TIME     3   // control enable time, seconds
#define CONTROL_TIMEOUT     7   // control disable time, seconds


struct ctrl_data_t
{
    // sensors data
    int air_temp;  // °C
    int air_hum;  // humidity, %
    bool soil_hum;  // 1 - low soil humidity
    bool light_sens;  // 1 - low light

    // control flags
    bool enable_heater;
    bool enable_vent;
    bool enable_pump;
    bool enable_light;

    bool control_on;
    unsigned long timeout_end;
};


ctrl_data_t ctrl_data;


DHT11 dht(PIN_DHT11_SENS);


void pins_init()
{
    pinMode(PIN_LIGHT_SENS, INPUT);
    pinMode(PIN_SOIL_SENS, INPUT);

    pinMode(PIN_HEAT_CTRL, OUTPUT);
    pinMode(PIN_PUMP_CTRL, OUTPUT);
    pinMode(PIN_LIGHT_CTRL, OUTPUT);
    pinMode(PIN_VENT_CTRL, OUTPUT);
}


void setup()
{
    pins_init();

    ctrl_data.timeout_end = millis();

    Serial.begin(9600);
}


int get_time_of_day() //minutes
{
    unsigned long ms = millis();

    unsigned long min = ms / 1000 / 60;
    return min % (60 * 24);
}


void collect_sensors_data()
{
    ctrl_data.air_temp = dht.readTemperature();
    ctrl_data.air_hum = dht.readHumidity();
    ctrl_data.soil_hum = digitalRead(PIN_SOIL_SENS);
    ctrl_data.light_sens = digitalRead(PIN_LIGHT_SENS);
}


void climate_control()
{
    if(millis() > ctrl_data.timeout_end) {
        ctrl_data.control_on = !ctrl_data.control_on;
        if(ctrl_data.control_on) {
            ctrl_data.timeout_end = millis() + CONTROL_ON_TIME * 1000;
        }
        else {
            ctrl_data.timeout_end = millis() + CONTROL_TIMEOUT * 1000;
        }
    }

    ctrl_data.enable_heater = 0;
    ctrl_data.enable_vent = 0;
    ctrl_data.enable_pump = 0;
    ctrl_data.enable_light = 0;

    if(ctrl_data.control_on) {
        if(ctrl_data.air_temp < SET_TEMP - TEMP_HYST) {
            ctrl_data.enable_heater = 1;
            ctrl_data.enable_vent = 1;
        }

        if(ctrl_data.air_temp > SET_TEMP + TEMP_HYST) {
            ctrl_data.enable_vent = 1;
        }

        if(ctrl_data.air_hum > SET_AIR_HUM) {
            ctrl_data.enable_vent = 1;
        }

        if(ctrl_data.soil_hum) {
            ctrl_data.enable_pump = 1;
        }
    }

    int time = get_time_of_day();

    if(time > (LIGHT_START * 60) && time < (LIGHT_END * 60) && ctrl_data.light_sens) {
        ctrl_data.enable_light = 1;
    }

    if(time > (PLAN_VENT_START * 60) && time < (PLAN_VENT_END * 60)) {
        ctrl_data.enable_vent = 1;
    }
}


void control_apply()
{
    digitalWrite(PIN_HEAT_CTRL, ctrl_data.enable_heater);
    digitalWrite(PIN_VENT_CTRL, ctrl_data.enable_vent);
    digitalWrite(PIN_PUMP_CTRL, ctrl_data.enable_pump);
    digitalWrite(PIN_LIGHT_CTRL, ctrl_data.enable_light);
}


void print_data()
{
    Serial.println("Sensors data:");
    Serial.print("air temp: ");
    Serial.print(ctrl_data.air_temp);
    Serial.print(", air hum: ");
    Serial.print(ctrl_data.air_hum);
    Serial.print(", soil is dry: ");
    Serial.print(ctrl_data.soil_hum);
    Serial.print(", low light: ");
    Serial.println(ctrl_data.light_sens);
}


void loop()
{
    collect_sensors_data();
    climate_control();
    control_apply();
    print_data();

    delay(100);
}
