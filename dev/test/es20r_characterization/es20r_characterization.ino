/*
  Data logger for ES 20r: Physics of Sports

  Writes requested reports to a text file (csv)
  and allows for configuration of requested sample rate

  modified 12 May 2022

  Active Learning Labs, Electrical Engineering
  https://www.seas.harvard.edu/active-learning-labs
*/

#include <SPI.h>
#include "SdFat.h"
#include <Adafruit_BNO08x.h>

// parameters

int sr_start = 10; // sample rate, in Hz    
int sr_stop = 210;
int sr_step = 10;
int sr = sr_start; // do not change

// characterization

char dimenNames[5] = {'a','l','g','m','r'};

const int numConfigs = 31;

bool dimenStates[numConfigs][5] = {{1,0,0,0,0}, 
                                   {0,1,0,0,0},
                                   {1,1,0,0,0},
                                   {0,0,1,0,0},
                                   {0,0,0,1,0},
                                   {0,0,0,0,1},
                                   {1,0,1,0,0},
                                   {0,1,1,0,0},
                                   {1,1,1,0,0},
                                   {1,0,0,1,0},
                                   {0,1,0,1,0},
                                   {1,1,0,1,0},
                                   {1,0,0,0,1},
                                   {0,1,0,0,1},
                                   {1,1,0,0,1},
                                   {0,0,1,1,0},
                                   {0,0,1,0,1},
                                   {0,0,0,1,1},
                                   {1,0,1,1,0},
                                   {0,1,1,1,0},
                                   {1,1,1,1,0},
                                   {0,0,1,1,1},
                                   {1,0,1,0,1},
                                   {0,1,1,0,1},
                                   {1,1,1,0,1},
                                   {1,0,0,1,1},
                                   {0,1,0,1,1},
                                   {1,1,0,1,1},
                                   {1,0,1,1,1},
                                   {0,1,1,1,1},
                                   {1,1,1,1,1}};
                                   

int dimenRates[numConfigs][5] = {{400,0,0,0,0}, 
                                 {0,400,0,0,0},
                                 {200,200,0,0,0},
                                 {0,0,250,0,0},
                                 {0,0,0,100,0},
                                 {0,0,0,0,250},
                                 {125,0,100,0,0},
                                 {0,100,100,0,0},
                                 {125,100,100,0,0},
                                 {125,0,0,100,0},
                                 {0,100,0,100,0},
                                 {125,100,0,100,0},
                                 {125,0,0,0,100},
                                 {0,100,0,0,100},
                                 {125,100,0,0,100},
                                 {0,0,100,100,0},
                                 {0,0,100,0,100},
                                 {0,0,0,100,100},
                                 {125,0,100,100,0},
                                 {0,100,100,100,0},
                                 {125,100,100,100,0},
                                 {0,0,100,100,100},
                                 {125,0,100,0,100},
                                 {0,100,100,0,100},
                                 {125,100,100,0,100},
                                 {125,0,0,100,100},
                                 {0,100,0,100,100},
                                 {125,100,0,100,100},
                                 {125,0,100,25,100},
                                 {0,100,100,25,100},
                                 {125,100,100,25,100}};




// pins

#define BNO08X_CS A4
#define BNO08X_INT A5
#define BNO08X_RESET 5
#define SWITCH A1
#define P0 9
#define P1 6

// other

#define us 1000000.0
#define FILE_BASE_NAME "data"
#define error(msg) sd.errorHalt(F(msg))

const uint8_t sd_chipSelect = 4;
unsigned long start_time;
bool recording = false;

Adafruit_BNO08x bno08x(BNO08X_RESET); // -1, no reset
sh2_SensorValue_t sensorValue;
SdFat sd;
SdFile file;

void setup() {

 pinMode(P0, OUTPUT);
 pinMode(P1, OUTPUT);
 // set BNO to SPI mode
 digitalWrite(P0, HIGH);
 digitalWrite(P1, HIGH);
  
  Serial.begin(115200);
//  while(!Serial){
//    delay(10);
//  }

  Serial.print("attempting to setup sd card...");
  if (!sd.begin(sd_chipSelect, SD_SCK_MHZ(12))) {
    sd.initErrorHalt();
  }

  Serial.print("attempting to setup IMU...");
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println(" failed to initialize BNO08x");
    while (1) {
      delay(10);
    }
  }
  Serial.println(" BNO08x initialized");

  //pinMode(record_en, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  bnoDetails();  
}

void bnoDetails(void) {
  for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
    Serial.print("Part ");
    Serial.print(bno08x.prodIds.entry[n].swPartNumber);
    Serial.print(": Version :");
    Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
    Serial.print(" Build ");
    Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }
}

