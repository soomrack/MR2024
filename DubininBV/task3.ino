#define LEFT_DIR                  4
#define LEFT_PWM                  5
#define RIGHT_DIR                 7
#define RIGHT_PWM                 6
#define SOUND                     A3
#define BUTTON                    A2
#define LEFT_SENSOR               A0
#define RIGHT_SENSOR              A1


const int black_color = 650;
int speed = 100;
int sensor_data[2];
static bool button_state = 0;


void pin_init()
{
  pinMode(LEFT_DIR, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(SOUND, OUTPUT);
  pinMode(BUTTON, INPUT);
}


void set_motor_speed(int speed_left, int speed_right)
{
    digitalWrite(LEFT_DIR, speed_left < 0);
    digitalWrite(RIGHT_DIR, speed_right < 0);

    analogWrite(LEFT_PWM, abs(speed_left));
    analogWrite(RIGHT_PWM, abs(speed_right));
}


void move_forward(const unsigned int speed)
{
    set_motor_speed(speed, speed);
}


void turn_right(const unsigned int speed_left ,const unsigned int speed_right)
{
    set_motor_speed(speed_left, -speed_right);
}


void turn_left(const int speed_left ,const int speed_right)
{
    set_motor_speed(-speed_left, speed_right);
}


void sensor_read()
{
    sensor_data[0] = analogRead(LEFT_SENSOR);
    sensor_data[1] = analogRead(RIGHT_SENSOR);
}


void stop()
{
    set_motor_speed(0, 0);
}


void search_line() {
    int speed = 100;
    unsigned long time = millis();

    while (analogRead(A0) < 650 && analogRead(A1) < 650) {
        unsigned long timel = millis();
        while (millis() - timel < 50) {
            turn_left(255,255);
        }
        unsigned long timef = millis();
            while (millis() - timef < 40){
                move_forward(speed);
            }

        speed = speed + 2;

        if (speed >= 170) {speed = 170;}
        delay(100);

        if (millis() - time > 7000) {
            while (1) {
                stop();
            }
        }
    }
}


void play_sound() {
    const unsigned long sound_duration = 500; // Duration of the sound in milliseconds
    const unsigned long start_time = millis();

    while (millis() - start_time < sound_duration) {
        digitalWrite(SOUND, HIGH);
    }

    digitalWrite(SOUND, LOW);
}


bool button_is_pressed()
{
    static bool old_state = 0;
    static bool is_pressed = 0;
    static unsigned long timeout_end = 0;


    bool state = digitalRead(BUTTON);

    if(!is_pressed) {
        if(state && !old_state) {
            is_pressed = 1;
            timeout_end = millis() + 5;
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


void setup()
{
    pin_init();
    Serial.begin(9600);
}


void loop() {
  if (button_state) {
      sensor_read();
      if (sensor_data[0] > black_color && sensor_data[1] > black_color) {
        move_forward(180);
      } 
      else if (sensor_data[0] > black_color && sensor_data[1] < black_color) {
        turn_left(230, 160);
      } 
      else if (sensor_data[0] > black_color && sensor_data[1] < black_color) {
        turn_right(230, 160);
      } 
      else {
        play_sound();
        search_line();
        delay(5);
      }
  }
  else {
      set_motor_speed(0, 0);
  }

  if (button_is_pressed()) {
      button_state = !button_state;
  }
}
