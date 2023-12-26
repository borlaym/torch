#include <Servo.h>

/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 10 to ground through 220 ohm resistor
  - pushbutton attached to pin 7 from +5V
  - 10K resistor attached to pin 7 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 10.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin
const int pauseButtonPin = 8;
const int ledPin =  10;      // the number of the LED pin
const int motorPin = 5;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int pauseButtonState = 0;
int led = 0;
int lastMotorWrite = 0;
unsigned long lastMotorWriteTimestamp = 0UL;

unsigned long time = 0UL;           // the last time the output pin was toggled
unsigned long pauseButtonTime = 0UL;
unsigned long debounce = 200UL;
unsigned long lastTime = 0UL;

unsigned long startTime = 0UL;  // Az időmérés kezdőpontja
unsigned long elapsedTime = 0UL;  // Az eltelt idő
unsigned long targetTime = 6000UL;  // Célidő (1 óra)
unsigned long motorDebounce = 15000UL;
bool timerRunning = false;
bool isPaused = false;
bool motorOn = false;

Servo myServo;

void setup() {
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(pauseButtonPin, INPUT);
  pinMode(motorPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long loopTime = millis();
  // read the state of the pushbutton value:
  int newButtonState = digitalRead(buttonPin);
  if (buttonState == LOW && newButtonState == HIGH && loopTime - time > debounce) {
      // Button was pressed
      if (!timerRunning) {
        timerRunning = true;
        startTime = loopTime;
        lastTime = loopTime;
        elapsedTime = 0;
      } else {
        timerRunning = false;
      }
      time = loopTime;
  }

  // read the state of the pause button value:
  int newPauseButtonState = digitalRead(pauseButtonPin);
  if (timerRunning && pauseButtonState == LOW && newPauseButtonState == HIGH && loopTime - pauseButtonTime > debounce) {
      // Button was pressed
      if (!isPaused) {
        isPaused = true;
      } else {
        isPaused = false;
        lastTime = loopTime;
      }
      pauseButtonTime = loopTime;
  }

  if (timerRunning && !isPaused) {
    
    unsigned long timeSinceLastLoop = loopTime - lastTime;
//    Serial.println(String(timeSinceLastLoop) + "=" + String(loopTime) + "-" + String(lastTime));
    elapsedTime = elapsedTime + timeSinceLastLoop;
//    Serial.println(elapsedTime);
    lastTime = loopTime;
 
    
    
    // Serial.println(String(elapsedTime) + " " + String(targetTime) + " " + (elapsedTime >= targetTime ? "stop" : "go"));

    // Ha lejárt az idő
    if (elapsedTime >= targetTime) {
      timerRunning = false;
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(ledPin, HIGH);
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
  
  // Motor
  if (timerRunning && !isPaused) {
    int angle = map(elapsedTime, 0, targetTime, 0, 180);

    // Start of the cycle
    if (elapsedTime < 200) {
//      Serial.println("Motor setup");
      myServo.attach(motorPin);
    }

    if (elapsedTime > 200 && elapsedTime < 1000) {
      myServo.write(0);
    }

    if (elapsedTime > 1000 && elapsedTime < motorDebounce) {
//      Serial.println("Motor setup ended");
      myServo.detach();
    }

    // Turn on the motor every X seconds
    if (loopTime - lastMotorWriteTimestamp > motorDebounce && !motorOn) {
//      Serial.println("MOTOR start");
      myServo.attach(motorPin);
      motorOn = true;
    } else if (lastMotorWrite != angle && loopTime - lastMotorWriteTimestamp > (motorDebounce + 100UL)) {
//      Serial.println("MOTOR " + String(angle));
//      Serial.println("TIME " + String(elapsedTime));
      lastMotorWrite = angle;
      myServo.write(angle);
     
    } else if (loopTime - lastMotorWriteTimestamp > (motorDebounce + 500UL)) {
//      Serial.println("MOTOR detach");
      lastMotorWriteTimestamp = loopTime;
       myServo.detach();
       motorOn = false;
    }
  } else {
    myServo.detach();
  }

  buttonState = newButtonState;
  pauseButtonState = newPauseButtonState;
}
