#include <TimeAlarms.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;


#define PIN_SEN_LIGHT A0
#define PIN_SEN_WET_EARTH A1
#define PIN_SEN_WET_EARTH_2 A2
#define PIN_SEN_WET_EARTH_3 A3
#define PIN_SEN_TEMP_WET 12
#define PIN_DIR_HEAT 4
#define PIN_DIR_WATER_PUMP 5
#define PIN_DIR_LIGHT 6
#define PIN_DIR_COOLER 7

static unsigned long start_time_minutes;
static unsigned long last_time_air_temp_humidity = 0;
static unsigned long last_time_illumination = 0;
static unsigned long last_time_soil_humidity = 0;
unsigned long current_time = 0;
unsigned long interval_read_sensor = 500;
unsigned int soil_humidity1; 

struct Climate // Структура для заранее заданных характеристик
{
  int MIN_ILLUMINATION;
  int MIN_SOIL_HUMIDITY;
  int MIN_AIR_HUMIDITY;
  int MAX_AIR_HUMIDITY;
  int MIN_TEMP_AIR;
  int MAX_TEMP_AIR;

  int TIME_SUNRISE_MINUTES;
  int TIME_SUNSET_MINUTES;

  int TIME_WATERING_SECONDS;
  int TIME_WAITING_DATA_WATERING_SECONDS;
  int TIME_VENTILATION_SECONDS;

};

typedef struct Climate Climate;
Climate Greenhouse;

struct Status_flag //Флаги активности приборов
{
  bool WATER_PUMP = 0;
  bool LIGHT = 0;
  bool HEAT = 0;
  bool VENTILATION = 0;
};

typedef struct Status_flag Status_flag;
Status_flag Greenhouse_status_flag;

struct Sensor 
{
  int air_humidity;
  int air_temperature;
  int illumination;
  int soil_humidity1;
};

typedef struct Sensor Sensor;
Sensor Greenhouse_sensor;


void initialize_climate_parametrs()
{
  Greenhouse.MIN_ILLUMINATION = 800;
  Greenhouse.MIN_SOIL_HUMIDITY = 900;
  Greenhouse.MIN_AIR_HUMIDITY = 30;
  Greenhouse.MAX_AIR_HUMIDITY = 70;
  Greenhouse.MIN_TEMP_AIR = 20;
  Greenhouse.MAX_TEMP_AIR = 25;

  Greenhouse.TIME_SUNRISE_MINUTES = 420;
  Greenhouse.TIME_SUNSET_MINUTES = 1200;

  Greenhouse.TIME_WATERING_SECONDS = 4;
  Greenhouse.TIME_WAITING_DATA_WATERING_SECONDS = 1;
  Greenhouse.TIME_VENTILATION_SECONDS = 2;
}


void set_time()
{
  start_time_minutes = (hour() * 60) + minute();

  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
}


void illumination_read() // Чтение данных с фоторезистора
{
  if ((current_time - last_time_illumination) > interval_read_sensor) {
    Greenhouse_sensor.illumination = analogRead(PIN_SEN_LIGHT);
    
    last_time_illumination = current_time;
  }
}


void control_light() 
{
  unsigned long current_time = ((millis() / 1000 * 60) + start_time_minutes) % (24 * 60);

  if ((current_time > Greenhouse.TIME_SUNRISE_MINUTES) && (current_time < Greenhouse.TIME_SUNSET_MINUTES)) {
     // Чем ниже напряжение на датчике тем больше освещённость. При слишком низкой освещённости включаем лампы
    if ((Greenhouse_sensor.illumination > Greenhouse.MIN_ILLUMINATION) && (Greenhouse_status_flag.WATER_PUMP == 0)) {

      digitalWrite(PIN_DIR_LIGHT, 1);

      Greenhouse_status_flag.LIGHT = 1;
    }
    else{

      digitalWrite(PIN_DIR_LIGHT, 0);

      Greenhouse_status_flag.LIGHT = 0;
    }
  }
}


void air_temp_humidity_read() //Чтение данных с DHT
{
  if((current_time - last_time_air_temp_humidity) > interval_read_sensor) {
    DHT.read(PIN_SEN_TEMP_WET);

    Greenhouse_sensor.air_humidity = DHT.humidity;
    Greenhouse_sensor.air_temperature = DHT.temperature;

    last_time_air_temp_humidity = current_time;
  }
}


void soil_humidity_read()
{ 
  if ((current_time - last_time_soil_humidity) > interval_read_sensor) {
    
    Greenhouse_sensor.soil_humidity1 = analogRead(PIN_SEN_WET_EARTH);

    last_time_soil_humidity = current_time;
  }
}


