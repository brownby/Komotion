/*
  Data logger for AP10: Physics of Sports

  Writes requested reports to a text file (csv)
  and allows for configuration of requested sample rate

  modified 12 April 2022
  by J. Evan Smith

  Active Learning Labs, Electrical Engineering
  https://www.seas.harvard.edu/active-learning-labs
*/

#include <SPI.h>
#include "SdFat.h"
#include <Adafruit_BNO08x.h>

#define recon A0
#define sr 100       
#define us 1000000.0
#define BNO08X_CS 10
#define BNO08X_INT 9
#define BNO08X_RESET 5
#define FILE_BASE_NAME "Data"
#define error(msg) sd.errorHalt(F(msg))

const uint8_t chipSelect = 4;
const uint32_t SAMPLE_INTERVAL_MS = 10;

unsigned long tic;
char dataBuffer[40];
bool recording = false;
unsigned long start_time;

Adafruit_BNO08x bno08x(BNO08X_RESET); // -1, no reset
sh2_SensorValue_t sensorValue;

SdFat sd;
SdFile file;
uint32_t logTime;

void setup() {
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";
  
  Serial.begin(115200);
  while(!Serial){
    delay(10);
  }
  //
  Serial.print("attempting to setup sd card...");
  if (!sd.begin(chipSelect, SD_SCK_MHZ(12))) {
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

  //
  Serial.print("attempting to setup IMU...");
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println(" failed to initialize BNO08x");
    while (1) {
      delay(10);
    }
  }
  Serial.println(" BNO08x initialized");
  //
  pinMode(recon, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  //
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  bnoDetails();
  setReports();

  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;
  
  digitalWrite(LED_BUILTIN, LOW);
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
  Serial.println("setting desired reports ... ");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.println("could not enable accelerometer (!)");
  } 
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
    Serial.println("Could not enable gyroscope (!)");
  } 
  if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
    Serial.println("Could not enable magnetic field calibrated (!)");
  }
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) {
    Serial.println("Could not enable rotation vector");
  }
  Serial.println("reports set");
  Serial.println("in standby ...");
}

void loop() {

  logTime += 1000UL*SAMPLE_INTERVAL_MS;
  String dataString = "";
  delayMicroseconds(10); // TODO
  //
  if (bno08x.wasReset()) {
    Serial.println("sensor was reset ...");
    Serial.println("in standby ...");
    setReports();
  }
  //
  if (!digitalRead(recon)){
    if (!recording){
      recording = !recording;
      digitalWrite(LED_BUILTIN,HIGH);
      Serial.println("recording (!)");
      start_time = micros();
    }
    if (!bno08x.getSensorEvent(&sensorValue)) { return; }
    //
    switch (sensorValue.sensorId) {
      case SH2_ACCELEROMETER:
        //tic = micros();
        dataString += "a,";
        dataString += String(sensorValue.un.accelerometer.x);
        dataString += ",";
        dataString += String(sensorValue.un.accelerometer.y);
        dataString += ",";
        dataString += String(sensorValue.un.accelerometer.z);
        dataString += ",";
        dataString += String((micros()-start_time)/us,3);
        //tic = micros();
        //Serial.println(micros()-tic);
        file.println(dataString);
        break;
      case SH2_GYROSCOPE_CALIBRATED:
        dataString += "g,";
        dataString += String(sensorValue.un.gyroscope.x);
        dataString += ",";
        dataString += String(sensorValue.un.gyroscope.y);
        dataString += ",";
        dataString += String(sensorValue.un.gyroscope.z);
        dataString += ",";
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
        dataString += ",";
        dataString += String((micros()-start_time)/us,3);
        file.println(dataString);
        break;
      case SH2_ROTATION_VECTOR:
        dataString += "rv,";
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
  else{
    if (recording) {
      recording = !recording;
      digitalWrite(LED_BUILTIN,LOW);
      //tic = micros();
      file.close();
      //Serial.println(micros()-tic);
      Serial.println("in standby ...");
    }
  }
}
