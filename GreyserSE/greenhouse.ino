#include "DHT.h"

#define PIN_MOISTURE_SENSOR A1    
#define PIN_LIGHT_SENSOR A0     
#define PIN_DHT 8              
#define PIN_PUMP 5             
#define PIN_LIGHT 6       
#define PIN_FAN 7              
#define PIN_HEATER 4

struct Climate
{
    const int MIN_TEMP = 19;
    const int NORMAL_TEMP = 26;
    const int MAX_HUMIDITY = 90;
    const int NORMAL_HUMIDITY = 60;
    const int MIN_MOISTURE = 40;
    const int NORMAL_MOISTURE = 60;
    const int MIN_LIGHT = 30;
};


DHT dht(PIN_DHT, DHT11);


struct Data
{
    float temperature;
    float humidity;
    int moisture;
    int light; 
    unsigned long current_hour;
};
typedef struct Data Data;


struct Fan
{
    bool humidity_state = 0;
    bool temperature_state = 0;
    unsigned long activation_time = 0;
};
typedef struct Fan Fan;


struct Pump
{
    bool moisture_state = 0;
    unsigned long activation_time = 0;
};
typedef struct Pump Pump;


struct Heater
{
    bool heater_state = 0;
};
typedef struct Heater Heater;


struct Light
{
    bool light_state = 0;
    unsigned long activation_time = 0;
};
typedef struct Light Light;


Climate tomatoes_climate;
Data data;
Fan fan;
Pump pump;
Heater heater;
Light light;


void print_status()
{
    Serial.print("==Состояние теплицы==\n");
    Serial.print("Текущий час: ");
    Serial.println(data.current_hour);
    Serial.print("Влажность воздуха: ");
    Serial.println(data.humidity);
    Serial.print("Температура воздуха: ");
    Serial.println(data.temperature);
    Serial.print("Влажность почвы: ");
    Serial.println(data.moisture);
}


void read_humidity()
{
    data.humidity = dht.readHumidity();

    if (data.humidity > tomatoes_climate.MAX_HUMIDITY) {
        fan.humidity_state = true;
    } else if (data.humidity <= tomatoes_climate.NORMAL_HUMIDITY) {
        fan.humidity_state = false;
    }
}


void read_temperature()
{
    data.temperature = dht.readTemperature();

    if (data.temperature <= tomatoes_climate.MIN_TEMP) {
        heater.heater_state = true;
    } else if (data.temperature >= tomatoes_climate.NORMAL_TEMP) {
        heater.heater_state = false;
    }
}


void read_moisture()
{
    data.moisture = map(analogRead(PIN_MOISTURE_SENSOR), 1023, 0, 0, 100);

    if (data.moisture <= tomatoes_climate.MIN_MOISTURE) {
        pump.moisture_state = true;
    } else if (data.moisture >= tomatoes_climate.NORMAL_MOISTURE) {
        pump.moisture_state = false;
    }
}


void read_light()
{
    data.light = map(analogRead(PIN_LIGHT_SENSOR), 1023, 0, 0, 100);

    if (data.light < tomatoes_climate.MIN_LIGHT) {
        light.light_state = true;
    } else {
        light.light_state = false;
    }
}


void read_sensors()
{
    read_light();
    read_moisture();
    read_temperature();
    read_humidity();
}


void read_time()
{
    data.current_hour = (millis() / (1000 * 60 * 60)) % 24;
}


void manage_light()
{
    if (millis() - light.activation_time > 1000) {
        if (light.light_state) {
            digitalWrite(PIN_LIGHT, HIGH);
            light.activation_time = millis();
        } else {
            digitalWrite(PIN_LIGHT, LOW);
        }
    }
}


void manage_pump()
{
    if (millis() - pump.activation_time > 3000) {
        if (pump.moisture_state && millis() - pump.activation_time > 8000) {
            digitalWrite(PIN_PUMP, HIGH);
            pump.activation_time = millis();
        } else {
            digitalWrite(PIN_PUMP, LOW);
        }
    }
}


void manage_fan()
{
    if (heater.heater_state || fan.humidity_state) {
        digitalWrite(PIN_FAN, HIGH);
    } else {
        digitalWrite(PIN_FAN, LOW);
    }
}


void manage_heater()
{
    if (heater.heater_state) {
        digitalWrite(PIN_HEATER, HIGH);
    } else {
        digitalWrite(PIN_HEATER, LOW);
    }
}


void setup() {
    Serial.begin(9600);

    pinMode(PIN_MOISTURE_SENSOR, INPUT);
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    pinMode(PIN_DHT, INPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_LIGHT, OUTPUT);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_HEATER, OUTPUT);

    digitalWrite(PIN_PUMP, LOW);
    digitalWrite(PIN_LIGHT, LOW);
    digitalWrite(PIN_FAN, LOW);
    digitalWrite(PIN_HEATER, LOW);
}

void loop() {
    read_sensors();
    read_time();
    
    manage_light();
    manage_fan();
    manage_heater();
    manage_pump();

    print_status();
}
