#include <stdint.h>


const uint8_t PIN_PWM_LEFT = 6;
const uint8_t PIN_DIR_LEFT = 7;
const uint8_t PIN_PWM_RIGHT = 5;
const uint8_t PIN_DIR_RIGHT = 4;
const uint8_t PIN_BUTTON = A0;
const uint8_t PINS_ANALOG[3] = {A3, A2, A1};


const int DEBOUNCE_DELAY_MS = 5;


//PD regilator coefficients
const float K_P = 5.0;
const float K_D = 20.0;


const int SPEED = 190; //basic movement speed


//driving speed limits
const int SPEED_MAX = 250;
const int SPEED_MIN = -100;


const int U_SEEK_LINE = 100; //u when seeking line


const int BLACK_THRESHOLD = 40; //threshold for detecting black line


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


Limit calibration_data[3];
int sensors_data_raw[3];
int sensors_data[3];


void pins_init()
{
    pinMode(PIN_PWM_LEFT, OUTPUT);
    pinMode(PIN_DIR_LEFT, OUTPUT);
    pinMode(PIN_PWM_RIGHT, OUTPUT);
    pinMode(PIN_DIR_RIGHT, OUTPUT);

    pinMode(PIN_BUTTON, INPUT);
}


void motors_set_speed(int left, int right)
{
    left = constrain(left, -(int)UINT8_MAX, (int)UINT8_MAX);
    right = constrain(right, -(int)UINT8_MAX, (int)UINT8_MAX);

    digitalWrite(PIN_DIR_LEFT, left > 0);
    digitalWrite(PIN_DIR_RIGHT, left > 0);

    analogWrite(PIN_PWM_LEFT, abs(left));
    analogWrite(PIN_PWM_RIGHT, abs(right));
}


//check button is pressed with bounce protection
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


void sensors_read_raw()
{
    for(uint8_t idx = 0; idx < 3; idx++) {
        sensors_data_raw[idx] = analogRead(PINS_ANALOG[idx]);
    }
}


void sensors_calibrate()
{
    sensors_read_raw();

    //update limits
    for(uint8_t idx = 0; idx < 3; idx++) {
        if(sensors_data_raw[idx] < calibration_data[idx].min) {
            calibration_data[idx].min = sensors_data_raw[idx];
        }

        if(sensors_data_raw[idx] > calibration_data[idx].max) {
            calibration_data[idx].max = sensors_data_raw[idx];
        }
    }
}


void calibration()
{
    unsigned long time_end = millis() + 4000;

    motors_set_speed(120, -120); //start spinning

    while(millis() < time_end) {
        sensors_calibrate();
    }

    motors_set_speed(0, 0); //stop
}


void sensors_read()
{
    sensors_read_raw();

    for(uint8_t idx = 0; idx < 3; idx++) {
        //make sensors output range 0 - 100
        sensors_data[idx] = map(sensors_data_raw[idx], calibration_data[idx].min, calibration_data[idx].max, 0, 100);
    }
}


void setup()
{
    pins_init();

    calibration();
}


bool is_on_line()
{
    bool line = 0;

    for(uint8_t idx = 0; idx < 3; idx++) {
        if(sensors_data[idx] < BLACK_THRESHOLD) line = 1;
    }

    return line;
}


int regulator()
{
    static int u_old = 0;
    static float err_old = 0.0f;

    float err = sensors_data[0] - sensors_data[1];
    float u = err * K_P + (err - err_old) * K_D;
    err_old = err;

    if(!is_on_line()) return (u_old > 0) ? U_SEEK_LINE : -U_SEEK_LINE;

    u_old = u;

    return u;
}


void drive_line()
{
    sensors_read();

    int u = regulator();

    int left_speed = constrain(SPEED + u, SPEED_MAX, -SPEED_MIN);
    int right_speed = constrain(SPEED - u, SPEED_MAX, -SPEED_MIN);
    motors_set_speed(left_speed, right_speed);
}


void loop()
{
    static bool is_running = 0;

    if(is_running) {
        drive_line();
    }
    else {
        motors_set_speed(0, 0);
    }

    if(button_is_pressed()) {
        is_running = !is_running;
    }

    delay(1);
}
