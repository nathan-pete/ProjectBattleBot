#include <Servo.h>

// Pin definitions 
const int leftWheelBack = 8;
const int leftWheelFront = 4;
const int rightWheelBack = 5;
const int rightWheelFront = 6;
const int leftWheelSpeedPin = 9; 
const int rightWheelSpeedPin = 10; 
const int sensorPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int trigPin = 7;
const int echoPin = 13;
const int GRIPPER_PIN = 12;

int blackBoxDetectedStart = 0;
int blackBoxWaitTime = 40;
int timerStart = 0;
bool timerRunning = false;
const int timerDuration = 25;

Servo servoGripper;

// Setup function
void setup() {
  pinMode(leftWheelBack, OUTPUT);
  pinMode(leftWheelFront, OUTPUT);
  pinMode(rightWheelBack, OUTPUT);
  pinMode(rightWheelFront, OUTPUT);
  pinMode(leftWheelSpeedPin, OUTPUT);
  pinMode(rightWheelSpeedPin, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  servoGripper.attach(GRIPPER_PIN);
  servoGripper.write(125); 

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

// Main program loop
void loop() {
  int sensorValues[8];
  
  for (int i = 0; i < 8; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
  }

  if (sensorValues[3] > 500 && sensorValues[4] > 500) {
    moveForward();
    startTimer();
  } else if (sensorValues[2] > 500) {
    turnRight();
  } else if (sensorValues[5] > 500) {
    turnLeft();
  } else {
    blackBoxDetectedStart = millis();
    if (millis() - blackBoxDetectedStart > blackBoxWaitTime) {
      controlGripper(125);
      stopMotors(); 
    }
  }
  checkTimer();

  if (isObjectDetected()) {
    controlGripper(45); 
  } else {
    controlGripper(125); 
  }
}

void moveForward() {
  digitalWrite(leftWheelFront, HIGH);
  digitalWrite(rightWheelFront, HIGH);
  digitalWrite(leftWheelBack, LOW);
  digitalWrite(rightWheelBack, LOW);
}

void turnRight() {
  digitalWrite(leftWheelFront, HIGH);
  digitalWrite(rightWheelFront, LOW);
  digitalWrite(leftWheelBack, LOW);
  digitalWrite(rightWheelBack, LOW);
}

void turnLeft() {
  digitalWrite(leftWheelFront, LOW);
  digitalWrite(rightWheelFront, HIGH);
  digitalWrite(leftWheelBack, LOW);
  digitalWrite(rightWheelBack, LOW);
}

void stopMotors() {
  digitalWrite(leftWheelFront, LOW);
  digitalWrite(rightWheelFront, LOW);
  digitalWrite(leftWheelBack, LOW);
  digitalWrite(rightWheelBack, LOW);
}

void startTimer() {
  timerStart = millis();
  timerRunning = true;
}

void checkTimer() {
  if (timerRunning && (millis() - timerStart >= timerDuration)) {
    // Timer has elapsed, stop the motors
    stopMotors();
    timerRunning = false;
  }
}

void controlGripper(int position) {
  servoGripper.write(position);
}

bool isObjectDetected() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2.5); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; // Convert to cm

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance <= 5) {
    return true;
  } else {
    return false; 
  }
}
