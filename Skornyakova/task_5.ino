#include <DHT.h>
#define DHTTYPE DHT11

#define DHT_PIN 12        //переписать все дифы на капс 
#define LAMP_PIN 6        // лампа
#define FAN_PIN 7         // вентилятор
#define NAGREV_PIN 4      // наргреватель
#define PUMP_PIN 5        // помпа
#define HUM_SOIL_PIN A1     // влажность почвы
#define ILLUMINATION_PIN A3       // датчик света

DHT dht(DHT_PIN, DHTTYPE);

 
class Climate {
    public:
        int temperature_min;
        int temperature_max;
        int humidity_soil_min;
        int humidity_soil_max;
        int watering_interval;
        int illumination_min;
    
    };


void set_climate_tomato(Climate &climate)
{
    climate.temperature_max = 30;
    climate.temperature_min = 20;
    climate.humidity_soil_max = 10;
    climate.humidity_soil_min = 500; 
    climate.watering_interval = 5000;
    climate.illumination_min = 30;
    
}


void set_climate_potato(Climate &climate)
{
    climate.temperature_max = 30;
    climate.temperature_min = 20;
    climate.humidity_soil_max = 10;
    climate.humidity_soil_min = 5;
    climate.watering_interval = 5000; 
     climate.illumination_min = 30;
}


boolean FLAG_PUMP_SWITCH = false;
boolean FLAG_NAGREV_ON = false;

 
class Time {
    private:
        
        int minutes;
        int seconds;
        unsigned long previousMillis;

    public:
        int hours;
        int time_of_day;// Время суток (0 - ночь, 1 - день)
        
    public:
        Time(int h, int m, int s);
        void update_time();
        void update_time_of_day(); 
};

void Time::update_time() {
            const unsigned long interval = 1000; // Интервал в миллисекундах (1 секунда)
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= interval) {
                seconds += (currentMillis - previousMillis)/interval;
                previousMillis = currentMillis;
                if (seconds >= 60) {
                    seconds = 0;
                    minutes++;
                    if (minutes >= 60) {
                        minutes = 0;
                        hours++;
                        if (hours >= 24) {
                            hours = 0;
                        }
                    }
                }
                update_time_of_day(); // Обновляем время суток после изменения времени
            }
        }

        
void Time::update_time_of_day() {
              if (hours >= 6 && hours < 18) {
                 time_of_day = 1;
              } else {
                time_of_day = 0;
            }
        }   

Time::Time(int h, int m, int s) : hours(h), minutes(m), seconds(s), time_of_day(0), previousMillis(0) {
            if (hours < 0 || hours > 23) hours = 0;
            if (minutes < 0 || minutes > 59) minutes = 0;
            if (seconds < 0 || seconds > 59) seconds = 0;
            update_time_of_day(); 
        };

// Датчик освещенности
int is_dark(Climate &climate){
  if(analogRead(ILLUMINATION_PIN)< climate.illumination_min)
  return 1;
  else return 0;
}


// Влажность почвы
int Soil_humidity_sensor(){
  return analogRead(HUM_SOIL_PIN);
}


// Помпа
void PUMP_switch(const boolean PUMP_flag){
  digitalWrite(PUMP_PIN, PUMP_flag);
}


// Нагреватель
void Heating_switch(const boolean heat_flag){
  digitalWrite(NAGREV_PIN, heat_flag);
}


// Вентилятор
void Ventilation_switch(const boolean fan_flag){
  digitalWrite(FAN_PIN, fan_flag);
}


// УФ лампа
void Lamp_switch(const boolean lamp_flag){
  digitalWrite(LAMP_PIN, lamp_flag);
}


 void Illumination_control(Time &mytime,Climate &climate){
  if (mytime.time_of_day)
  Lamp_switch(is_dark(climate));
}


void Humidity_soil_control(Climate &climate){
  static unsigned long int Hum_Time = 0;
  if (Soil_humidity_sensor() > climate.humidity_soil_min && FLAG_PUMP_SWITCH == false && millis() - Hum_Time > 3 * climate.watering_interval) 
  {
    PUMP_switch(true);
    FLAG_PUMP_SWITCH = true;
    Hum_Time = millis(); 
  }
 else if (millis() - Hum_Time > climate.watering_interval){
    PUMP_switch(false);
    FLAG_PUMP_SWITCH = false; 
 }
}


void temperature_control(Climate &climate){
  float temp;
  temp = dht.readTemperature(); 
  if (temp < climate.temperature_min  and FLAG_PUMP_SWITCH == false){ 
    Heating_switch(true);
    FLAG_NAGREV_ON = true;
    Ventilation_switch(true);
  }
    else {
      Ventilation_switch(false);
      Heating_switch(false);
      FLAG_NAGREV_ON = false;
    }
  }
  

  void humidity_control(Climate &climate) { 
    float hum = dht.readHumidity();
    if (hum > climate.humidity_soil_min ) {
      Ventilation_switch(true);
    }
    else if(FLAG_NAGREV_ON == false) Ventilation_switch(false);
}
 
void Ventilation_at_time(int hours) { 
   static int last_hour = 0;
    if (hours != last_hour) {
      Ventilation_switch(true);
    }
    else if(FLAG_NAGREV_ON == false) Ventilation_switch(false);
    last_hour = hours;
}
 
  void Illumination_control(Time &mytime,Climate &climate);
  void Ventilation_at_time(Time &Mytime);
  Time Mytime(14, 30, 0);
void setup() {
  Serial.begin(9600);  
  dht.begin();

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(NAGREV_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

}

void loop() {
  
  Climate climate_tomato;
  set_climate_tomato(climate_tomato);
  Illumination_control(Mytime,climate_tomato);
  Humidity_soil_control(climate_tomato);
  temperature_control(climate_tomato);
  humidity_control(climate_tomato);
  Ventilation_at_time(Mytime.hours); 
  delay(10);
}
