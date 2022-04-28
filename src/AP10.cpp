/*
   AP10.cpp - Library for AP10 Sensing Platform
   Created by J. Evan Smith and Bejamin Y. Brown
   Active Learning Labs

   last revised 28 April 2022
*/

#include "Arduino.h"
#include "AP10.h"

SdFat _sd;
SdFile _file;
Adafruit_BNO08x _bno08x(BNO08X_RESET);
Adafruit_NeoPixel _pixel(1, AP10_NEOPIX, NEO_GRB + NEO_KHZ800);

AP10::AP10(){}

void AP10::begin(std::array<char, 4> config){
    
    _config = config;
    _saveBat = false;

    // indicate in begin()

    if (!_saveBat){
        pinMode(LED_BUILTIN, OUTPUT); 
        digitalWrite(LED_BUILTIN, HIGH);
    }

    // setup BNO in SPI mode

    pinMode(BNO08X_P0, OUTPUT);
    pinMode(BNO08X_P1, OUTPUT);
    digitalWrite(BNO08X_P0, HIGH);
    digitalWrite(BNO08X_P1, HIGH);

    // setup switch, neopixel, other

    pinMode(AP10_SWITCH, INPUT_PULLUP);

    if (!_saveBat){
        pinMode(AP10_NEOPIX, OUTPUT);
        _pixNum = 1;
        _pixel.begin();
        _pixel.clear();
        _pixel.setPixelColor(_pixNum,0,0,255);
    }

    Serial.begin(115200);
    while(!Serial){delay(10);}

    Serial.print("attempting to setup SD card...");

    _sd_cs = AP10_SD_CS;
    if (!_sd.begin(_sd_cs, SD_SCK_MHZ(12))) {
        _sd.initErrorHalt();
    }
    Serial.println(" initialized");

    Serial.print("attempting to setup IMU...");

    if (!_bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
            Serial.println(" failed to initialize BNO08x");
        while(1) {delay(10);}
    }
    Serial.println(" initialized");

    bool _dimenState[4] = {0,0,0,0};

    for (int x = 0; x < 4; x++){
        switch(_config[x]){
            case 'a':
                _dimenState[0] = 1;
                break;
            case 'g':
                _dimenState[1] = 1;
                break;
            case 'm':
                _dimenState[2] = 1;
                break;
            case 'r':
                _dimenState[3] = 1;
                break;
        }
    }

    // Serial.println("_config: ");
    // for (int x = 0; x < 4; x++){
    //     Serial.println(_config[x]);
    // }

    // Serial.println("_dimenState: ");
    // for (int x = 0; x < 4; x++){
    //     Serial.println(_dimenState[x]);
    // }

    bool _dimenStates[14][4] = {{1,0,0,0}, // 14,4
                                {0,1,0,0},
                                {0,0,1,0},
                                {0,0,0,1},
                                {1,1,0,0},
                                {1,0,1,0},
                                {1,0,0,1},
                                {0,1,1,0},
                                {0,1,0,1},
                                {0,0,1,1},
                                {1,1,1,0},
                                {0,1,1,1},
                                {1,1,0,1},
                                {1,1,1,1}};

    for (int x = 0; x < 14; x++){
        int _countCompare = 0;
        for (int y = 0; y < 4; y++){
            if (_dimenStates[x][y] == _dimenState[y]){
                _countCompare += 1;
            }
        }
        if (_countCompare == 4){
            _setConfig = x;
            break;
        } 
    }
                                   
    int _dimenRates[14][4] = {{400,0,0,0},
                              {0,250,0,0},
                              {0,0,100,0},
                              {0,0,0,250},
                              {200,100,0,0},
                              {250,0,100,0},
                              {200,0,0,100},
                              {0,100,100,0},
                              {0,100,0,100},
                              {0,0,100,100},
                              {250,100,100,0},
                              {0,100,100,100},
                              {250,100,0,100},
                              {125,100,25,100}};   

    delay(100);
    _bnoDetails();
    _setReports(_dimenStates[_setConfig],_dimenRates[_setConfig]);
    _recording = false;  

    if (!_saveBat){
        _pixel.clear();
        digitalWrite(LED_BUILTIN, LOW); // indicate end of begin()
    }
}

void AP10::_bnoDetails(void){
    for (int n = 0; n < _bno08x.prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(_bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(_bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(_bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(_bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(_bno08x.prodIds.entry[n].swBuildNumber);
    }
}

void AP10::_setReports(bool configState[], int configRate[]){
    if (configState[0]){
        if (!_bno08x.enableReport(SH2_ACCELEROMETER, (int)us/configRate[0])) {
            Serial.println("could not enable accelerometer (!)");
        }
    }
    if (configState[1]){
        if (!_bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/configRate[1])) {
            Serial.println("could not enable gyroscope (!)"); 
        }
    }
    if (configState[2]){
        if (!_bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/configRate[2])) {
            Serial.println("could not enable magnetometer (!)");
        }
    }
    if (configState[3]){
        if (!_bno08x.enableReport(SH2_ROTATION_VECTOR, (int)us/configRate[3])) {
            Serial.println("could not enable rotation vector");
        }
    }
}

void AP10::record(void){
    if (_bno08x.wasReset()){
        _setReports(_dimenStates[14], _dimenRates[14]);
    }
    if(!digitalRead(AP10_SWITCH)){
        if(!_recording){
            _recording = !_recording;
            if (!_saveBat){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,255,0);
            }
            _file.open("data.csv",FILE_WRITE);
            Serial.println("recording");
            _start_time = micros();
        }
        _dataString = "";
        if (!_bno08x.getSensorEvent(&_sensorValue)){}
        else{
          switch (_sensorValue.sensorId) {
            case SH2_ACCELEROMETER:
              _dataString += "a,";
              _dataString += String(_sensorValue.un.accelerometer.x);
              _dataString += ",";
              _dataString += String(_sensorValue.un.accelerometer.y);
              _dataString += ",";
              _dataString += String(_sensorValue.un.accelerometer.z);
              _dataString += ", ,";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              break;
            case SH2_GYROSCOPE_CALIBRATED:
              _dataString += "g,";
              _dataString += String(_sensorValue.un.gyroscope.x);
              _dataString += ",";
              _dataString += String(_sensorValue.un.gyroscope.y);
              _dataString += ",";
              _dataString += String(_sensorValue.un.gyroscope.z);
              _dataString += ", ,";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              break;
            case SH2_MAGNETIC_FIELD_CALIBRATED:
              _dataString += "m,";
              _dataString += String(_sensorValue.un.magneticField.x);
              _dataString += ",";
              _dataString += String(_sensorValue.un.magneticField.y);
              _dataString += ",";
              _dataString += String(_sensorValue.un.magneticField.z);
              _dataString += ", ,";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              break;
            case SH2_ROTATION_VECTOR:
              _dataString += "r,";
              _dataString += String(_sensorValue.un.rotationVector.real);
              _dataString += ",";
              _dataString += String(_sensorValue.un.rotationVector.i);
              _dataString += ",";
              _dataString += String(_sensorValue.un.rotationVector.j);
              _dataString += ",";
              _dataString += String(_sensorValue.un.rotationVector.k);
              _dataString += ",";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              break;
            default:
              break;
            }   
        }
    }
    else{
        if(_recording){
            _recording = !_recording;
            if (!_saveBat){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,255,0,0);
            }
            _file.close();
            Serial.println("in standby");
        }
    }
}