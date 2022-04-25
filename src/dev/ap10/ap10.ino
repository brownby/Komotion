/*
  Data logger for AP10: Physics of Sports

  Writes requested reports to a text file (csv)
  and allows for configuration of requested sample rate

  modified 14 April 2022
  by J. Evan Smith

  Active Learning Labs, Electrical Engineering
  https://www.seas.harvard.edu/active-learning-labs
*/

#include <SPI.h>
#include "SdFat.h"
#include <Adafruit_BNO08x.h>

// parameters

int sr_start = 10; // sample rate, in Hz    
int sr_stop = 200;
int sr_step = 10;

int sr = sr_start; // do not change

// pins

#define BNO08X_CS 10
#define BNO08X_INT 9
#define BNO08X_RESET 5

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
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";
  
  Serial.begin(115200);
  while(!Serial){
    delay(10);
  }

  Serial.print("attempting to setup sd card...");
  if (!sd.begin(sd_chipSelect, SD_SCK_MHZ(12))) {
    sd.initErrorHalt();
  }
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_WRONLY | O_CREAT | O_EXCL)) {
    error("file.open");
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

  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  bnoDetails();
  setReports();  
  start_time = micros();
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

void setReports(void) {
  if (!bno08x.enableReport(SH2_ACCELEROMETER, (int)us/250)) {
    Serial.println("could not enable accelerometer (!)");
  } 
//  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/100)) {
//    Serial.println("could not enable gyroscope (!)");
//  } 
//  if(!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/100)) {
//    Serial.println("could not enable magnetometer (!)");
//  }
  if(!bno08x.enableReport(SH2_ROTATION_VECTOR, (int)us/100)) {
    Serial.println("could not enable rotation vector");
  }
}

void loop() {

  if (bno08x.wasReset()) {
    //Serial.println("sensor was reset ...");
    setReports();
  }
  
  if (sr<sr_stop){
    if ((micros()-start_time)/us > 5){
      sr = sr + sr_step;
      Serial.println(sr);
//      bno08x.hardwareReset();
      delay(1000);
      start_time = micros();
    }
    else{
      String dataString = "";
      if (!bno08x.getSensorEvent(&sensorValue)) { return; }
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
  else{
    file.close();
    while(1){
      digitalWrite(LED_BUILTIN,LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN,HIGH);
      delay(1000);
    }
  }
}
