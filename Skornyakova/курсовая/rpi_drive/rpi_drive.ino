#define LEFT_PWM  6
#define LEFT_DIR  7
#define RIGHT_PWM 5
#define RIGHT_DIR 4

const float Kp = 5.0;
const float Ki = 0.0;
const float Kd = 4.0;

float prev_error_x = 0, integral_x = 0;


const int base_speed = 100;//скорость изменяется в окрестности этого числа в зависимости от ошибки 
unsigned long lastUpdate = 0;
const unsigned long timeout = 1000;  // Если нет сигнала 1 секунду — остановить

void setup() {
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(LEFT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Tracker ready");
}

void setMotorSpeed(int left, int right) {
  digitalWrite(LEFT_DIR, left >= 0);
  digitalWrite(RIGHT_DIR, right >= 0);
  analogWrite(LEFT_PWM, constrain(abs(left), 0, 200));
  analogWrite(RIGHT_PWM, constrain(abs(right), 0, 200));
}

void stopMotors() {
  analogWrite(LEFT_PWM, 0);
  analogWrite(RIGHT_PWM, 0);
}

void loop() {
  //setMotorSpeed(80, 240);
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    int x_pos = data.indexOf('X');
    int y_pos = data.indexOf('Y');
    int R_pos = data.indexOf('R');
    int comma_pos = data.indexOf(',');

    if (x_pos == 0 && comma_pos > 1 && y_pos > comma_pos) {
       float error_x = data.substring(x_pos + 1, comma_pos).toFloat(); // "124"
       float error_y = data.substring(y_pos + 1, R_pos-1).toFloat();     // "141"
       float radius = data.substring(R_pos + 1).toFloat();            // "200"

      if (radius >= 220){
        stopMotors();
      }
      else {
        // PID по X
        float P_x = Kp * error_x;
        integral_x += error_x;
        float I_x = Ki * integral_x;
        float D_x = Kd * (error_x - prev_error_x);
        prev_error_x = error_x;
  
        float correction_x = P_x + I_x + D_x;
    
  
        int left_speed = base_speed - correction_x;
        int right_speed = base_speed + correction_x;
  
        left_speed = constrain(left_speed, 70, 200);
        right_speed = constrain(right_speed, 70, 200);
  
        setMotorSpeed(left_speed, right_speed);
  
        lastUpdate = millis();  // обновляем таймер
      }
    }
  }

  // Если долго нет данных — стоп
  if (millis() - lastUpdate > timeout) {
    stopMotors();
  }
}
