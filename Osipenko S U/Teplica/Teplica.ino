
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

#define PIN_SEN_LIGHT A0
#define PIN_WET_EARTH A1
#define PIN_HEAT 4
#define PIN_WATER_PUMP 5
#define PIN_LIGHT 6
#define PIN_COOLER 7
#define PIN_TEMP_WET 12

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
void on(int PIN) {
  digitalWrite(PIN, 1);
}
void off(int PIN) {
  digitalWrite(PIN, 0);
}
void setup() {
  pin_initialization();
  off(5);
  off(7);
  check(PIN_LIGHT);
  Serial.begin(115200);
}

void loop() {
  Serial.print("light:");
  Serial.print(analogRead(PIN_SEN_LIGHT));
  Serial.print("  humi_earth:");
  
  Serial.print(analogRead(PIN_WET_EARTH));
  DHT.read(PIN_TEMP_WET);
  Serial.print("  temp:");
  Serial.print(DHT.temperature);
  Serial.print("  humi:");
  Serial.println(DHT.humidity);
  delay(1000);
  // put your main code here, to run repeatedly:
  }
//А0 Датчик освещённости +
//А1 Датчик влажности почвы +
//4 подогрев в вентиляторе+
//5  Помпа с подачей воды	+
//6 Подсветка+
//7 Вентиляция+
//12 Влажность и температура воздуха+
