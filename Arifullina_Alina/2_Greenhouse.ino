#include <DHT.h>
#define DHTTYPE DHT11

#define DHT_PIN 12        
#define LAMP_PIN 6        
#define FAN_PIN_1 7
//#define FAN_PIN_2 9  
#define NAGREV_PIN_1 4 
//#define NAGREV_PIN_2 3
#define PUMP_PIN 5      
#define HUM_SOIL_PIN A1     
#define ILLUMINATION_PIN_1 A5       
//#define ILLUMINATION_PIN_2 A2
//#define ILLUMINATION_PIN_3 A3

class Light_sensors;
class Lamp;
class Humidity_soil_sensor;
class Pump;
class Thermometer;
class Heaters;
class Ventilators;
class Humidity_sensor;
class Time;


DHT dht(DHT_PIN, DHTTYPE);


typedef struct {
  int temperature_min;
  int temperature_max;
  int humidity_soil_min;
  int humidity_soil_max;
  unsigned int watering_interval;
  int illumination_min;
} Climate;


void set_climate(Climate *climate)
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
    Light_sensor(int mypin):pin(mypin){};
    void check_lighting(){illumination = analogRead(pin);} 
};

class Light_sensors{
  public:
    int count;
    Light_sensor light_sensors[];
    
    Light_sensors(int mycount, Light_sensor mylight_sensors[]): count(mycount){   
      for(int i = 0; i < mycount; i++) {
        light_sensors[i] = mylight_sensors[i];
      }
    }
    float Get_average_analog_value();     
};

float Light_sensors::Get_average_analog_value()
{
  float sum = 0;
  for (int i=0; i < count;i++){
    light_sensors[i].check_lighting();
    sum += light_sensors[i].illumination;
  }
  return (sum) / 3;
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
    Humidity_soil_sensor()= delete;
    Humidity_soil_sensor(int mypin):pin(mypin){};
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
    void start(){digitalWrite(pin, true);}  
    void stop(){digitalWrite(pin, false);} 
};

Heater::Heater(const int mypin) {
  pin = mypin;
  on = false;
}
  
class Heaters{
  public:
    int count;
    Heater heaters[];
    
    Heaters(int mycount, Heater myheaters[]): count(mycount){
      for(int i = 0; i < mycount; i++) {
        heaters[i] = myheaters[i];
      }
    }
    void start();
    void stop();     
}; 

void Heaters::start(){
  for(int i = 0; i < count; i++) {
        heaters[i].start();
    }
}

void Heaters::stop(){
  for(int i = 0; i < count; i++) {
        heaters[i].stop();
    }
}
 
class Ventilator{
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

 Ventilator::Ventilator(const int mypin){
      pin = mypin;
      on = false;    
}

class Ventilators{
  public:
    int count;
    Ventilator ventilators[];
    
