#include <DHT.h>
#define DHTTYPE DHT11

#define DHT_PIN 12        //переписать все дифы на капс 
#define LAMP_PIN 6        // лампа
#define FAN_PIN_1 7
#define FAN_PIN_2 9  // вентилятор
#define NAGREV_PIN_1 4 
#define NAGREV_PIN_2 3// наргреватель
#define PUMP_PIN 5        // помпа
#define HUM_SOIL_PIN A1     // влажность почвы
#define ILLUMINATION_PIN_1 A5       // датчик света
#define ILLUMINATION_PIN_2 A2
#define ILLUMINATION_PIN_3 A3


DHT dht(DHT_PIN, DHTTYPE);


typedef struct {
  int temperature_min;
  int temperature_max;
  int humidity_soil_min;
  int humidity_soil_max;
  int watering_interval;
  int illumination_min;
}Climate;


void set_climate_tomato(Climate *climate)
{
    climate->temperature_max = 30;
    climate->temperature_min = 20;
    climate->humidity_soil_max = 10;
    climate->humidity_soil_min = 500; 
    climate->watering_interval = 5000;
    climate->illumination_min = 30;
    
}


class Thermometer {
  private:
      int pin;
  public:
      float temperature;
  public:
      Thermometer(int mypin):pin(mypin){};
  public:
      void get_temperature(){temperature = dht.readTemperature();} 
  };


  class Light_sensor{
    private:
      int pin;
    public:
        int illumination;
    public:
      Light_sensor(int mypin){
        pin = mypin; 
      }
      void check_lighting(){illumination = analogRead(pin);} 
  };

  
 float getAverageAnalogValue(Light_sensor light_sensors)
{
  float sum = 0;
  for (int i=0; i < 3;i++){

      light_sensors[i].check_lighting();
      sum += light_sensors[i].illumination;
  }
  return (sum) / 3.0f;
} 


 class Humidity_sensor {
      private:
          int pin;
      public:
          float humidity;
      public:
          Humidity_sensor(int mypin):pin(mypin){};
      public:
          void get_humidity(){humidity = dht.readHumidity();} 
      };


  class Humidity_soil_sensor {
      private:
        int pin;
      public:
          float humidity_soil;
      public:
        Humidity_soil_sensor(int mypin) {
          pin = mypin;
        }
        void check_humidity_soil(){humidity_soil = analogRead(pin);}    
    };


  class Heater {
    private:
      int pin;
    public:
        bool on;
    public:
        Heater(const int pin);
    public:
        void start() {digitalWrite(pin, true);}  
        void stop() { digitalWrite(pin, false);} 
  };


  Heater::Heater( const int mypin)
  {
     pin = mypin;
     on = false;
  }
 
  
 class Ventilator {
    private:
        int pin;
    public:
        bool on;
    public:
        Ventilator(const int pin);
    public:
        void start() {digitalWrite(pin, true);}  
        void stop() { digitalWrite(pin, false);}  
   };


 Ventilator::Ventilator(const int mypin)
   {
      pin = mypin;
      on = false;    
   }


class Pump {
    private:
      int pin;
    public:
        bool on;
    public:
        Pump(const int pin);
    public:
        void start() {digitalWrite(pin, true);}  
        void stop() { digitalWrite(pin, false);} 
  };


Pump::Pump(const int mypin)
 {
    pin = mypin;
    on = false;    
 }


 class Lamp {
  private:
    int pin;
  public:
      bool on;
  public:
      Lamp(const int pin);
  public:
      void start() {digitalWrite(pin, true);}  
      void stop() { digitalWrite(pin, false);} 
};


Lamp::Lamp(const int mypin)
{
   pin = mypin;
   on = false;   
}


class Greenhouse{
  public:
       Thermometer thermometer;
       Humidity_sensor humidity_sensor;
       Humidity_soil_sensor humidity_soil_sensor;
       Lamp lamp;
       Pump pump;
       Ventilator ventilators[2];
       Light_sensor light_sensors[3];
       Heater heaters[2];
       Climate myclimate;
       
        Greenhouse(Thermometer mythermometer,Humidity_sensor myhumidity_sensor,Humidity_soil_sensor myhumidity_soil_sensor,Lamp mylamp,Pump pump,
        Ventilator myventilators[2],Climate myclimate,Light_sensor mylight_sensors[3],Heater myheaters[2]){};
      
        void Illumination_control(int time_of_day);
        void humidity_soil_control();
        void temperature_control();
        void humidity_control();
        void ventilation_at_time(int hours);

};

Greenhouse::Greenhouse(Thermometer mythermometer,Humidity_sensor myhumidity_sensor,Humidity_soil_sensor myhumidity_soil_sensor,Lamp mylamp,Pump pump,Ventilator myventilators[2],Climate myclimate,Light_sensor mylight_sensors[3],
    Heater myheaters[2]){
  thermometer = mythermometer;
  humidity_sensor = myhumidity_sensor;
  humidity_soil_sensor = myhumidity_soil_sensor;
  lamp = mylamp;
  pump= mypump;
  ventilators[2] = myventilators[2];
  climate = myclimate;
  light_sensors[3] = mylight_sensors[3];
  heaters[2]= myheaters[2];
};


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


 void Greenhouse::Illumination_control(int time_of_day){
  if (time_of_day && (getAverageAnalogValue(Light_sensor light_sensors[3])< myclimate.illumination_min)){
    lamp.on = true;
  } else lamp.on = false;
}


