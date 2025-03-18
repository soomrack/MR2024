#include <DHT11.h>


#define PIN_LIGHT_SENSOR 9
#define PIN_SOIL_HUMIDITY_SENSOR A1
#define PIN_DHT_SENSOR 12


#define PIN_BACKLIGHT 6
#define PIN_PUMP 5
#define PIN_HEATER 4
#define PIN_VENTILATION 7


#define TIME_DAY_START 7 // hours
#define TIME_DAY_END 23 // hours
#define TARGET_SOIL_HUMIDITY_LEVEL 40 // %
#define TARGET_TEPM_LEVEL 26 // °C
#define TARGET_AIR_HUMIDITY_LEVEL 30 // %


static bool heater_is_active = false;
static bool pomp_is_active = false;
static bool ventilation_is_need_for_heater = false;
static bool is_time_to_plan_air_out = false;


DHT11 dht(PIN_DHT_SENSOR);


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
    Serial.begin(115200);
}


inline void wait(const uint16_t duration) {
    unsigned long current_time = millis();
    while(millis() - current_time < duration) {}
}


uint16_t get_current_time() { // In minutes
    uint16_t minutes = (millis() / 1000) / (60);
    uint16_t count_minutes_in_day = 24 * 60;

    while(minutes >= count_minutes_in_day) { minutes -= count_minutes_in_day; }

    return minutes;
}


void control_backlight() {
    uint16_t current_time = get_current_time();
    uint8_t current_lighting_level = digitalRead(PIN_LIGHT_SENSOR);

    if(current_time >= (TIME_DAY_START * 60) && current_time <= (TIME_DAY_END * 60) || current_lighting_level == 1) {
            digitalWrite(PIN_BACKLIGHT, HIGH);
    } else {
        digitalWrite(PIN_BACKLIGHT, LOW);
    }
}


void control_pomp(const uint8_t min_deviation = 5, const uint16_t deadtime = 1000) {
    static unsigned long int last_pump_toggle_time = millis();
    if(millis() - last_pump_toggle_time <= deadtime) return;

    uint8_t max_soil_humidity = TARGET_SOIL_HUMIDITY_LEVEL + min_deviation; 
    uint8_t min_soil_humidity = TARGET_SOIL_HUMIDITY_LEVEL - min_deviation;
    uint8_t current_soil_humidity = map(analogRead(PIN_SOIL_HUMIDITY_SENSOR), 0, 1024, 0, 100);

    if(current_soil_humidity < min_soil_humidity) {
        if(heater_is_active) return;
        
        digitalWrite(PIN_PUMP, HIGH);
        pomp_is_active = true;
        
        last_pump_toggle_time = millis();
    } else if (current_soil_humidity > max_soil_humidity) {
        digitalWrite(PIN_PUMP, LOW);
        pomp_is_active = false;
        
        last_pump_toggle_time = millis();
    }
}


void control_heater(const uint8_t min_deviation = 2, const uint16_t deadtime = 1000) {
    static unsigned long int last_heater_toggle_time = millis();
    if(millis() - last_heater_toggle_time <= deadtime) return;

    int max_temp = TARGET_TEPM_LEVEL + min_deviation; 
    int min_temp = TARGET_TEPM_LEVEL - min_deviation;
    int current_temp = dht.readTemperature();
    
    if(current_temp < min_temp) {
        if(pomp_is_active) return;
        
        digitalWrite(PIN_HEATER, HIGH);
        heater_is_active = true;
        
        digitalWrite(PIN_VENTILATION, HIGH);
        ventilation_is_need_for_heater = true;
        
        last_heater_toggle_time = millis();
    } else if(current_temp > max_temp) {
        digitalWrite(PIN_HEATER, LOW);
        heater_is_active = false;
        
        digitalWrite(PIN_VENTILATION, HIGH);
        ventilation_is_need_for_heater = true;
        
        last_heater_toggle_time = millis();
    } else {
        ventilation_is_need_for_heater = false; 
    }
}


void control_ventilation(const uint8_t min_deviation = 5) {
    uint8_t max_air_humidity = TARGET_AIR_HUMIDITY_LEVEL + min_deviation; 
    uint8_t min_air_humidity = TARGET_AIR_HUMIDITY_LEVEL - min_deviation;
    uint8_t current_hum = dht.readHumidity();
 if (!ventilation_is_need_for_heater && !is_time_to_plan_air_out) {
        if (current_hum > max_air_humidity) {
            digitalWrite(PIN_VENTILATION, HIGH);
        } else if (current_hum < min_air_humidity) {
            digitalWrite(PIN_VENTILATION, LOW);
        }
    }
}


template<typename T>
void inline swap(T& value1, T&value2) {
    T tmp = value1;
    value1 = value2;
    value2 = tmp;
}


void plan_air_out(const uint8_t start_hour, const uint16_t start_minut, const uint8_t end_hour, const uint16_t end_minut) {    
    uint16_t start_time = min(23, start_hour) * 60 + min(59, start_minut);
    uint16_t end_time = min(23, end_hour) * 60 + min(59, end_minut);
    
    if(start_time > end_time) swap(start_time, end_time);

    uint16_t current_time = get_current_time();

    if(current_time >= start_time && current_time <= end_time) {
        is_time_to_plan_air_out = true;
        digitalWrite(PIN_VENTILATION, HIGH);
    } else {
        is_time_to_plan_air_out = false;
    }
}


void print_current_environment_params() {
    uint16_t current_time = get_current_time();
    Serial.print("Status is active, current time: ");
    Serial.print(current_time / 24);
    Serial.print(" hour ");
    Serial.print(current_time % 24);
    Serial.println(" minut");

    Serial.print("Current lighting: ");
    if(digitalRead(PIN_LIGHT_SENSOR)) {
      Serial.println("low");
    } else {
      Serial.println("high");
    }

    Serial.print("Current soil humidity: ");
    Serial.println(map(analogRead(PIN_PUMP), 0, 1024, 0, 100));

    Serial.print("Current temperature: ");
    Serial.println(dht.readTemperature());

    Serial.print("Current air humidity: ");
    Serial.println(dht.readHumidity());

    Serial.println(); // For better formating
}


void loop() {
    control_backlight();
    control_pomp();
    control_heater();
    control_ventilation();
    plan_air_out(15, 30, 16, 0);

    print_current_environment_params();
    
    wait(100);
}
