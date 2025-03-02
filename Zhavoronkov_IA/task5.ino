// Sensors:
#define EXTERNAL_LIGHT_PIN A0
#define SOIL_HUMI_PIN A1
#define AIR_HUMI_PIN 12
#define AIR_TEMP_PIN 12  // Real check to understand

// Actuators:
#define WATER_PUMP_PIN 5
#define LED_LIGHT_PIN 6
#define FAN_PIN 7
#define HEAT_FAN_PIN 4

// Desired values
#define DESIR_TEMP 23  // Temperature in Celsius degrees
#define DESIR_SOIL_HUMI 23
#define DESIR_AIR_HUMI 23
#define DESIR_LIGHT 23

typedef unsigned long long int time;

unsigned int ext_light, soil_humi, air_humi, air_temp;
bool is_heating, is_airing;

void pin_init()
{
  pinMode(EXTERNAL_LIGHT_PIN, INPUT);
  pinMode(SOIL_HUMI_PIN, INPUT);
  pinMode(AIR_HUMI_PIN, INPUT);
  pinMode(AIR_TEMP_PIN, INPUT);

  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_FAN_PIN, OUTPUT);
}


void on(int PIN)
{
  digitalWrite(PIN, 1);
}


void off(int PIN)
{
  digitalWrite(PIN, 0);
}

void get_info()
{
  ext_light = analogRead(EXTERNAL_LIGHT_PIN);
  soil_humi = analogRead(SOIL_HUMI_PIN);
  //air_humi = ;
  //air_temp = ;
}

void lighting_check()  //  Проверка освещенности
{
  static time t_light;

  if ((ext_light <= DESIR_LIGHT) && (digitalRead(LED_LIGHT_PIN) == 0)) {  // Вкл если освещения мало и лампа не работает
    on(LED_LIGHT_PIN);
    t_light = millis();
  }

  if ((millis() - t_light >= 2 * 60* 1000) && (ext_light > DESIR_LIGHT)) { // Выкл если освещения достаточно и лампа поработала >= 2 мин
    off(LED_LIGHT_PIN);
  }
}


void watering_check()
{
  static time p_time;

  if ((soil_humi <= DESIR_SOIL_HUMI) && (digitalRead(WATER_PUMP_PIN) == 0)) { // Вкл если малая влажность и помпа не работала
    on(WATER_PUMP_PIN);
    p_time = millis();
  }

  if ((millis() - p_time >= 1 * 60 * 1000) && (soil_humi > DESIR_SOIL_HUMI)) { // Выкл если влажность достаточная и помпа проработала >= 1 мин
    off(WATER_PUMP_PIN);
  }
}


void airing_check()
{
  if (air_humi >= DESIR_AIR_HUMI) {
    is_airing = 1;
  } else {
    is_airing = 0;
  }
}


void heating_check()
{
  if (air_temp <= DESIR_TEMP) {
    is_heating = 1;
  } else {
    is_heating = 0;
  }
}


void fan_check()
{
  static time t_fan;

  airing_check();
  heating_check();

  if ((is_heating == 1) || (is_airing == 1)) { // Вкл вент если нужен подогрев или проветривание
    if (is_heating == 1) {
      on(HEAT_FAN_PIN);
    }

    if (digitalRead(FAN_PIN) == 0) {
      on(FAN_PIN);
      t_fan = millis();
    }
  }

  if ((millis() - t_fan >= 5 * 60 * 1000) && (is_airing == 0) && (is_heating == 0)) {  // Выкл если проработало больше 5 минут и показатели в норме
    off(FAN_PIN);
    off(HEAT_FAN_PIN);
  }
}


void setup() {
  pin_init();
}

void loop() {
  get_info();
  
  lighting_check();
  watering_check();
  fan_check();
}
