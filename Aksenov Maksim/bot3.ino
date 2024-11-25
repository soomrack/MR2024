#define DIR_R 4
#define DIR_L 7

#define PWR_R 5     
#define PWR_L 6

#define BTN 8

#define PIEZO 9

typedef unsigned int colour;

colour black, white;

colour right_sensor, left_sensor;

colour lastseen;

colour border;

unsigned int search_time = 0;


void pin_initialization()
{
    pinMode(A0, INPUT);
    pinMode(A2, INPUT);

    pinMode(DIR_R, OUTPUT);
    pinMode(PWR_R, OUTPUT);
    pinMode(PWR_L, OUTPUT);
    pinMode(DIR_L, OUTPUT);
    pinMode(PIEZO, OUTPUT);
}


void calibration()
{
    while (!digitalRead(BTN)) {}

    right_sensor = analogRead(A0);
    left_sensor = analogRead(A2);
    white = (right_sensor + left_sensor) / 2;
    delay(500);
    
    while (!digitalRead(BTN)) {}

    right_sensor = analogRead(A0);
    left_sensor = analogRead(A2);
    black = (right_sensor + left_sensor) / 2;
    border = (white + black) / 2;

    delay(500);  

    while (!digitalRead(BTN)) {}
}


void forward()  //вперёд
{
    digitalWrite(DIR_R, 1);
    digitalWrite(DIR_L, 1);

    for (int idx = 50; idx <= 230; idx += 6) {
        analogWrite(PWR_R, idx);
        analogWrite(PWR_L, idx);
    }

    lastseen = left_sensor;
    search_time = 0;
}


void leftward()  //поворот влево     
{
    digitalWrite(DIR_R, 1);
    digitalWrite(DIR_L, 0);

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PWR_R, i);
    }

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PWR_L, i);
    }

    search_time = 0;

    delay(15);
}


void rightward()  //поворот вправо   
{
    digitalWrite(DIR_R, 0);
    digitalWrite(DIR_L, 1);

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PWR_R, i);
    }

    for (int i = 0; i <= 150; i += 6) {
        analogWrite(PWR_L, i);
    }

    search_time = 0;

    delay(15);
}


void out_of_line()   //сход с линии     
{
    search_time++;
    delay(1);

    if (lastseen > border) {
        digitalWrite(DIR_R, 0);
        analogWrite(PWR_R, 250);
        digitalWrite(DIR_L, 0);
        analogWrite(PWR_L, 50);

        if (search_time > 5000) {
            stop();
        }
    }

    else {
        digitalWrite(DIR_R,0);
        analogWrite(PWR_R,50);
        digitalWrite(DIR_L,0);
        analogWrite(PWR_L,250);

        if (search_time > 5000) {
            stop();
        }
    }
}

void setup() 
{
    pin_initialization();
    calibration();   
}


void stop() 
{     
    digitalWrite(DIR_R, 0);
    analogWrite(PWR_R, 0);
    digitalWrite(DIR_L, 0);
    analogWrite(PWR_L, 0);
    
    tone(PIEZO, 1000, 500);

    search_time = 0;

    while (!digitalRead(BTN)) {}

    delay (500);

    exit(1);   
}


void loop() 
{
    right_sensor = analogRead(A0);  
    left_sensor = analogRead(A2);

    if (right_sensor > border && left_sensor > border) { //вперёд
        forward();
    }

    else if (left_sensor > border && right_sensor < border) {  //поворот влево     
        leftward();
    }

    else if (left_sensor < border && right_sensor > border) {  //поворот вправо
        rightward();
    }
  
    else if (left_sensor < border && right_sensor < border) { //сход с линии
        out_of_line();           
    }
}
