#define PIN_DIR_R 4
#define PIN_DIR_L 7
#define PIN_PWR_R 5
#define PIN_PWR_L 6
#define IR_SENSOR_PIN A2

#define SAFE_DISTANCE 10.0
#define RESUME_DISTANCE 12.0
#define SAMPLES 5

unsigned long lastCommandTime = 0;
const unsigned long AUTO_STOP_TIMEOUT = 1000;
char lastCommand = 'X';
bool isMoving = false;
bool isBlocked = false;
const unsigned long CONNECTION_LOST_TIMEOUT = 600;
bool connectionLost = false;

void setup()
{
  pinMode(PIN_DIR_R, OUTPUT);
  pinMode(PIN_DIR_L, OUTPUT);
  pinMode(PIN_PWR_R, OUTPUT);
  pinMode(PIN_PWR_L, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);

  stopMotors();

  Serial.begin(9600);
  delay(100);

  while (Serial.available())
  {
    Serial.read();
  }

  lastCommandTime = millis();
  Serial.println("Arduino Robot Ready - IR Safety Stop < 10cm");
}

void loop()
{
  float distance = readDistance();

  if (Serial.available() > 0)
  {
    char command = Serial.read();
    executeCommand(command);
    lastCommandTime = millis();
    isMoving = (command != 'X');

    connectionLost = false;
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200)
  {
    lastPrint = millis();
    Serial.print("DIST:");
    Serial.print(distance);
    Serial.println(":END");
  }

  if (lastCommand != 'X' && millis() - lastCommandTime > CONNECTION_LOST_TIMEOUT)
  {
    if (!connectionLost)
    {
      connectionLost = true;
      Serial.println("CONNECTION LOST! Searching...");
    }
    searchForSignal(distance);
  }
  else
  {
    connectionLost = false;
  }

  if (isMoving && lastCommand == 'W' && distance < SAFE_DISTANCE)
  {
    emergencyStop();
    isBlocked = true;
    Serial.println("SAFETY STOP: Obstacle < 10cm");
  }

  if (isBlocked && distance > RESUME_DISTANCE)
  {
    isBlocked = false;
    Serial.println("OBSTACLE CLEARED");
  }

  if (!connectionLost && isMoving && (millis() - lastCommandTime > AUTO_STOP_TIMEOUT))
  {
    stopMotors();
    isMoving = false;
    Serial.println("AutoStop: No command received");
  }

  delay(10);
}

float readDistance()
{
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    sum += analogRead(IR_SENSOR_PIN);
    delay(2);
  }
  float avgValue = sum / (float)SAMPLES;

  float voltage = avgValue * (5.0 / 1023.0);

  if (voltage <= 0.18)
    return 30.0;

  float distance = 13.15 / (voltage - 0.15);

  if (distance < 4.0)
    distance = 4.0;
  if (distance > 30.0)
    distance = 30.0;

  return distance;
}

void executeCommand(char cmd)
{
  lastCommand = cmd;

  switch (cmd)
  {
  case 'W':
    moveForward();
    break;
  case 'S':
    moveBackward();
    break;
  case 'A':
    turnLeft();
    break;
  case 'D':
    turnRight();
    break;
  case 'X':
    stopMotors();
    break;
  default:
    break;
  }
}

void moveForward()
{
  if (isBlocked)
  {
    stopMotors();
    return;
  }

  digitalWrite(PIN_DIR_R, HIGH);
  digitalWrite(PIN_DIR_L, HIGH);
  analogWrite(PIN_PWR_R, 200);
  analogWrite(PIN_PWR_L, 200);
}

void moveBackward()
{
  digitalWrite(PIN_DIR_R, LOW);
  digitalWrite(PIN_DIR_L, LOW);
  analogWrite(PIN_PWR_R, 200);
  analogWrite(PIN_PWR_L, 200);
}

void turnLeft()
{
  digitalWrite(PIN_DIR_R, HIGH);
  digitalWrite(PIN_DIR_L, LOW);
  analogWrite(PIN_PWR_R, 200);
  analogWrite(PIN_PWR_L, 200);
}

void turnRight()
{
  digitalWrite(PIN_DIR_R, LOW);
  digitalWrite(PIN_DIR_L, HIGH);
  analogWrite(PIN_PWR_R, 200);
  analogWrite(PIN_PWR_L, 200);
}

void stopMotors()
{
  analogWrite(PIN_PWR_R, 0);
  analogWrite(PIN_PWR_L, 0);
  isMoving = false;
}

void emergencyStop()
{
  stopMotors();
  lastCommand = 'X';
}

void searchForSignal(float frontDistance)
{
  if (frontDistance < 8.0)
  {
    stopMotors();
    return;
  }

  digitalWrite(PIN_DIR_R, LOW);
  digitalWrite(PIN_DIR_L, LOW);
  analogWrite(PIN_PWR_R, 170);
  analogWrite(PIN_PWR_L, 170);
  delay(600);

  stopMotors();
  delay(200);

  digitalWrite(PIN_DIR_R, LOW);
  digitalWrite(PIN_DIR_L, HIGH);
  analogWrite(PIN_PWR_R, 150);
  analogWrite(PIN_PWR_L, 150);
  delay(400);

  stopMotors();
}