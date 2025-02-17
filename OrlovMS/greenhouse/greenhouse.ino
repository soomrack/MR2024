#include <DHT11.h>


#define PIN_LIGHT_SENS A0
#define PIN_SOIL_SENS A1
#define PIN_HEAT_CTRL 4
#define PIN_PUMP_CTRL 5
#define PIN_LIGHT_CTRL 6
#define PIN_VENT_CTRL 7
#define PIN_DHT11_SENS 12


#define SET_TEMP 30 // °C
#define SET_SOIL_HUM 45 // %
#define SET_LIGHT 20 // %
#define LIGHT_START 8 // hour
#define LIGHT_END 21 // hour
#define SET_AIR_HUM 80 // %


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
}


void control_heat()
{
    int temp = dht.readTemperature();

    if(temp < SET_TEMP) {
        digitalWrite(PIN_HEAT_CTRL, HIGH);
    }
    else {
        digitalWrite(PIN_HEAT_CTRL, LOW);
    }
}


void control_pump()
{
    int raw = analogRead(PIN_SOIL_SENS);
    int hum = map(raw, 0, 1023, 0, 100);

    if(hum < SET_SOIL_HUM) {
        digitalWrite(PIN_PUMP_CTRL, HIGH);
    }
    else {
        digitalWrite(PIN_PUMP_CTRL, LOW);
    }
}


int get_time_of_day() //minutes
{
    unsigned long ms = millis();

    unsigned long min = ms / 1000 / 60;
    return min % (60 * 24);
}


void control_light()
{
    int tim = get_time_of_day();

    int raw = analogRead(PIN_LIGHT_SENS);
    int light = map(raw, 0, 1023, 0, 100);

    if(tim > LIGHT_START && tim < LIGHT_END && light < SET_LIGHT) {
        digitalWrite(PIN_LIGHT_CTRL, HIGH);
    }
    else {
        digitalWrite(PIN_LIGHT_CTRL, LOW);
    }
}


void control_vent()
{
    int hum = dht.readHumidity();

    if(hum > SET_AIR_HUM) {
        digitalWrite(PIN_VENT_CTRL, HIGH);
    }
    else {
        digitalWrite(PIN_VENT_CTRL, LOW);
    }
}


void loop()
{
    control_heat();
    control_light();
    control_pump();
    control_vent();

    delay(1000);
}
