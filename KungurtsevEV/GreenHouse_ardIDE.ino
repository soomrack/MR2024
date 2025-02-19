#include <DHT.h>

#define PIN_LUX_SENSOR A3
#define LED_STRIP 6
#define HUMIDITY_SENSOR A1
#define WATER_PUMP 5
#define ATAD_SENSOR 12
#define HEAT_VENT 4
#define VENTILATION 7
#define WATER_TIME 3

DHT dht(ATAD_SENSOR, DHT11);

class Climate {
public:
    int min_light;
    double min_temp;
    double max_temp;
    double min_humidity;
    double max_humidity;
    int min_soil_humidity;
    int max_soil_humidity;
public:
    void set_for_basil();
};


void Climate::set_for_basil() {
    min_light = 500;
    min_temp = 15;
    max_temp = 27;
    min_soil_humidity = 20;
    max_soil_humidity = 120;
    min_humidity = 40;
    max_humidity = 60;
}


class Sensors {
public:
    int light;
    double temperature;
    double humidity;
    int soil_humidity;
public:
    void read_sensors();
};


void Sensors::read_sensors() {
    dht.read();
    soil_humidity = analogRead(HUMIDITY_SENSOR);
    light = analogRead(PIN_LUX_SENSOR);
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
}


class SystemState {
public:
    bool vent_temp;
    bool light;
    bool vent;
    bool pump;
    bool heat;
    bool day;
    int water_time;

    SystemState();
    void reset_state();
};


SystemState::SystemState() {
    reset_state();
}


void SystemState::reset_state() {
    vent_temp = false;
    light = false;
    vent = false;
    pump = false;
    heat = false;
    day = false;
    water_time = 0;
}


class Time {
public:
    int days;
    long hours;
    int minutes;
    int seconds;

    void update_time();
};


void Time::update_time() {
    unsigned long ms = millis();
    days = ms / (1000 * 60 * 60 * 24);
    hours = (ms / (1000 * 60 * 60)) % 24;
    minutes = (ms / (1000 * 60)) % 60;
    seconds = (ms / 1000) % 60;
}


class GreenhouseController {
private:
    Climate clim;
    Sensors sens;
    SystemState state;
    Time time;
public:
    GreenhouseController();
    void setup_pins();
    void check_ventilation();
    void check_air_temperature();
    void check_air_humidity();
    void check_ground_humidity();
    void check_light();
    void control_light();
    void control_heat();
    void control_vent();
    void control_pump();
    void periodic_check();
};


GreenhouseController::GreenhouseController() {
    setup_pins();
    clim.set_for_basil();
}


void GreenhouseController::setup_pins() {
    pinMode(LED_STRIP, OUTPUT);
    pinMode(WATER_PUMP, OUTPUT);
    pinMode(HEAT_VENT, OUTPUT);
    pinMode(VENTILATION, OUTPUT);
    dht.begin();
}


void GreenhouseController::check_ventilation() {
    if ((time.days % 4 == 0) && (time.hours == 13)) {
        state.vent = true;
    }
}


void GreenhouseController::check_air_temperature() {
    if (sens.temperature >= clim.min_temp && sens.temperature <= clim.max_temp) {
        state.vent = true;
        state.heat = true;
    } else if (sens.temperature < clim.min_temp) {
        state.vent = false;
        state.heat = false;
    } else {
        state.vent = true;
        state.heat = false;
    }
}


void GreenhouseController::check_air_humidity() {
    if (sens.humidity >= clim.min_humidity && sens.humidity <= clim.max_humidity) {
        state.vent = true;
        state.pump = false;
    } else if (sens.humidity < clim.min_humidity) {
        state.vent = false;
        state.pump = true;
    } else {
        state.vent = true;
        state.pump = false;
    }
}


void GreenhouseController::check_ground_humidity() {
    state.pump = (sens.soil_humidity < clim.min_soil_humidity);
}


void GreenhouseController::check_light() {
    state.light = (sens.light > clim.min_light);
}


void GreenhouseController::control_light() {
    digitalWrite(LED_STRIP, state.light ? HIGH : LOW);
}


void GreenhouseController::control_heat() {
    digitalWrite(HEAT_VENT, state.heat ? HIGH : LOW);
    digitalWrite(VENTILATION, state.heat ? HIGH : LOW);
}


void GreenhouseController::control_vent() {
    digitalWrite(VENTILATION, state.vent ? HIGH : LOW);
}


void GreenhouseController::control_pump() {
    if (state.pump) {
        unsigned long start = millis();
        while (millis() - start < WATER_TIME * 1000) {
            digitalWrite(WATER_PUMP, HIGH); //////
            digitalWrite(HEAT_VENT, LOW);
        }
    }
    digitalWrite(WATER_PUMP, LOW);
}


void GreenhouseController::periodic_check() {
    time.update_time();
    if (time.seconds % 5 == 0) {
        //sensors
        sens.read_sensors();
        //control
        check_ventilation();
        check_air_temperature();
        check_air_humidity();
        check_ground_watering();
        check_light();

        //act
        control_light(); //turn
        control_heat();
        control_vent();
        control_pump();
        
        //log
        Serial.println(sens.light);
    }
}

GreenhouseController controller;


void setup() {
    Serial.begin(9600);
}


void loop() {
  controller.periodic_check();
//   digitalWrite(HEAT_VENT, HIGH);
//   digitalWrite(VENTILATION, HIGH );
//   float l = analogRead(LUX_SENSOR);
//float hum = analogRead(HUMIDITY_SENSOR);
 float t = dht.readTemperature();
  Serial.print("temp:");
  Serial.println(t);
//   Serial.print("lux:");
//   Serial.println(l);
//   Serial.print("hum:");
//   Serial.println(hum);
//   delay(500);
}