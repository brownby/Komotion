/*
   ES20r_main.ino - Primary Sketch for AP10 Sensing Platform
   Created by Bejamin Y. Brown and J. Evan Smith
   Active Learning Labs

   last revised 28 April 2022
*/

#include "ES20r.h"

/*
   Additional dependencies include:

   (1) SdFat.h
   (2) Adafruit_NeoPixel.h
   (3) ArduinoLowPower.h

   Use 'Tools -> Manage Libraries' to install accordingly
*/

ES20r sensor;
char configure[] = "agmr"; // enable as required

/*
   a = accelerometer (angular)
   g = gyroscope (angular velocity) 
   m = magnetometer (B fields)
   r = rotation vector (orientation)

   if above letters are passed to configure -> measured
*/

void setup() { sensor.begin(configure, false); } 
void loop() { sensor.record(); }
