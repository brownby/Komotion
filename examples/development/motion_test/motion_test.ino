/*
  Data logger for ES 20r: Physics of Sports

  modified 13 May 2022
  by J. Evan Smith

  Active Learning Labs, Electrical Engineering
  https://www.seas.harvard.edu/active-learning-labs
*/

#include <Servo.h>

#define panPin 6
#define tiltPin 7
#define switchPin 5

Servo pan;
Servo tilt;

void setup() {
  pinMode(switchPin,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pan.attach(panPin);
  tilt.attach(tiltPin);

  pan.writeMicroseconds(1500);
  tilt.writeMicroseconds(1500);
}

void loop() {
  if (digitalRead(switchPin)){
    digitalWrite(LED_BUILTIN,HIGH);
    int newPan = random(1050,1950);
    int newTilt = random(1050,1950);
    pan.writeMicroseconds(newPan);
    tilt.writeMicroseconds(newTilt);
    delay(200);
  }
  else{
    digitalWrite(LED_BUILTIN,LOW);
  }
}