void setReports(bool configState[], int configRate[]) {

  if (configState[0]){
    if (configRate[0]<0){
      if (!bno08x.enableReport(SH2_ACCELEROMETER, (int)us/sr)) {
        Serial.println("could not enable accelerometer (!)");
      }
    }
    else{
      if (!bno08x.enableReport(SH2_ACCELEROMETER, (int)us/configRate[0])) {
        Serial.println("could not enable accelerometer (!)");
      }
    }    
  }
  if (configState[1]){
    if (configRate[1]<0){
      if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION, (int)us/sr)) {
        Serial.println("could not enable accelerometer (!)");
      }
    }
    else{
      if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION, (int)us/configRate[1])) {
        Serial.println("could not enable accelerometer (!)");
      }
    }    
  }
  if (configState[2]){
    if (configRate[2]<0){
      if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/sr)) {
        Serial.println("could not enable gyroscope (!)");
      } 
    }
    else{
      if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/configRate[2])) {
        Serial.println("could not enable gyroscope (!)");
      } 
    }
  }
  if (configState[3]){
    if (configRate[3]<0){
      if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/sr)) {
        Serial.println("could not enable magnetometer (!)");
      }
    }
    else{
      if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/configRate[3])) {
        Serial.println("could not enable magnetometer (!)");
      }
    }
  }
  if (configState[4]){
    if (configRate[4]<0){
      if (!bno08x.enableReport(SH2_ROTATION_VECTOR, (int)us/sr)) {
        Serial.println("could not enable rotation vector");
      }
    }
    else{
      if (!bno08x.enableReport(SH2_ROTATION_VECTOR, (int)us/configRate[4])) {
        Serial.println("could not enable rotation vector");
      }
    }
  }
}

void loop() {
  while(digitalRead(SWITCH)){
    delay(500);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  for (byte i=0; i<numConfigs; i++){
    String fileName;
    for (byte j=0; j<5; j++){
      if(dimenStates[i][j]){
       fileName += String(dimenNames[j]);
       fileName += String(dimenRates[i][j]);
      }
    }
    Serial.println(fileName);
    fileName += ".csv";
    if (sd.exists(fileName.c_str())){
      sd.remove(fileName.c_str());
    }
    if (!file.open(fileName.c_str(), FILE_WRITE)) {
      error("file.open");
    }
    sr = sr_start;
    while(sr<sr_stop){
      bno08x.hardwareReset();
      delay(500);
      setReports(dimenStates[i], dimenRates[i]);
      Serial.println(sr);
      start_time = micros();
      while((micros()-start_time)/us < 5){
        String dataString = "";
        if (!bno08x.getSensorEvent(&sensorValue)){}
        else{
          switch (sensorValue.sensorId) {
            case SH2_ACCELEROMETER:
              dataString += "a,";
              dataString += String(sensorValue.un.accelerometer.x);
              dataString += ",";
              dataString += String(sensorValue.un.accelerometer.y);
              dataString += ",";
              dataString += String(sensorValue.un.accelerometer.z);
              dataString += ", ,";
              dataString += String((micros()-start_time)/us,3);
              file.println(dataString);
              break;
            case SH2_LINEAR_ACCELERATION:
              dataString += "l,";
              dataString += String(sensorValue.un.linearAcceleration.x);
              dataString += ",";
              dataString += String(sensorValue.un.linearAcceleration.y);
              dataString += ",";
              dataString += String(sensorValue.un.linearAcceleration.z);
              dataString += ", ,";
              dataString += String((micros()-start_time)/us,3);
              file.println(dataString);
              break;
            case SH2_GYROSCOPE_CALIBRATED:
              dataString += "g,";
              dataString += String(sensorValue.un.gyroscope.x);
              dataString += ",";
              dataString += String(sensorValue.un.gyroscope.y);
              dataString += ",";
              dataString += String(sensorValue.un.gyroscope.z);
              dataString += ", ,";
              dataString += String((micros()-start_time)/us,3);
              file.println(dataString);
              break;
            case SH2_MAGNETIC_FIELD_CALIBRATED:
              dataString += "m,";
              dataString += String(sensorValue.un.magneticField.x);
              dataString += ",";
              dataString += String(sensorValue.un.magneticField.y);
              dataString += ",";
              dataString += String(sensorValue.un.magneticField.z);
              dataString += ", ,";
              dataString += String((micros()-start_time)/us,3);
              file.println(dataString);
              break;
            case SH2_ROTATION_VECTOR:
              dataString += "r,";
              dataString += String(sensorValue.un.rotationVector.real);
              dataString += ",";
              dataString += String(sensorValue.un.rotationVector.i);
              dataString += ",";
              dataString += String(sensorValue.un.rotationVector.j);
              dataString += ",";
              dataString += String(sensorValue.un.rotationVector.k);
              dataString += ",";
              dataString += String((micros()-start_time)/us,3);
              file.println(dataString);
              break;
            default:
              break;
          }   
        }
      }
      sr = sr + sr_step;
    }
    file.close();
  }
  while(1){
    digitalWrite(LED_BUILTIN,LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
  }
}
