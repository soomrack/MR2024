#include <stdint.h>


#define PIN_PWM_LEFT 6
#define PIN_PWM_RIGHT 5
#define PIN_DIR_LEFT 7
#define PIN_DIR_RIGHT 4
#define PIN_BUTTON A2
#define PIN_BUZZER 9

const uint8_t PINS_ANALOG[] = {A0, A1};
const uint8_t SENSORS_NUM = sizeof(PINS_ANALOG);

//PD regilator coefficients
const int BASE_SPEED = 100;
const float K_P = 7.0;
const float K_D = 30.0;




//driving speed limits
const int ABS_SPEED_MAX = 250;
const int ABS_SPEED_MIN = -100;


// SEEK and other constants
const int DEBOUNCE_DELAY_MS = 5;
const int U_SEEK_LINE = 140;
const unsigned long SEEK_TIME = 7000;
const int BLACK_THRESHOLD = 50;

struct Limit
{
    int min;
    int max;

    Limit()
    {
        min = 1024;
        max = 0;
    }
};


Limit calibration_data[SENSORS_NUM];
int sensors_data_raw[SENSORS_NUM];
int sensors_data[SENSORS_NUM];


void pins_init()
{
    pinMode(PIN_PWM_LEFT, OUTPUT);
    pinMode(PIN_DIR_LEFT, OUTPUT);
    pinMode(PIN_PWM_RIGHT, OUTPUT);
    pinMode(PIN_DIR_RIGHT, OUTPUT);

    pinMode(PIN_BUTTON, INPUT);

    pinMode(PIN_BUZZER, OUTPUT);
}


void motors_drive(int left, int right)
{
    left = constrain(left, -(int)UINT8_MAX, (int)UINT8_MAX);
    right = constrain(right, -(int)UINT8_MAX, (int)UINT8_MAX);

    digitalWrite(PIN_DIR_LEFT, left < 0);
    digitalWrite(PIN_DIR_RIGHT, right < 0);

    analogWrite(PIN_PWM_LEFT, abs(left));
    analogWrite(PIN_PWM_RIGHT, abs(right));
}


bool button_is_pressed()
{
    static bool old_state = 0;
    static bool is_pressed = 0;
    static unsigned long timeout_end = 0;


    bool state = digitalRead(PIN_BUTTON);

    if(!is_pressed) {
        if(state && !old_state) { //positive edge detection
            is_pressed = 1;
            timeout_end = millis() + DEBOUNCE_DELAY_MS;
        }
        old_state = state;
    }
    else {
        if(millis() > timeout_end) {
            is_pressed = 0;
            if(state) return 1;
        }
    }

    return 0;
}


void sensors_update_raw()
{
    for(uint8_t idx = 0; idx < SENSORS_NUM; idx++) {
        sensors_data_raw[idx] = analogRead(PINS_ANALOG[idx]);
    }
}


void sensors_update()
{
    sensors_update_raw();

    for(uint8_t idx = 0; idx < SENSORS_NUM; idx++)
        //make sensors output range 0 - 100
        sensors_data[idx] = map(sensors_data_raw[idx], calibration_data[idx].min, calibration_data[idx].max, 0, 100);
}


bool is_on_line()
{
    bool line = 0;

    for(uint8_t idx = 0; idx < SENSORS_NUM; idx++) {
        if(sensors_data[idx] > BLACK_THRESHOLD) line = 1;
    }

    return line;
}


void sensors_calibrate()
{
    sensors_update_raw();

    // update limits
    for(uint8_t idx = 0; idx < SENSORS_NUM; idx++) {
        if(sensors_data_raw[idx] < calibration_data[idx].min) {
            calibration_data[idx].min = sensors_data_raw[idx];
        }

        if(sensors_data_raw[idx] > calibration_data[idx].max) {
            calibration_data[idx].max = sensors_data_raw[idx];
        }
    }
}


void calibrate()
{
    unsigned long time_end = millis() + 4000;
    motors_drive(-120, 120); //start spin
    while(millis() < time_end)
        sensors_calibrate();
    motors_drive(0, 0); // stop
}


bool line_seek(int* u)
{
    static unsigned long time_start_seek = 0;
    static int u_old_sign = 0;

    if(!is_on_line()) {
        if(time_start_seek == 0) {
            time_start_seek = millis();
        }

        *u = u_old_sign * U_SEEK_LINE * expf(-(float)(millis() - time_start_seek) / 2000.0f);

        if((millis() - time_start_seek) > SEEK_TIME) return 0;
    }
    else {
        time_start_seek = 0;
    }
    u_old_sign = (*u > 0) ? 1 : -1;

    return 1;
}


int PD_calc()
{
    static float err_old = 0.0f;

    float err = sensors_data[0] - sensors_data[1];
    float u = err * K_P + (err - err_old) * K_D;
    err_old = err;

    return u;
}


bool line()
{
    sensors_update();

    int regulation = PD_calc();
    if(!line_seek(&regulation)) return 0;

    motors_drive(constrain(BASE_SPEED + regulation, ABS_SPEED_MIN, ABS_SPEED_MAX),
        constrain(BASE_SPEED - regulation, ABS_SPEED_MIN, ABS_SPEED_MAX));
    return 1;
}


bool bzz()
{
    static unsigned long start_time = 0;

    if(start_time == 0)
        start_time = millis();

    if((millis() - start_time) > 5100) {
        start_time = 0;
        return 0;
    }

    if((millis() - start_time) % 2000 < 1000)
        analogWrite(PIN_BUZZER, 128);
    else
        digitalWrite(PIN_BUZZER, 0);

    return 1;
}


void setup()
{
    pins_init();

    calibrate();
}


void loop()
{
    static bool is_running = 0;
    static bool is_bzzing = 0;

    if(is_running) {
        if(!line()) {
            is_running = 0;
            is_bzzing = 1;
        }
    } else
        motors_drive(0, 0);

    if(is_bzzing)
        is_bzzing = bzz();

    if(button_is_pressed())
        is_running = !is_running;

    delay(1);
}
