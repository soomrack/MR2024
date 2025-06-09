#define PIN_DIR_R 4
#define PIN_DIR_L 7
#define PIN_PWR_R 5
#define PIN_PWR_L 6
#define PIN_BTN 8
#define PIN_PIEZO 11
#define PIN_SEN_L A1
#define PIN_SEN_R A0

typedef unsigned int Colour;
int a = 0;
int k = 0;
Colour black, white; // значения сигналов полученных при каллибровке для белого и черного цвета
Colour r_sensor, l_sensor; // текущие значения правого и левого датчика
Colour last_color; // крайнее значение сигнала перед съездом с линии
Colour gray; // Среднее арифметическое значение между белым и черным цветом
unsigned int search_time = 0;
unsigned long previousMillis = 0; // Переменная для отслеживания времени

void pin_initialization() {
    pinMode(PIN_SEN_R, INPUT);
    pinMode(PIN_SEN_L, INPUT);
    pinMode(PIN_DIR_R, OUTPUT);
    pinMode(PIN_PWR_R, OUTPUT);
    pinMode(PIN_PWR_L, OUTPUT);
    pinMode(PIN_DIR_L, OUTPUT);
    pinMode(PIN_PIEZO, OUTPUT);
}

void calibration() {
    while (!digitalRead(PIN_BTN)) {}

    r_sensor = analogRead(PIN_SEN_R);
    l_sensor = analogRead(PIN_SEN_L);
    white = (r_sensor + l_sensor) / 2;
    delay(500); // избежание ложного нажатия кнопки

    while (!digitalRead(PIN_BTN)) {}

    r_sensor = analogRead(PIN_SEN_R);
    l_sensor = analogRead(PIN_SEN_L);
    black = (r_sensor + l_sensor) / 2;
    gray = (white + black) / 2;

    delay(500); // избежание ложного нажатия кнопки

    while (!digitalRead(PIN_BTN)) {}
}

void forward() { // вперёд
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 1);

    // Величины 50 и 230 подобраны эмпирически
    for (int idx = 50; idx <= 230; idx += 6) {
        analogWrite(PIN_PWR_R, idx);
        analogWrite(PIN_PWR_L, idx);
    }

    last_color = l_sensor;
    search_time = 0;
}

void leftward() { // поворот налево
    digitalWrite(PIN_DIR_R, 1);
    digitalWrite(PIN_DIR_L, 0);

    for (int idx = 0; idx <= 180; idx += 6) {
        analogWrite(PIN_PWR_R, idx);
    }

    for (int idx = 0; idx <= 180; idx += 6) {
        analogWrite(PIN_PWR_L, idx);
    }

    search_time = 0;
}

void playmusic() {
    if (k == 0 || k == 4){
    k = 0;
    tone(PIN_PIEZO, 1000, 100);
    k += 1;
    }
    else if (k == 1){
    tone(PIN_PIEZO, 1000, 100);
    k += 1;
    }
    else if (k == 2){
    tone(PIN_PIEZO, 1000, 100);
    k += 1;
    }
    else if (k == 3){
    tone(PIN_PIEZO, 1000, 100);
    k += 1;
    }
}

void rightward() { // поворот направо
    digitalWrite(PIN_DIR_R, 0);
    digitalWrite(PIN_DIR_L, 1);

    for (int idx = 0; idx <= 180; idx += 6) {
        analogWrite(PIN_PWR_R, idx);
    }

    for (int idx = 0; idx <= 180; idx += 6) {
        analogWrite(PIN_PWR_L, idx);
    }

    search_time = 0;
}

void out_of_line_l() { // сход с линии влево
    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 250);
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 50);
}

void out_of_line_r() { // сход с линии вправо
    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 50);
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 250);
}

void stop() {
    digitalWrite(PIN_DIR_R, 0);
    analogWrite(PIN_PWR_R, 0);
    digitalWrite(PIN_DIR_L, 0);
    analogWrite(PIN_PWR_L, 0);

    tone(PIN_PIEZO, 1000, 500);

    search_time = 0;

    a = 1;
}

void setup() {
    pin_initialization();
    calibration();
}

void loop() {
    r_sensor = analogRead(PIN_SEN_R);
    l_sensor = analogRead(PIN_SEN_L);

    if (a == 0) {
        if (r_sensor > gray && l_sensor > gray) { // вперёд
            forward();
        } else if (l_sensor > gray && r_sensor < gray) {  // поворот влево
            leftward();
        } else if (l_sensor < gray && r_sensor > gray) {  // поворот вправо
            rightward();
        } else if (l_sensor < gray && r_sensor < gray) { // сход с линии
            search_time++;

            if (last_color > gray / 2) {
                out_of_line_l();  // Сход с линии влево

                if (search_time > 5000) {
                    stop();
                }
            } else {
                out_of_line_r();  // Сход с линии вправо

                if (search_time > 5000) {
                    stop();
                }
            }
        }
        // Воспроизведение звуковых сигналов каждые две секунды
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 1000) {
            previousMillis = currentMillis;
            playmusic();
    }
    }
    }
    
