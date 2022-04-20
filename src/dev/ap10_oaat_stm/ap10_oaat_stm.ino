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
#include <STM32SD.h>
#include <Adafruit_BNO08x.h>

#ifndef SD_DETECT_PIN
#define SD_DETECT_PIN SD_DETECT_NONE
#endif

// parameters

int sr = 10; // sample rate, in Hz
int stp = 10;
int smp = 0;

// pins

#define BNO08X_CS 5
#define BNO08X_INT 12
#define BNO08X_RESET 9

// other

#define us 1000000.0
#define FILE_BASE_NAME "data"

File file;

unsigned long start_time;
bool recording = false;

Adafruit_BNO08x bno08x(BNO08X_RESET); // -1, no reset
sh2_SensorValue_t sensorValue;

void setup() {
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.print("attempting to setup sd card...");
  while (!SD.begin(SD_DETECT_PIN)) {
    delay(10);
  }
  while (SD.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    }
  }
  file = SD.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println("error opening file");
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
  if (!bno08x.enableReport(SH2_ACCELEROMETER, (int)us/sr)) {
    Serial.println("could not enable accelerometer (!)");
  }
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/200)) {
    Serial.println("could not enable accelerometer (!)");
  }
}

void loop() {

  if (bno08x.wasReset()) {
    //Serial.println("sensor was reset ...");
    setReports();
  }

  if (stp < 501) {
    if ((micros() - start_time) / us > 5) {
      //if (sr<90){
        sr = sr + 10;
      //}
      stp = stp + 10;
      Serial.println(sr);
      Serial.println(stp);
      bno08x.hardwareReset();
      delay(1000);
      start_time = micros();
    }
    else {
      String dataString = "";
      if (!bno08x.getSensorEvent(&sensorValue)) {
        return;
      }
      switch (sensorValue.sensorId) {
        case SH2_ACCELEROMETER:
          dataString += "a,";
          dataString += String(sensorValue.un.accelerometer.x);
          dataString += ",";
          dataString += String(sensorValue.un.accelerometer.y);
          dataString += ",";
          dataString += String(sensorValue.un.accelerometer.z);
          dataString += ",";
          dataString += String((micros() - start_time) / us, 3);
          file.println(dataString);
          smp = smp + 1;
          if (smp > 100){
            //file.flush();
            smp = 0;
          }
          break;
        case SH2_GYROSCOPE_CALIBRATED:
          dataString += "g,";
          dataString += String(sensorValue.un.gyroscope.x);
          dataString += ",";
          dataString += String(sensorValue.un.gyroscope.y);
          dataString += ",";
          dataString += String(sensorValue.un.gyroscope.z);
          dataString += ",";
          dataString += String((micros() - start_time) / us, 3);
          file.println(dataString);
          smp = smp + 1;
          if (smp > 100){
            //file.flush();
            smp = 0;
          }
          break;
        default:
          break;
      }
    }
  }
  else {
    file.close();
    while (1) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
    }
  }
}
