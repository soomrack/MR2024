#include <DHT.h>

#define PIN_LUX_SENSOR 9 //фоторез
#define PIN_LED_STRIP 6 // лед лента
#define PIN_HUMIDITY_SENSOR A1 //влажность почвы
#define PIN_WATER_PUMP 5 // помпа
#define PIN_ATAD_SENSOR 12 // темп и влаж возд
#define PIN_HEAT_VENT 4 // нагрев дулки
#define PIN_VENTILATION 7 // дулка
const int WATER_TIME = 1;

DHT dht(PIN_ATAD_SENSOR, DHT11);

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
};


class SystemState {
public:
    bool vent_temp;
    bool light;
    bool vent;
    bool pump;
    bool heat;
    bool day;
    bool vent_flag;
    bool heat_flag;
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
    vent_flag = false;
    heat_flag = false;
    water_time = 0;
}


class Time {
public:
    int days;
    long hours;
    int minutes;
    int seconds;
    unsigned long long int ms;
    void update_time();
};


void Time::update_time() {
    unsigned long long int ms = millis();
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
    //void check_air_temperature();
    //void check_air_humidity();
    void check_ventilation();
    void check_heat();
    void check_ground_watering();
    void check_light();
    void turn_regulators();
    void turn_light();
    void turn_heat();
    void turn_vent();
    void turn_pump();
    void periodic_check();
};


GreenhouseController::GreenhouseController() {
    setup_pins();
    clim.set_for_basil();
}


void GreenhouseController::setup_pins() {
    pinMode(PIN_LED_STRIP, OUTPUT);
    pinMode(PIN_WATER_PUMP, OUTPUT);
    pinMode(PIN_HEAT_VENT, OUTPUT);
    pinMode(PIN_VENTILATION, OUTPUT);
    dht.begin();
}

void GreenhouseController::check_ventilation() {
    if ((time.days % 4 == 0) && (time.hours == 13)) {
      state.vent_flag = true;
    }
    if (sens.temperature > clim.max_temp) {
      state.vent_flag = true;
    }
    if (sens.humidity > clim.min_humidity) {
      state.vent_flag = true;
    }
}

void GreenhouseController::check_heat() {
  if (sens.temperature < clim.min_temp) {
    state.heat_flag = true;
  }
}

void GreenhouseController::check_ground_watering() {
  state.pump = (sens.soil_humidity < clim.min_soil_humidity);
}


void GreenhouseController::check_light() {
  state.light = (sens.light > clim.min_light);
}


void GreenhouseController::turn_regulators() {
    if (state.vent_flag) {
      state.vent = true;
    }
    else {
      state.vent = false;
    }

    if (state.heat_flag) {
      state.heat = true;
    }
    else {
      state.heat = false;
    }
}


// void GreenhouseController::check_air_temperature() {
//     if (sens.temperature >= clim.min_temp && sens.temperature <= clim.max_temp) {
//         state.vent = false;
//         state.heat = false;
//     } else if (sens.temperature < clim.min_temp) {
//         state.vent = false;
//         state.heat = true;
//     } else { 
//         state.vent = true;
//         state.heat = false;
//     }
// }


// void GreenhouseController::check_air_humidity() {
//     if (sens.humidity >= clim.min_humidity && sens.humidity <= clim.max_humidity) {
//         state.vent = false;
//         state.pump = false;
//     } else if (sens.humidity < clim.min_humidity) {
//         state.vent = false;
//         state.pump = true;
//     } else {
//         state.vent = true;
//         state.pump = false;
//     }
// }


void GreenhouseController::turn_light() {
  digitalWrite(PIN_LED_STRIP, state.light ? HIGH : LOW);
}


void GreenhouseController::turn_heat() {
  digitalWrite(PIN_HEAT_VENT, state.heat ? HIGH : LOW);
  digitalWrite(PIN_VENTILATION, state.heat ? HIGH : LOW);
}


void GreenhouseController::turn_vent() {
  digitalWrite(PIN_VENTILATION, state.vent ? HIGH : LOW);
}


void GreenhouseController::turn_pump() {
  unsigned long pump_start_time = 0; 
  bool pump_active = false; 

  if (state.pump) {
      if (!pump_active) { 
          pump_start_time = millis(); 
          pump_active = true;
          digitalWrite(PIN_WATER_PUMP, HIGH); 
          digitalWrite(PIN_HEAT_VENT, LOW); 
      } else if (millis() - pump_start_time >= WATER_TIME * 100) { 
          digitalWrite(PIN_WATER_PUMP, LOW); 
          pump_active = false; 
      }
  } else {
      if (pump_active) { 
          digitalWrite(PIN_WATER_PUMP, LOW);
      }
  }
}


void GreenhouseController::periodic_check() {
  time.update_time();
  if (time.seconds % 5 == 0) {
    //sensors
    sens.soil_humidity = analogRead(PIN_HUMIDITY_SENSOR);
    sens.light = analogRead(PIN_LUX_SENSOR);
    sens.temperature = dht.readTemperature();
    sens.humidity = dht.readHumidity();

    //control
    check_ventilation();
    check_ventilation();
    check_heat();
    check_ground_watering();
    check_light();

    //actuator
    turn_light();
    turn_heat();
    turn_vent();
    turn_pump();
    
    //log
    Serial.println(sens.light);
    Serial.println(sens.temperature);
    Serial.println(sens.humidity);
    Serial.println(sens.soil_humidity); 
  }
}
GreenhouseController controller;


void setup() {
  Serial.begin(9600);
}


void loop() {
    controller.periodic_check();
}
