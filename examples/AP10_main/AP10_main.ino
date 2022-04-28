/*
   AP10_main.ino - Primary Sketch for AP10 Sensing Platform
   Created by Bejamin Y. Brown and J. Evan Smith
   Active Learning Labs

   last revised 28 April 2022
*/

#include<AP10.h>

/*
   Additional dependencies include:

   (1) SdFat.h
   (2) Adafruit_BNO08x.h
   (3) Adafruit_NeoPixel.h

   Use 'Tools -> Manage Libraries' to install accordingly
*/

AP10 sensor;
//sensor.configure[4] = {'a','g','m','r'}; // enable as required

/*
   a = accelerometer (angular)
   g = gyroscope (angular velocity) 
   m = magnetometer (B fields)
   r = rotation vector (orientation)

   if above letters are passed to configure -> measured
   else, replace with character 'x'
*/

void setup() { sensor.begin(); } 
void loop() { sensor.record(); }
