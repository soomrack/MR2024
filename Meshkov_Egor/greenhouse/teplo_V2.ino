#include <Bonezegei_DHT11.h>


#define PIN_LIGHT_SENSOR A0
#define PIN_SOIL_HUMIDITY_SENSOR A1
#define PIN_DHT_SENSOR 12


#define PIN_BACKLIGHT 6
#define PIN_PUMP 5
#define PIN_HEATER 4
#define PIN_VENTILATION 7


#define TIME_DAY_START 7 // hours
#define TIME_DAY_END 23 // hours
#define TARGET_LIGHTING_LEVEL 40 // %
#define TARGET_SOIL_HUMIDITY_LEVEL 30 // %
#define TARGET_TEPM_LEVEL 35 // °C
#define TARGET_AIR_HUMIDITY_LEVEL 30 // %


static bool heater_is_active = false;
static bool priority = false;   //if 0 pomp 1 hetar 0 if 1 pomp 0 haeter 1
static bool pomp_is_active = false;
static bool ventilation_is_need_for_heater = false;


Bonezegei_DHT11 dht(PIN_DHT_SENSOR);


void pins_init() {
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    pinMode(PIN_SOIL_HUMIDITY_SENSOR, INPUT);

    pinMode(PIN_BACKLIGHT, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_HEATER, OUTPUT);
    pinMode(PIN_VENTILATION, OUTPUT);
}


void setup() {
    pins_init();
}


void wait(const uint16_t duration) {
    volatile unsigned long current_time = millis();
    while(millis() - current_time < duration) {}
}


uint16_t get_current_time() {
    uint16_t count_day = 0;
    uint16_t minutes = millis() / (60 * 1000) - count_day * 24 * 60;
    uint16_t one_day = 24 * 60;

    while(minutes >= one_day) { count_day++; }

    return minutes;
}


void control_backlight(const uint8_t min_deviation = 5) {
    uint16_t current_time = get_current_time();
    uint8_t max_lighting_level = TARGET_LIGHTING_LEVEL + min_deviation;
    uint8_t min_lighting_level = TARGET_LIGHTING_LEVEL - min_deviation;
    uint8_t current_lighting_level = analogRead(PIN_LIGHT_SENSOR);

    if(current_time >= (TIME_DAY_START * 60) && current_time <= (TIME_DAY_END * 60)) {
        if(current_lighting_level < min_lighting_level) {
            digitalWrite(PIN_PUMP, HIGH);
        } else if(current_lighting_level > max_lighting_level) {
            digitalWrite(PIN_PUMP, LOW);
        }
    } else {
        digitalWrite(PIN_BACKLIGHT, LOW);
    }
}


void control_pomp(const uint8_t min_deviation = 5, const uint16_t deadtime = 1000) {
    uint8_t max_soil_humidity = TARGET_SOIL_HUMIDITY_LEVEL + min_deviation; 
    uint8_t min_soil_humidity = TARGET_SOIL_HUMIDITY_LEVEL - min_deviation;
    uint8_t current_soil_humidity = analogRead(PIN_SOIL_HUMIDITY_SENSOR);

    if(current_soil_humidity < min_soil_humidity) {
        if(heater_is_active){ 
            if(priority == false){
                digitalWrite(PIN_HEATER, LOW);
                digitalWrite(PIN_PUMP, HIGH);
                priority = true;
                wait(4500);
                return;
            }
            else return; 
        }
        digitalWrite(PIN_PUMP, HIGH);
        pomp_is_active = true;
        wait(deadtime);
    } else if (current_soil_humidity > max_soil_humidity) {
        digitalWrite(PIN_PUMP, LOW);
        pomp_is_active = false;
        wait(deadtime);
    }
}


void control_heater(const uint8_t min_deviation = 5, const uint16_t deadtime = 1000) {
    int max_temp = TARGET_TEPM_LEVEL + min_deviation; 
    int min_temp = TARGET_TEPM_LEVEL - min_deviation;
    int current_temp = dht.getTemperature();
    
    if(current_temp < min_temp) {
        if(pomp_is_active){
            if(priority == true){
                digitalWrite(PIN_PUMP, LOW);
                digitalWrite(PIN_HEATER, HIGH);
                priority = false;
                wait(4500);
                return;
            }
            else return; 
        }
        
        digitalWrite(PIN_HEATER, HIGH);
        heater_is_active = true;
        
        digitalWrite(PIN_VENTILATION, HIGH);
        ventilation_is_need_for_heater = true;
        
        wait(deadtime);
    } else if(current_temp > max_temp) {
        digitalWrite(PIN_HEATER, LOW);
        heater_is_active = false;
        
        digitalWrite(PIN_VENTILATION, HIGH);
        ventilation_is_need_for_heater = true;
        
        wait(deadtime);
    } else {
        ventilation_is_need_for_heater = false; 
    }
}


void control_ventilation(const uint8_t min_deviation = 5, const uint16_t deadtime = 1000) {
    uint8_t max_air_humidity = TARGET_AIR_HUMIDITY_LEVEL + min_deviation; 
    uint8_t min_air_humidity = TARGET_AIR_HUMIDITY_LEVEL - min_deviation;
    uint8_t current_hum = dht.getHumidity();

    if (!ventilation_is_need_for_heater) {
        if (current_hum > max_air_humidity) {
            digitalWrite(PIN_VENTILATION, HIGH);
            wait(deadtime);
        } else if (current_hum < min_air_humidity) {
            digitalWrite(PIN_VENTILATION, LOW);
            wait(deadtime);
        }
    }
}


template<typename T>
void inline swap(T& value1, T&value2) {
    T tmp = value1;
    value1 = value2;
    value2 = tmp;
}


void plan_aer_out(const uint8_t start_hour, const uint16_t start_minut, const uint8_t end_hour, const uint16_t end_minut) {    
    uint16_t start_time = min(start_hour * 60 + start_minut, 23 * 60 + 50);
    uint16_t end_time = min(end_hour * 60 + end_minut, 24 * 60 - 1);
    
    if(start_time > end_time) { swap(start_time, end_time); }

    uint16_t current_time = get_current_time();

    if(current_time >= start_time && current_time <= end_time) {
        digitalWrite(PIN_VENTILATION, HIGH);
        wait(end_time - start_time);
        digitalWrite(PIN_VENTILATION, LOW);
    }
}


void loop() {
    control_backlight();
    control_pomp();
    control_heater();
    control_ventilation();
    plan_aer_out(15, 30, 16, 40);
    
    delay(100);
}
