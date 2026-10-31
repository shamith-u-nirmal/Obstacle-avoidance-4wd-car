#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <MPU6050.h>

// ---------------- MOTOR ----------------
#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 33
#define ENB 32

// ---------------- LINE SENSOR ----------------
#define LINE_LEFT 18
#define LINE_RIGHT 19

// ---------------- ULTRASONIC ----------------
#define FRONT_TRIG 4
#define FRONT_ECHO 34

#define LEFT_TRIG 16
#define LEFT_ECHO 35

#define RIGHT_TRIG 17
#define RIGHT_ECHO 39

// ---------------- ENCODERS ----------------
#define LEFT_ENCODER 13
#define RIGHT_ENCODER 23

// ---------------- DHT22 ----------------
#define DHTPIN 5
#define DHTTYPE DHT22

// ---------------- FEATURE TOGGLES ----------------
bool lineFollowEnabled = false;
bool obstacleAvoidEnabled = true;
bool climateEnabled = true;
bool mpuEnabled = true;
bool encoderEnabled = true;

// ---------------- VARIABLES ----------------
DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;

volatile long leftCount = 0;
volatile long rightCount = 0;

int baseSpeed = 90;   // slower cruising
int turnSpeed = 180;  // stronger turning torque

// ---------------- ENCODER INTERRUPTS ----------------
void IRAM_ATTR leftEncoderISR() {
  leftCount++;
}

void IRAM_ATTR rightEncoderISR() {
  rightCount++;
}

// ---------------- MOTOR CONTROL ----------------
void setSpeed(int left, int right) {
  ledcWrite(0, left);
  ledcWrite(1, right);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  setSpeed(baseSpeed, baseSpeed);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  setSpeed(baseSpeed, baseSpeed);
}

void stopBot() {
  setSpeed(0, 0);
}

// Arc turn left (less torque needed)
void turnLeft() {
  // Left side stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // Right side forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  setSpeed(0, turnSpeed);
}

// Arc turn right (less torque needed)
void turnRight() {
  // Left side forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right side stop
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  setSpeed(turnSpeed, 0);
}

// ---------------- ULTRASONIC ----------------
long readDistance(int trigPin, int echoPin) {
  long total = 0;
  int validReads = 0;

  for (int i = 0; i < 3; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);

    if (duration > 0) {
      total += duration * 0.034 / 2;
      validReads++;
    }

    delay(20);
  }

  if (validReads == 0) return -1;

  return total / validReads;
}

// ---------------- OBSTACLE AVOIDANCE ----------------
bool obstacleDetected() {
  long front = readDistance(FRONT_TRIG, FRONT_ECHO);
  long left  = readDistance(LEFT_TRIG, LEFT_ECHO);
  long right = readDistance(RIGHT_TRIG, RIGHT_ECHO);

  int threshold = 25;

  // If front blocked
  if (front > 0 && front < threshold) {

    stopBot();
    delay(200);

    // choose better side
    if (left > right) {
      turnLeft();
    } else {
      turnRight();
    }

    delay(700);
    stopBot();
    delay(150);

    return true;
  }

  // proactive side correction
  if (left > 0 && left < 10) {
    turnRight();
    delay(300);
    return true;
  }

  if (right > 0 && right < 10) {
    turnLeft();
    delay(300);
    return true;
  }

  return false;
}

// ---------------- LINE FOLLOW ----------------
bool lineDetected() {
  int left = digitalRead(LINE_LEFT);
  int right = digitalRead(LINE_RIGHT);

  // both sensors must agree
  return (left == 1 && right == 1);
}

void lineFollow() {
  int left = digitalRead(LINE_LEFT);
  int right = digitalRead(LINE_RIGHT);

  if (left == 1 && right == 0) {
    turnLeft();
  }
  else if (left == 0 && right == 1) {
    turnRight();
  }
  else {
    forward();
  }
}

// ---------------- DHT22 ----------------
void readClimate() {
  if (!climateEnabled) return;

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" C | Humidity: ");
    Serial.println(hum);
  }
}

// ---------------- MPU6050 ----------------
void readMPU() {
  if (!mpuEnabled) return;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("AX:");
  Serial.print(ax);
  Serial.print(" AY:");
  Serial.print(ay);
  Serial.print(" AZ:");
  Serial.println(az);
}

// ---------------- ENCODERS ----------------
void readEncoders() {
  if (!encoderEnabled) return;

  Serial.print("Left Encoder: ");
  Serial.print(leftCount);
  Serial.print(" | Right Encoder: ");
  Serial.println(rightCount);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);

  ledcAttachPin(ENA, 0);
  ledcAttachPin(ENB, 1);

  // Line sensors
  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);

  // Ultrasonic
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  // Encoders
  pinMode(LEFT_ENCODER, INPUT);
  pinMode(RIGHT_ENCODER, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER), rightEncoderISR, RISING);

  // I2C
  Wire.begin(21, 22);

  // MPU
  mpu.initialize();

  // DHT
  dht.begin();

  Serial.println("Hybrid Robot System Started");
}

// ---------------- LOOP ----------------
void loop() {

  // PRIORITY 1: obstacle avoidance
  if (obstacleDetected()) {
    delay(100);
    return;
  }

  // PRIORITY 2: line following (optional)
  if (lineFollowEnabled && lineDetected()) {
    lineFollow();
  }
  else {
    // default move forward
    forward();
  }

  // Logging
  readClimate();
  readMPU();
  readEncoders();

  delay(200);
}