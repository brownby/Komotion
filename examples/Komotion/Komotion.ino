/*
   Komotion.ino - Primary Sketch for Komotion, the Motion Sensing Platform for ES20r
   Created by Benjamin Y. Brown and J. Evan Smith

   last revised 6 September 2022
*/

#include <Komotion.h>

Komotion sensor;
char configure[] = "lgr"; // enable as required

/*
   a = accelerometer
   l = linear acceleration (gravity removed, usually want to use this!)
   g = gyroscope (angular velocity) 
   m = magnetometer (B fields)
   r = rotation vector (orientation)

   if above letters are passed to configure -> measured
*/

void setup() { sensor.begin(configure, false); } 
void loop() { sensor.record(); }
