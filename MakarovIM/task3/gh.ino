#include <DHT.h>

#define PIN_DHT1        2    // датчик температуры 1
#define PIN_DHT2        3    // датчик температуры 2
#define PIN_DHT3        4    // датчик температуры 3
#define PIN_MOISTURE    A1   
#define PIN_LIGHT_SENS  A0   
#define PIN_PUMP        5    
#define PIN_LIGHT       6    
#define PIN_FAN         7    
#define PIN_HEATER      8    


struct Climate {
    int temperature_min;
    int temperature_max;
    int humidity_dirt_min;  
    int humidity_dirt_max;  
    int light_min;          
    void printStatus();
};


class Thermometer {
public:
    Thermometer(uint8_t pin): _dht(pin, DHT11) { _pin = pin; }
    float read();
private:
    uint8_t _pin;
    DHT _dht;
};


class Heater {
public:
    Heater(uint8_t pin): _pin(pin), on_temp(false) {}
    void start() { digitalWrite(_pin, HIGH); }
    void stop()  { digitalWrite(_pin, LOW);  }
    bool on_temp;
private:
    uint8_t _pin;
};


class Ventilator {
public:
    Ventilator(uint8_t pin): _pin(pin), on_temp(false), on_schedule(false) {}
    void start() { digitalWrite(_pin, HIGH); }
    void stop()  { digitalWrite(_pin, LOW);  }
    bool on_temp;    
    bool on_schedule;
private:
    uint8_t _pin;
};


class Pump {
public:
    Pump(uint8_t pin): _pin(pin), on_soil(false) {}
    void start() { digitalWrite(_pin, HIGH); }
    void stop()  { digitalWrite(_pin, LOW);  }
    bool on_soil;
private:
    uint8_t _pin;
};


class MoistureSensor {
public:
    MoistureSensor(uint8_t pin): _pin(pin), moisture(0), is_dry(false) {}
    void read(int min_threshold, int normal_threshold);
    int moisture;
    bool is_dry;
private:
    uint8_t _pin;
};


class LightSensor {
public:
    LightSensor(uint8_t pin): _pin(pin), light(0), is_dark(false) {}
    void read(int min_threshold);
    int light;
    bool is_dark;
private:
    uint8_t _pin;
};


class Datetime {
public:
    Datetime(): hour(0) {}
    void read();  
    int hour;
};


float Thermometer::read() {
    _dht.begin();
    float t = _dht.readTemperature();
    return isnan(t) ? 0.0 : t;
}

void MoistureSensor::read(int min_threshold, int normal_threshold) {
    int raw = analogRead(_pin);
    moisture = map(raw, 1023, 0, 0, 100);
    is_dry = (moisture < min_threshold);
    // считать "влажной" только когда выше нормального порога
    if (moisture >= normal_threshold) is_dry = false;
}

void LightSensor::read(int min_threshold) {
    int raw = analogRead(_pin);
    light = map(raw, 1023, 0, 0, 100);
    is_dark = (light < min_threshold);
}

void Datetime::read() {
    hour = (millis() / 3600000UL) % 24;
}

void Climate::printStatus() {
    Serial.println("=== Статус среды ===");
    Serial.print("Темп min/max: "); Serial.print(temperature_min); Serial.print("/"); Serial.println(temperature_max);
    Serial.print("Влажность почвы min/max: "); Serial.print(humidity_dirt_min); Serial.print("/"); Serial.println(humidity_dirt_max);
    Serial.print("Освещённость min: "); Serial.println(light_min);
}

// Выбор медианы из трёх показаний датчиков температуры
float filteredTemperature(Thermometer &t1, Thermometer &t2, Thermometer &t3) {
    float a = t1.read();
    float b = t2.read();
    float c = t3.read();
    if (a > b) { float tmp=a; a=b; b=tmp; }
    if (b > c) { float tmp=b; b=c; c=tmp; }
    if (a > b) { float tmp=a; a=b; b=tmp; }
    return b;
}


void control_temperature(Climate &cl, float temp, Heater &h, Ventilator &v) {
    if (temp < cl.temperature_min) {
        h.on_temp = true;
        v.on_temp = false;
    } else if (temp > cl.temperature_max) {
        h.on_temp = false;
        v.on_temp = true;
    } else {
        h.on_temp = false;
        v.on_temp = false;
    }
}


void control_ventilation(Datetime &dt, Ventilator &v) {
    if (dt.hour < 6 || dt.hour > 23) {
        v.on_schedule = false;
    } else {
        v.on_schedule = (dt.hour % 4 == 0);
    }
}


void control_moisture(Climate &cl, MoistureSensor &ms, Pump &p) {
    p.on_soil = ms.is_dry;
}


void control_light(LightSensor &ls, uint8_t lightPin) {
    digitalWrite(lightPin, ls.is_dark ? HIGH : LOW);
}


void do_heat(Heater &h) {
    if (h.on_temp) h.start(); else h.stop();
}


void do_ventilation(Ventilator &v) {
    if (v.on_temp || v.on_schedule) v.start(); else v.stop();
}


void do_pump(Pump &p) {
    if (p.on_soil) p.start(); else p.stop();
}

// Init
Climate climate;
Thermometer t1(PIN_DHT1), t2(PIN_DHT2), t3(PIN_DHT3);
Heater heater(PIN_HEATER);
Ventilator ventilator(PIN_FAN);
MoistureSensor moistureSensor(PIN_MOISTURE);
Pump pump(PIN_PUMP);
LightSensor lightSensor(PIN_LIGHT_SENS);
Datetime datetime;


void setup() {
    Serial.begin(9600);
    pinMode(PIN_HEATER, OUTPUT);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_LIGHT, OUTPUT);

    // Настройки
    climate.temperature_min   = 20;
    climate.temperature_max   = 30;
    climate.humidity_dirt_min = 40;
    climate.humidity_dirt_max = 60;
    climate.light_min         = 30;
}

void loop() {
    // Считываем время
    datetime.read();

    // Считываем датчики
    float temp = filteredTemperature(t1, t2, t3);
    moistureSensor.read(climate.humidity_dirt_min, climate.humidity_dirt_max);
    lightSensor.read(climate.light_min);

    // Управление
    control_temperature(climate, temp, heater, ventilator);
    control_ventilation(datetime, ventilator);
    control_moisture(climate, moistureSensor, pump);
    control_light(lightSensor, PIN_LIGHT);

    // Не позволяем одновременно включаться обогревателю и помпе
    if (heater.on_temp && pump.on_soil) {
        pump.on_soil = false;
    }

    do_heat(heater);
    do_ventilation(ventilator);
    do_pump(pump);

    Serial.print("Температура (медиана): "); Serial.println(temp);
    Serial.print("Влажность почвы: "); Serial.println(moistureSensor.moisture);
    climate.printStatus();

    delay(2000);
}