    Ventilators(int mycount, Ventilator myventilators[]): count(mycount){
    for(int i = 0; i < mycount; i++) {
        ventilators[i] = myventilators[i];
    }
    }
    void start();
    void stop();     
}; 

void Ventilators::start(){
  for(int i = 0; i < count; i++) {
        ventilators[i].start();
  }
}

void Ventilators::stop(){
  for(int i = 0; i < count; i++) {
        ventilators[i].stop();
  }
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

Pump::Pump(const int mypin){
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

class Time {
  private:
        int minutes;
        int seconds;
        unsigned long previousMillis;

    public:
        int hours;
        char time_of_day;// Время суток (0 - ночь, 1 - день)
        
    public:
        Time(int h, int m, int s);
        void update_time();
        void update_time_of_day(); 
};


void Time::update_time() {
const unsigned long interval = 1000;
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
      update_time_of_day();
}
}
   
Time::Time(int h, int m, int s) : minutes(m),seconds(s),previousMillis(0),hours(h),
      time_of_day(0){
            if (hours < 0 || hours > 23) hours = 0;
            if (minutes < 0 || minutes > 59) minutes = 0;
            if (seconds < 0 || seconds > 59) seconds = 0;
            update_time_of_day(); 
        };
        
void Time::update_time_of_day() {
      if (hours >= 6 && hours < 18) {
            time_of_day = 1;
            } 
      else {
            time_of_day = 0;
      }
}   

void Illumination_control(char time_of_day, Climate myclimate, Light_sensors mylight_sensors, Lamp mylamp){
    if (time_of_day && (mylight_sensors.Get_average_analog_value()< myclimate.illumination_min)){
      mylamp.on = true;
    } else mylamp.on = false;
}

void humidity_soil_control(Humidity_soil_sensor myhumidity_soil_sensor,Climate myclimate,Pump mypump){
  static unsigned long int Hum_Time = 0;
  myhumidity_soil_sensor.check_humidity_soil();
  if (myhumidity_soil_sensor.humidity_soil > myclimate.humidity_soil_min && mypump.on == false && millis() - Hum_Time > 3 * myclimate.watering_interval) 
  {
    mypump.on = true;
    Hum_Time = millis(); 
  }
  else if (millis() - Hum_Time > myclimate.watering_interval){
    mypump.on = false; 
  }
}

void temperature_control(Thermometer mythermometer, Climate myclimate,Pump mypump,Heaters myheaters,Ventilators myventilators){ 
  mythermometer.get_temperature();
  if ((mythermometer.temperature < myclimate.temperature_min) && (mypump.on == false)){ 
    for (int i=0; i < myheaters.count;i++){
        myheaters.heaters[i].on = true;
    }
    for (int i=0; i < myventilators.count;i++){
        myventilators.ventilators[i].on = true;
    } 
  }
  else {
    for (int i=0; i < myheaters.count;i++){
      myheaters.heaters[i].on = false;
    }
    for (int i=0; i < myventilators.count;i++){
      myventilators.ventilators[i].on = false;
    }
  }
}
  
void humidity_control(Humidity_sensor myhumidity_sensor,Climate myclimate,Ventilators myventilators,Heaters myheaters){ 
    if (myhumidity_sensor.humidity > myclimate.humidity_soil_min) {
      for (int i=0; i < myventilators.count;i++){
        myventilators.ventilators[i].on = true;
      }
    }
    else { 
      if(myheaters.heaters[0].on == false){
            for (int i=0; i < myventilators.count;i++){
                  myventilators.ventilators[i].on = false;
            }
      }
    }
}

void ventilation_at_time(int hours,Ventilators myventilators,Heaters myheaters) { 
   static int last_hour = 0;
    if (hours != last_hour) {
      for (int i=0; i < myventilators.count;i++){
        myventilators.ventilators[i].on = true;
      }
    }
    else {
      if(myheaters.heaters[0].on == false){
        for (int i=0; i < myventilators.count;i++){
          myventilators.ventilators[i].on = false;
        }
      }  
    }
    last_hour = hours;
}

void do_heat(Heaters myheaters)
{
    if(myheaters.heaters[0].on) {
      for (int i=0; i < myheaters.count;i++){
        myheaters.heaters[i].start();
      }
    }
    else{
      for (int i=0; i < myheaters.count;i++){
        myheaters.heaters[i].stop();
      }
    }
}

void do_ventilation(Ventilators myventilators)
{
    if( myventilators.ventilators[0].on) {
      for (int i=0; i < myventilators.count;i++){
        myventilators.ventilators[i].start();
      }
    }
    else {
      for (int i=0; i < myventilators.count;i++){
         myventilators.ventilators[i].stop();
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


  
Time mytime(14, 30, 0);
  
void setup() {
  Serial.begin(9600);  
  dht.begin();

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(FAN_PIN_1, OUTPUT);
  //pinMode(FAN_PIN_2, OUTPUT);
  pinMode(NAGREV_PIN_1, OUTPUT);
  //pinMode(NAGREV_PIN_2, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  
  
}


void loop() {
  mytime.update_time();

  static Pump mypump(PUMP_PIN);
  static Lamp mylamp(LAMP_PIN);

  Illumination_control(Mytime.time_of_day, climate, mylight_sensors, mylamp);
  humidity_soil_control(myhumidity_soil_sensor, climate,mypump);
  temperature_control(mythermometer, climate, mypump, myheaters, myventilators);
  humidity_control(myhumidity_sensor, climate, myventilators, myheaters);
  ventilation_at_time(Mytime.hours, myventilators, myheaters); 

  do_heat(myheaters);
  do_ventilation(myventilators);
  do_pump(mypump);
  do_lamp(mylamp);
  delay(1000);
  
}