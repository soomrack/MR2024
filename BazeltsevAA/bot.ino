#define PIN_DIR_R 4
#define PIN_DIR_L 7

#define PIN_PWR_R 5     
#define PIN_PWR_L 6

#define PIN_BTN 8

#define PIN_PIEZO 9

typedef unsigned int Colour; 

Colour black, white; // значения сигналов полученных при каллибровке для белого и черного цвета

Colour right_sensor, left_sensor; //текущие значения правого и оевого датчика

Colour lastseen; // крайнее значение сигнала перед съездом с линии

Colour border; //Среднее арефметическое значение между белым и черным цветом

unsigned int search_time = 0;


void pin_initialization()
{
    pinMode(A0, INPUT);
    pinMode(A2, INPUT);

    pinMode(PIN_DIR_R, OUTPUT);
    pinMode(PIN_PWR_R, OUTPUT);
    pinMode(PIN_PWR_L, OUTPUT);
    pinMode(PIN_DIR_L, OUTPUT);
    pinMode(PIN_PIEZO, OUTPUT);
}


void calibration()
{
    while (!digitalRead(PIN_BTN)) {}

    right_sensor = analogRead(A0);
    left_sensor = analogRead(A2);
    white = (right_sensor + left_sensor) / 2;
    delay(500);
    
    while (!digitalRead(PIN_BTN)) {}

    right_sensor = analogRead(A0);
    left_sensor = analogRead(A2);
    black = (right_sensor + left_sensor) / 2;
    border = (white + black) / 2;

    delay(500);  

    while (!digitalRead(PIN_BTN)) {}
}


void forward()  //вперёд
{
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 1);

    //Велчины 50 и 230 подобраны эмпирически
    for (int idx = 50; idx <= 230; idx += 6) {   
        analogWrite(PIN_PWR_R, idx);
        analogWrite(PIN_PWR_L, idx);
    }

    lastseen = left_sensor;
    search_time = 0;
}


void leftward()  //поворот налево     
{
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 0);

    for (int idx = 0; idx <= 150; idx += 6) {
        analogWrite(PIN_PWR_R, idx);
    }

    for (int idx = 0; idx <= 150; idx += 6) {
        analogWrite(PIN_PWR_L, idx);
    }

    search_time = 0;
}


void rightward()  //поворот направо   
{
    digitalWrite(PIN_DIR_R, 0);
    digitalWrite(PIN_DIR_L, 1);

    for (int idx = 0; idx <= 150; idx += 6) {
        analogWrite(PIN_PWR_R, idx);
    }

    for (int idx = 0; idx <= 150; idx += 6) {
        analogWrite(PIN_PWR_L, idx);
    }

    search_time = 0;
}

void out_of_line_left()   //сход с линии влево     
{
    //Данные скорости двигателей 50 для правого и 250 для левого подобраны на основе опытных данных, чтобы выполнялся задний поворот вправо
    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 250);
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 50);
}
void out_of_line_right()   //сход с линии вправо   
{
    //Данные скорости двигателей 50 для правого и 250 для левого подобраны на основе опытных данных, чтобы выполнялся задний поворот влево
    digitalWrite(PIN_DIR_R,0);
    analogWrite(PIN_PWR_R,50);
    digitalWrite(PIN_DIR_L,0);
    analogWrite(PIN_PWR_L,250);
}


void stop() 
{     
    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 0);
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 0);
    
    tone(PIN_PIEZO, 1000, 500);

    search_time = 0;

    if (digitalRead(PIN_BTN)) {
        exit(1); 
    }  

}


void setup() 
{
    pin_initialization();
    calibration();   
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
        search_time++;
        delay(1);

        if (lastseen > border) {
            
            out_of_line_left();  //Сход с линии влево

            if (search_time > 5000) {
                stop();
            }
        }

        else {

            out_of_line_right();  //Сход с линии вправо

            if (search_time > 5000) {
                stop();
            }
        }
    }        
}

    delay (15);          
}