void Greenhouse::humidity_soil_control(){
  static unsigned long int Hum_Time = 0;
  if (humidity_soil_sensor.humidity_soil > climate.humidity_soil_min && pump.on == false && millis() - Hum_Time > 3 * climate.watering_interval) //магическое число 3 
  {
    pump.on = true;
    Hum_Time = millis(); 
  }
 else if (millis() - Hum_Time > climate.watering_interval){
    pump.on = false; 
 }
}


void Greenhouse::temperature_control(){
   
  if ((thermometer. temperature < climate.temperature_min) && (pump.on == false)){ 
    for (int i=0; i < 2;i++){
        heaters[i].on = true;
    }
    for (int i=0; i < 2;i++){
        ventilators[i].on = true;
    } 
  }
  else {
    for (size_t i=0; i < 2;i++){
      heaters[i].on = false;
  }
  for (size_t i=0; i < 2;i++){
      ventilators[i].on = false;
  }
  }
}
  

  void Greenhouse::humidity_control() { 
    if (humidity_sensor.humidity > climate.humidity_soil_min) {
      for (size_t i=0; i < 2;i++){
        ventilators[i].on = true;
      }
    }
    else { 
      if(heaters[0].on == false){
            for (size_t i=0; i < 2;i++){
                  ventilators[i].on = false;
            }
      }
    }
}
 

void Greenhouse::ventilation_at_time(int hours) { 
   static int last_hour = 0;
    if (hours != last_hour) {
      for (int i=0; i < 2;i++){
        ventilators[i].on = true;
      }
    }
    else {
      if(heaters[0].on == false){
        for (size_t i=0; i < 2;i++){
          ventilators[i].on = false;
        }
      }  
    }
    last_hour = hours;
}


void do_heat(Heater heaters[2])
{
    if(heaters[0].on) {
      for (size_t i=0; i < 2;i++){
        heaters[i].start();
      }
    }
    else{
      for (size_t i=0; i < 2;i++){
        heaters[i].stop();
      }
    }
}


void do_ventilation(Ventilator ventilators[2])
{
    if(ventilators[0].on) {
      for (size_t i=0; i < 2;i++){
        ventilators[i].start();
      }
    }
    else {
      for (size_t i=0; i < 2;i++){
        ventilators[i].stop();
      }
    }
}


void do_pump(Pump pump)
{
    if(pump.on) {
      pump.start();
    }
    else{
      pump.stop();
    }
}


void do_lamp(Lamp lamp)
{
    if(lamp.on) {
      lamp.start();
    }
    else{
      lamp.stop();
    }
}

  Time Mytime(14, 30, 0);
void setup() {
  Serial.begin(9600);  
  dht.begin();

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(FAN_PIN_1, OUTPUT);
  pinMode(FAN_PIN_2, OUTPUT);
  pinMode(NAGREV_PIN_1, OUTPUT);
  pinMode(NAGREV_PIN_2, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

}


void loop() {
  Mytime.update_time();

  Climate climate_tomato;
  set_climate_tomato(&climate_tomato);
  Thermometer mythermometer(DHT_PIN);
  Humidity_sensor myhumidity_sensor(DHT_PIN);
  Humidity_soil_sensor myhumidity_soil_sensor(HUM_SOIL_PIN);
  Lamp mylamp(LAMP_PIN);
  Pump mypump(PUMP_PIN);
  Ventilator my_first_ventilator(FAN_PIN_1),my_second_ventilator(FAN_PIN_2);
  Light_sensor my_first_light_sensor(ILLUMINATION_PIN_1),my_second_light_sensor(ILLUMINATION_PIN_2),my_third_light_sensor(ILLUMINATION_PIN_3);
  Heater my_first_heater(NAGREV_PIN_1),my_second_ventilators(NAGREV_PIN_2);
 
  Greenhouse greenhouse_for_tomatoes(mythermometer,myhumidity_sensor,mylamp,mypump,{my_first_ventilator,my_second_ventilator},climate_tomato,
    { my_first_light_sensor,my_second_light_sensor,my_third_light_sensor},{my_first_heater,my_second_ventilators});
  
  
  greenhouse_for_tomatoes.Illumination_control(Mytime.time_of_day);
  greenhouse_for_tomatoes.humidity_soil_control();
  greenhouse_for_tomatoes.temperature_control();
  greenhouse_for_tomatoes.humidity_control();
  greenhouse_for_tomatoes.ventilation_at_time(Mytime.hours); 

  do_heat(greenhouse_for_tomatoes.heaters);
  do_ventilation(greenhouse_for_tomatoes.ventilators);
  do_pump(greenhouse_for_tomatoes.pump);
  do_lamp(greenhouse_for_tomatoes.lamp);
  delay(10);
}
