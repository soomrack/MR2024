#define RIGHT_SPEED               5
#define LEFT_SPEED                6
#define RIGHT_DIRECTION           4
#define LEFT_DIRECTION            7 
#define LIGHT_RIGHT               1
#define LIGHT_LEFT                0
#define SOUND                     8


int light = 0;
int time;


void setup() 
{
  Serial.begin(9600);
  pinMode(LIGHT_RIGHT, INPUT);
  pinMode(LIGHT_LEFT, INPUT);
  pinMode(SOUND, OUTPUT);
}


int binarize(int detect)
{
  if (detect >= 800) return 1;
  else return 0;
}


void forward(int speed)
{
  digitalWrite(RIGHT_DIRECTION, HIGH);
  digitalWrite(LEFT_DIRECTION, HIGH);

  analogWrite(RIGHT_SPEED, speed); 
  analogWrite(LEFT_SPEED, speed); 
}


void stop() { 
  analogWrite(RIGHT_SPEED, LOW); 
  analogWrite(LEFT_SPEED, LOW); 
  
  while (true) {
    digitalWrite(SOUND, HIGH);

    if ((binarize(analogRead(LIGHT_RIGHT))==1)  ||  (binarize(analogRead(LIGHT_LEFT) == 1)))
    {
      digitalWrite(SOUND, LOW);
      break;
    }
    delay(300);

    digitalWrite(SOUND, LOW);

    delay(300);
  };
} 


void turn(int right, int left, int speed) 
{ 
  if((right == 1) && (left == 0)) {
    digitalWrite(RIGHT_DIRECTION, LOW); 
    digitalWrite(LEFT_DIRECTION, HIGH); 
  } else if ((right == 0) && (left == 1)) {
    digitalWrite(RIGHT_DIRECTION,HIGH); 
    digitalWrite(LEFT_DIRECTION, LOW);
  }
  analogWrite(RIGHT_SPEED, speed);
  analogWrite(LEFT_SPEED, speed);
} 


void search() {
  unsigned long time = millis();
  int speed_increment = 100;
  while ((binarize(analogRead(LIGHT_RIGHT)) == 0) && (binarize(analogRead(LIGHT_LEFT)) == 0))
  { 
    delay(100); 
    turn(!light, light, 150);
    delay(100); 
    forward(speed_increment); 


    if (speed_increment > 150) { 
      speed_increment = 150; 
    } 
    speed_increment += 1; 

    if (millis() - time > 10000) {
      stop();
      break;
    }
    delay(100);
  } 
}


void loop() {
  while (1) {
    if ((binarize(analogRead(LIGHT_LEFT)) == 1) && (binarize(analogRead(LIGHT_RIGHT)) == 1))
      forward(255);
    
    if ((binarize(analogRead(LIGHT_LEFT)) == 1) && (binarize(analogRead(LIGHT_RIGHT)) == 0))
      turn(0, 1, 255);
    
    if ((binarize(analogRead(LIGHT_LEFT)) == 0) && (binarize(analogRead(LIGHT_RIGHT)) == 1))
      turn(1, 0, 255);

    if ((binarize(analogRead(LIGHT_LEFT)) == 0) && (binarize(analogRead(LIGHT_RIGHT)) == 0))
      search();

    light = binarize(analogRead(LIGHT_LEFT));

    delay(5);
  }  
}
