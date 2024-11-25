#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6

const int color = 650;
int speed = 100;
void InitMotors()
{
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void Motors(int Speed1, int Speed2)
{
  if(Speed1 > 255) Speed1 = 255;
  if(Speed1 < -255) Speed1 = -255;
  if(Speed2 > 255) Speed2 = 255;
  if(Speed2 < -255) Speed2 = -255;

  if(Speed1 > 0){
    digitalWrite(M1_DIR, 1);
    analogWrite(M1_PWM, Speed1);
  }
  else{
    digitalWrite(M1_DIR, 0);
    analogWrite(M1_PWM, -Speed1);
  }

  if(Speed2 > 0){
    digitalWrite(M2_DIR, 1);
    analogWrite(M2_PWM, Speed2);
  }
  else{
    digitalWrite(M2_DIR, 0);
    analogWrite(M2_PWM, -Speed2);
  }
}


void MoveForward(const int Speed)
{
  Motors(Speed, Speed);
}


void MoveRight(const int Speed_Left ,const int Speed_Right)
{
  Motors(Speed_Left, - Speed_Right);
}


void MoveLeft(const int Speed_Left ,const int Speed_Right)
{
  Motors(- Speed_Left, Speed_Right);
}


int LineSensorRead(const int SensorPin)
{
  if(SensorPin < A0 || SensorPin > A3)
    return -1;

  int Data = analogRead(SensorPin);
  return Data;
}


void Stop()
{
  Motors(0, 0);
}


void setup()
{
  pinMode(12, INPUT_PULLUP);
  InitMotors();
  Serial.begin(9600);
}

void motor_stop() {
int speed = 100;
unsigned long time = millis();

while (analogRead(A0) < 650 && analogRead(A1) < 650) {
unsigned long timel = millis();
while (millis() - timel < 50){
  MoveLeft(255,255);
}
unsigned long timef = millis();
while (millis() - timef < 40){
  MoveForward(speed);
}

speed = speed + 2;

if (speed >= 170) {speed = 170;}
delay(100);

if (millis() - time > 5000)
{
while (1) {
Stop();
}
}
}
}

void loop() {
  if (LineSensorRead(A0) > color && LineSensorRead(A1) > color) {
    MoveForward(180);
  } 
  else if (LineSensorRead(A1) > color && LineSensorRead(A0) < color) {
    MoveLeft(230, 160);
  } 
  else if (LineSensorRead(A0) > color && LineSensorRead(A1) < color) {
    MoveRight(230, 160);
  } 
  else {
    motor_stop();
    delay(5);
  }
}