void control_temperature()
{
  static unsigned long temperature_timer;
  static bool control_temperature_flag = 0;

  if ((Greenhouse_sensor.air_temperature < Greenhouse.MIN_TEMP_AIR) && (Greenhouse_status_flag.HEAT == 0) && (Greenhouse_status_flag.VENTILATION == 0)) {

    digitalWrite(PIN_DIR_COOLER, 1);
    digitalWrite(PIN_DIR_HEAT, 1);

    Greenhouse_status_flag.HEAT = 1;
    Greenhouse_status_flag.VENTILATION = 1;

    control_temperature_flag = 1;
    temperature_timer = millis();
  }

  if ((millis() - temperature_timer) > Greenhouse.TIME_VENTILATION_SECONDS*1000 && control_temperature_flag == 1) {

    digitalWrite(PIN_DIR_COOLER, 0);
    digitalWrite(PIN_DIR_HEAT, 0);

    Greenhouse_status_flag.HEAT = 0;
    Greenhouse_status_flag.VENTILATION = 0;

    control_temperature_flag = 0;
  }
}


void control_ventilation()
{
  static unsigned long ventilation_timer;
  static bool control_ventilation_flag = 0;
  if(((Greenhouse_sensor.air_temperature > Greenhouse.MAX_TEMP_AIR) || (Greenhouse_sensor.air_humidity > Greenhouse.MIN_AIR_HUMIDITY)) && (Greenhouse_status_flag.VENTILATION == 0)) {
    digitalWrite(PIN_DIR_COOLER, 1);

    Greenhouse_status_flag.VENTILATION = 1;

    control_ventilation_flag = 1;
    ventilation_timer = millis();
  }

  if((millis() - ventilation_timer) > Greenhouse.TIME_VENTILATION_SECONDS*1000 && control_ventilation_flag == 1) {

    digitalWrite(PIN_DIR_COOLER, 0);
    Greenhouse_status_flag.VENTILATION = 0;
    control_ventilation_flag = 0;
  }
}


void control_watering()
{
    static unsigned long watering_timer;
    static unsigned long timer_waiting;
    static bool flag_waiting = 0;

    if ((Greenhouse_sensor.soil_humidity1 > Greenhouse.MIN_SOIL_HUMIDITY) && (Greenhouse_status_flag.WATER_PUMP == 0) && (flag_waiting = 0)) {
        if (Greenhouse_status_flag.HEAT == 1) {
            digitalWrite(PIN_DIR_HEAT, 0);    
            Greenhouse_status_flag.HEAT = 0;   
        }

        digitalWrite(PIN_DIR_WATER_PUMP, 1);
        Greenhouse_status_flag.WATER_PUMP = 1;

        watering_timer = millis();

        if ((millis() - watering_timer) > Greenhouse.TIME_WATERING_SECONDS * 1000) {
            digitalWrite(PIN_DIR_WATER_PUMP, 0);
            Greenhouse_status_flag.WATER_PUMP = 0;

            flag_waiting = 1;
            timer_waiting = millis();
        }

        if (millis() - timer_waiting == Greenhouse.TIME_WAITING_DATA_WATERING_SECONDS * 1000) flag_waiting = 0;
    }   
}

void print_sensor_read()
{
    Serial.print("Освещённость");
    Serial.println(Greenhouse_sensor.illumination); // Чем выше значение, тем ниже освещение
    Serial.print("Влажность земли: ");
    Serial.println(Greenhouse_sensor.soil_humidity1); // Чем выше значение, тем меньше влажность почвы
    Serial.print("Влажность воздуха:");
    Serial.println(Greenhouse_sensor.air_humidity); // Чем выше значение, тем меньше влажность воздуха
    Serial.print("Температура: ");
    Serial.println(Greenhouse_sensor.air_temperature); // Чем выше значение, тем больше температура
    Serial.print("Флаг HEAT ");
    Serial.println(Greenhouse_status_flag.HEAT);
    Serial.print("Флаг VENTILATION ");
    Serial.println(Greenhouse_status_flag.VENTILATION);
}


void pin_initialization() 
{
    pinMode(PIN_SEN_LIGHT, INPUT);
    pinMode(PIN_SEN_WET_EARTH, INPUT);
    pinMode(PIN_SEN_TEMP_WET, OUTPUT);

    pinMode(PIN_DIR_HEAT, OUTPUT);
    pinMode(PIN_DIR_WATER_PUMP, OUTPUT);
    pinMode(PIN_DIR_LIGHT, OUTPUT);
    pinMode(PIN_DIR_COOLER, OUTPUT);
}


void setup()
{
  Serial.begin(9600);

  pin_initialization();
  initialize_climate_parametrs();

  set_time();

  delay(10);
}


void loop() 
{
  current_time = millis();

  illumination_read();
  air_temp_humidity_read();
  soil_humidity_read();

  print_sensor_read();

  control_light();
  control_temperature();
  control_ventilation();
  control_watering();

  delay(10);
}
