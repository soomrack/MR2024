
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

#define PIN_SEN_LIGHT A0
#define PIN_WET_EARTH A1
#define PIN_HEAT 4
#define PIN_WATER_PUMP 5
#define PIN_LIGHT 6
#define PIN_COOLER 7
#define PIN_TEMP_WET 12

unsigned long previousMillis = 0;
unsigned int min_light = 750;
unsigned int min_wet = 800;
unsigned int max_hum = 15;
unsigned int need_temp = 25;
unsigned int light = 0;
unsigned int wet = 0;
unsigned long temp = 0;
unsigned int hum = 0;
unsigned long currentMillis;
unsigned long get_millis;
int temp_switch = 0;
int heat_swith = 0;
int pump_switch = 0;
int cooler_switch = 0;
int hum_switch = 0;
int cooldown_hum = 100000;
int  cooldown_cooler = 30000;

int hu = 0;
int q = 0;
int cooldown = 10;
int counter = 0;
unsigned int sec, hour, min, hour_to_set, sec_to_set, min_to_set;

void pin_initialization() {
    pinMode(PIN_SEN_LIGHT, INPUT);
    pinMode(PIN_WET_EARTH, INPUT);
    pinMode(PIN_HEAT, OUTPUT);
    pinMode(PIN_WATER_PUMP, OUTPUT);
    pinMode(PIN_LIGHT, OUTPUT);
    pinMode(PIN_COOLER, OUTPUT);
    pinMode(PIN_TEMP_WET, OUTPUT);
}

void check(int PIN) {
  digitalWrite(PIN, 1);
  delay(5000);
  digitalWrite(PIN, 0);
}


//getting block
void get_light(){
  light = analogRead(PIN_SEN_LIGHT);
}

void get_air_humidity{
  hum = DHT.humidity;
}

void get_soil_humidity(){
  wet = analogRead(PIN_WET_EARTH);
}

void get_temperature(){
  temp = DHT.temperature;
}

void get_time(){
  get_millis = millis();
}

//control block
void check_light(){
  
  if(hour < 11){
      light_switch = 0;
      return;
  }
  if(min_light < light) && (digitalRead(PIN_LIGHT) == 0){
    light_switch = 1;
  }
}

void check_temp(){

  if(temp <= need_temp){
    heat_swith = 1;
  }
  else{
    heat_swith = 0;
  }
}

void check_wet(wet, min_wet){
  if(cooldown >= 10){
    if(wet >= min_wet){
      if(counter >= 10){
        pump_switch = 0;
        counter = 0;
        cooldown = 0;
      }
      else{
        pump_switch = 1;
        counter += 1;
        }
      }
    }  
  }

void check_hum(){
  unsigned long hum_time = get_millis;
  if(hum >= max_hum){
    hum_switch = 1;
    return;
  }
  if (get_millis - hum_time <= cooldown_hum){
    return;
  }
  else{
    hum_switch = 0;
  }
}

void check_cooler{
  unsigned long cooler_time = get_millis;
  if(heat_switch == 1){
    cooler_switch = 1;
    return;
  }
  if(hour == 10 || hour == 15 || hour == 20){
    cooler_switch = 1;
    if(get_millis - cooler_time >= cooldown_cooler) {
      cooler_switch = 0;
    }
  }
}

//activate block

void switch_hum(){
  if (hum_switch){
    on(PIN_COOLER);
  }
  else{
    off(PIN_COOLER);
  }
}

void switch_light(){
  if (light_switch){
    on(PIN_LIGHT);
  }
  else{
    off(PIN_LIGHT);
  }
}

void switch_temp(){
  if (temp_switch){
    on(PIN_TEMP_WET);
  }
  else{
    off(PIN_TEMP_WET);
  }
}

void switch_heat(){
  if (heat_switch){
    on(PIN_HEAT);
  }
  else{
    off(PIN_HEAT);
  }
}

void cooler_switch(){
  if (cooler_switch){
    on(PIN_COOLER);
  }
  else{
    off(PIN_COOLER);
  }
}

//printer
void detector_print(){
  value();
  Serial.print("------------------------------\n");
  Serial.print("Light: "); Serial.println(light);
  Serial.print("humi_earth: "); Serial.println(wet);
  //DHT.read(PIN_TEMP_WET);
  Serial.print("temp: "); Serial.println(temp);
  Serial.print("humi: "); Serial.println(hum);
  Serial.print("");
}



//clock
void HMS_set(unsigned int hour_to_set, unsigned int min_to_set, unsigned int sec_to_set){
  hour = hour_to_set;
  min = min_to_set;
  sec = sec_to_set;
}
void HMS_count(){
  if((millis() - currentMillis) >= 1000){
    sec = sec += (millis() - currentMillis) / 1000;
    currentMillis = millis();
  }
  if(sec >= 60){
    sec = 0;
    min += 1;
    cooldown += 1;
  }
  if (min >= 60){
    min = 0;
    hour += 1;
    h_hu += 1;
  }
  if(hour >= 24){
    hour = 0;
  }
}
void HMS_print(){
  Serial.println("Time: " + String(hour) + ":" + String(min) + ":" + String(sec));
}



int timer(string u,int cou){
  if (cou != 0){

  
    if (u =="h"){
      if (q = 0){
        q = 60 * h + min;
      }
      cou = cou - (q - 60 * h + min);
    }
  }
  else{
    q = 0;
    return 0
  }
}

void on(int PIN) {
  digitalWrite(PIN, 1);
}

void off(int PIN) {
  digitalWrite(PIN, 0);
}



void setup() {
  pin_initialization();
  HMS_set(11, 59, 50);

  Serial.begin(115200);
}

void loop(){
  //getting sensors
  get_light();
  get_temperature();
  get_time();
  get_air_humidity();
  get_soil_humidity(); 

//checking 
  check_hum();
  check_wet(wet, min_wet);
  check_temp();
  check_light();
  check_cooler();

//activating
  switch_wet();
  switch_temp(); 
  switch_light();
  switch_hum();
  switch_cooler();
  //activates

  HMS_count();
  HMS_print();
  detector_print(); 
  delay(1000);
  
  }
  

  
  // put your main code here, to run repeatedly:
  
//А0 Датчик освещённости +
//А1 Датчик влажности почвы +
//4 подогрев в вентиляторе+
//5  Помпа с подачей воды	+
//6 Подсветка+
//7 Вентиляция+
//12 Влажность и температура воздуха+
