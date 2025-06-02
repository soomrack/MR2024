#include "climate.hpp"
#include "DHT11.h"

#define LIGHT_SENSOR_PIN 13
#define MOISTURE_SENSOR_PIN A1
#define DHT_SENSOR_PIN 9

#define ILLUMINATION_PIN 6
#define PUMP_PIN 5
#define HEATER_PIN 4
#define VENTILATION_PIN 7

TimeManager time_manager(15, 30);
EnvironmentalParams env_params;

void initialize_pins() {
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    pinMode(MOISTURE_SENSOR_PIN, INPUT);

    pinMode(ILLUMINATION_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(VENTILATION_PIN, OUTPUT);
}

void set_target_parameters(EnvironmentalParams& env_params) {
    env_params.min_temp = 30;
    env_params.max_temp = 30;

    env_params.min_soil_moisture = 40;
    env_params.max_soil_moisture = 45;

    env_params.min_air_humidity = 50;
    env_params.max_air_humidity = 60;

    env_params.min_light = 40;
    env_params.max_light = 60;
}

void setup() {
    initialize_pins();
    set_target_parameters(env_params);
    Serial.begin(9600);
}

void loop() {
    static LightSensor light_sensor(LIGHT_SENSOR_PIN);
    static Illumination illumination(ILLUMINATION_PIN);
    static DHT11 dht_array[] = {
        DHT11(DHT_SENSOR_PIN),
        DHT11(DHT_SENSOR_PIN),
    };
    static MultiDHT multi_dht(dht_array, 2);
    static SoilMoistureSensor moisture_sensor_array[] = {
        SoilMoistureSensor(MOISTURE_SENSOR_PIN),
        SoilMoistureSensor(MOISTURE_SENSOR_PIN),
        SoilMoistureSensor(MOISTURE_SENSOR_PIN),
    };
    static MultiSoilMoistureSensor multi_moisture_sensor(moisture_sensor_array, 3);
    static WaterPump pump1(PUMP_PIN, 1000);
    static WaterPump pump2(PUMP_PIN, 1500);
    static ThermalUnit heater(HEATER_PIN, 1000);
    static AirVent vent(VENTILATION_PIN);

    manage_temperature<MultiDHT>(env_params, heater, multi_dht, vent, pump1);
    manage_soil_moisture<MultiSoilMoistureSensor>(env_params, pump1, multi_moisture_sensor, heater);
    manage_soil_moisture<MultiSoilMoistureSensor>(env_params, pump2, multi_moisture_sensor, heater);
    manage_air_humidity<MultiDHT>(env_params, vent, multi_dht);
    manage_light<LightSensor>(env_params, time_manager, illumination, light_sensor);

    heater.operate_device();
    pump1.operate_device();
    pump2.operate_device();
    vent.operate_device();
    illumination.operate_device();
}

