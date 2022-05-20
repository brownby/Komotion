/*
   ES20r.cpp - Library for ES20r Sensing Platform
   Created by J. Evan Smith and Bejamin Y. Brown
   Active Learning Labs

   last revised 28 April 2022
*/

#include "Arduino.h"
#include "ES20r.h"

SdFat _sd;
SdFile _file;
Adafruit_BNO08x _bno08x(BNO08X_RESET);
Adafruit_NeoPixel _pixel(1, ES20r_NEOPIX, NEO_GRB + NEO_KHZ800);
bool lpFlag = false;

ES20r::ES20r(){}

void ES20r::begin(char config[5], bool saveBat){
    
    memcpy(_config, config, sizeof(_config));
    _saveBat = saveBat;
    
    // indicate in begin()

    if (!_saveBat){
        pinMode(LED_BUILTIN, OUTPUT); 
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else{
        digitalWrite(LED_BUILTIN, LOW);
    }

    // setup BNO in SPI mode

    pinMode(BNO08X_P0, OUTPUT);
    pinMode(BNO08X_P1, OUTPUT);
    digitalWrite(BNO08X_P0, HIGH);
    digitalWrite(BNO08X_P1, HIGH);

    // setup switch, neopixel, other

    pinMode(ES20r_SWITCH, INPUT_PULLUP);
    pinMode(ES20r_SD_CD, INPUT_PULLUP);

    if (!_saveBat){
        pinMode(ES20r_NEOPIX, OUTPUT);
        _pixNum = 0;
        _pixel.begin();
        _pixel.clear();
        _pixel.setPixelColor(_pixNum,0,0,25);
        _pixel.show();
    }

    Serial.begin(115200);
    // while(!Serial){delay(10);}

    Serial.print("attempting to setup SD card...");

    _sd_cs = ES20r_SD_CS;
    if (!_sd.begin(_sd_cs, SD_SCK_MHZ(12))) {
        while(!digitalRead(ES20r_SD_CD)){
            _pixel.clear();
            _pixel.setPixelColor(_pixNum,0,0,25);
            _pixel.show();
            delay(100);  
            _pixel.clear();
            _pixel.setPixelColor(_pixNum,0,0,0);
            _pixel.show();  
            delay(1000);
        }
        _sd.begin(_sd_cs, SD_SCK_MHZ(12));
    }
    Serial.println(" initialized");

    Serial.print("attempting to setup IMU...");

    if (!_bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
            Serial.println(" failed to initialize BNO08x");
        while(1) {delay(10);}
    }
    Serial.println(" initialized");

    // look up desired rates for configuration

    bool _dimenState[5] = {0,0,0,0,0};

    for (int x = 0; x < strlen(_config); x++){
        switch(_config[x]){
            case 'a':
                _dimenState[0] = 1;
                break;
            case 'l':
                _dimenState[1] = 1;
                break;
            case 'g':
                _dimenState[2] = 1;
                break;
            case 'm':
                _dimenState[3] = 1;
                break;
            case 'r':
                _dimenState[4] = 1;
                break;
        }
    }

    bool _dimenStatesInit[31][5] = {{1,0,0,0,0},
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

    memcpy(_dimenStates, _dimenStatesInit, sizeof(_dimenStates));


    for (int x = 0; x < 31; x++){
        int _countCompare = 0;
        for (int y = 0; y < 5; y++){
            if (_dimenStates[x][y] == _dimenState[y]){
                _countCompare += 1;
            }
        }
        if (_countCompare == 5){
            _setConfig = x;
            break;
        } 
    }
                                   
    int _dimenRatesInit[31][5] = {{400,0,0,0,0}, 
                                   {0,200,0,0,0},
                                   {125,100,0,0,0},
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
                                   {125,100,100,50,0},
                                   {0,0,100,100,100},
                                   {125,0,100,0,100},
                                   {0,100,100,0,100},
                                   {125,100,100,0,100},
                                   {125,0,0,50,100},
                                   {0,100,0,50,100},
                                   {125,100,0,25,100},
                                   {125,0,100,25,100},
                                   {0,100,100,25,100},
                                   {125,100,100,25,100}};

    memcpy(_dimenRates, _dimenRatesInit, sizeof(_dimenRates));

    delay(100);
    _bnoDetails();
    _setReports(_dimenStates[_setConfig],_dimenRates[_setConfig]);
    _recording = false; 

    LowPower.attachInterruptWakeup(ES20r_SWITCH, lpCallback, FALLING);

    _accCal = 0;
    _gyroCal = 0;
    _magCal = 0;
    if (!_saveBat){
        _pixel.clear();
        _pixel.setPixelColor(_pixNum,0,0,0);
        _pixel.show();
        digitalWrite(LED_BUILTIN, LOW); // indicate end of begin()
    }
    calibrate();
}

void ES20r::_bnoDetails(void){
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

void ES20r::_setReports(bool configState[], int configRate[]){
    uint8_t calConfig = 0;
    if (configState[0]){
        calConfig |= SH2_CAL_ACCEL;
        if (!_bno08x.enableReport(SH2_ACCELEROMETER, (int)us/configRate[0])) {
            Serial.println("could not enable accelerometer (!)");
        }
    }
    if (configState[1]){
        calConfig |= SH2_CAL_ACCEL;
        if (!_bno08x.enableReport(SH2_LINEAR_ACCELERATION, (int)us/configRate[1])) {
            Serial.println("could not enable linear acceleration (!)");
        }
    }
    if (configState[2]){
        calConfig |= SH2_CAL_GYRO;
        if (!_bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/configRate[2])) {
            Serial.println("could not enable gyroscope (!)"); 
        }
    }
    if (configState[3]){
        calConfig |= SH2_CAL_MAG;
        if (!_bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/configRate[3])) {
            Serial.println("could not enable magnetometer (!)");
        }
    }
    if (configState[4]){
        if (!_bno08x.enableReport(SH2_ROTATION_VECTOR, (int)us/configRate[4])) {
            Serial.println("could not enable rotation vector");
        }
    }
    if(!sh2_setCalConfig(calConfig)) {Serial.print("succesfully set dynamic cal: "); Serial.println(calConfig);}
}

void ES20r::record(void){
    if(lpFlag) {
        delay(1000);
        lpFlag = false;
    }
    if (_bno08x.wasReset()){
        _setReports(_dimenStates[_setConfig], _dimenRates[_setConfig]);
    }
    if(!digitalRead(ES20r_SWITCH)){
        if(!_recording){
            _recording = !_recording;

            while(!digitalRead(ES20r_SD_CD)){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,25);
                _pixel.show();
                delay(100);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(1000);
                if(digitalRead(ES20r_SWITCH)){
                    _recording = !_recording;
                    return;
                }
            }
            
            // define filename, given existing files

            uint8_t _fileNameSize = sizeof(baseName) - 1;
            char _fileName[13] = baseName "00.csv";
            _fileIndex = 0;

            _sd.begin(_sd_cs, SD_SCK_MHZ(12)); 

            while (_sd.exists(_fileName)) {
                if (_fileName[_fileNameSize + 1] != '9') {
                    _fileName[_fileNameSize + 1]++;
                    _fileIndex++;
                } else if (_fileName[_fileNameSize] != '9') {
                    _fileName[_fileNameSize + 1] = '0';
                    _fileName[_fileNameSize]++;
                    _fileIndex++;
                } 
                else {
                    while(true){
                        Serial.println("cannot create file name");
                        _pixel.clear();
                        _pixel.setPixelColor(_pixNum,25,25,0);
                        _pixel.show();
                        delay(100);  
                        _pixel.clear();
                        _pixel.setPixelColor(_pixNum,0,0,0);
                        _pixel.show();  
                        delay(1000);
                    }
                }
            }
            Serial.println(_fileName);
            _file.open(_fileName, FILE_WRITE);    
            if (!_saveBat){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,25,0,0);
                _pixel.show();
            }
            Serial.println("recording");
            _start_time = micros();
        }
        _dataString = "";
        if (!_bno08x.getSensorEvent(&_sensorValue)){}
        else{
          switch (_sensorValue.sensorId) {
            case SH2_ACCELEROMETER:
              _accCal = _sensorValue.status;
              _dataString += "a,";
              _dataString += String(_sensorValue.un.accelerometer.x);
              _dataString += ",";
              _dataString += String(_sensorValue.un.accelerometer.y);
              _dataString += ",";
              _dataString += String(_sensorValue.un.accelerometer.z);
              _dataString += ", ,";
            //   _dataString += ",";
            //   _dataString += String(_accCal);
            //   _dataString += ",";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              break;
            case SH2_LINEAR_ACCELERATION:
                _accCal = _sensorValue.status;
                _dataString += "l,";
                _dataString += String(_sensorValue.un.linearAcceleration.x);
                _dataString += ",";
                _dataString += String(_sensorValue.un.linearAcceleration.y);
                _dataString += ",";
                _dataString += String(_sensorValue.un.linearAcceleration.z);
                _dataString += ", ,";
            //   _dataString += ",";
            //   _dataString += String(_accCal);
            //   _dataString += ",";
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
            //   _dataString += ",";
            //   _dataString += String(_gyroCal);
            //   _dataString += ",";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              _gyroCal = _sensorValue.status;
              break;
            case SH2_MAGNETIC_FIELD_CALIBRATED:
              _dataString += "m,";
              _dataString += String(_sensorValue.un.magneticField.x);
              _dataString += ",";
              _dataString += String(_sensorValue.un.magneticField.y);
              _dataString += ",";
              _dataString += String(_sensorValue.un.magneticField.z);
              _dataString += ", ,";
            //   _dataString += ",";
            //   _dataString += String(_magCal);
            //   _dataString += ",";
              _dataString += String((micros()-_start_time)/us,3);
              _file.println(_dataString);
              _magCal = _sensorValue.status;
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
            Serial.println("writing to file");
            _file.close();
            _pixel.clear();
            _pixel.show();
            delay(500);
            for (int x = 0; x < _fileIndex; x++){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,25,0);
                _pixel.show();
                delay(300);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(300);
            }
            if (!_saveBat){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();
            }
            Serial.println("in standby");
        }
        LowPower.sleep();
    }
}

void ES20r::calibrate() {
    while(digitalRead(ES20r_SWITCH)) { // keep calibrating until record switch is switched to record
        if (!_bno08x.getSensorEvent(&_sensorValue)){continue;}
        else {
            switch (_sensorValue.sensorId) {
                case SH2_ACCELEROMETER:
                    _accCal = _sensorValue.status;
                    break;
                case SH2_LINEAR_ACCELERATION:
                    _accCal = _sensorValue.status;
                    break;
                case SH2_GYROSCOPE_CALIBRATED:
                    _gyroCal = _sensorValue.status;
                    break;
                case SH2_MAGNETIC_FIELD_CALIBRATED:
                    _magCal = _sensorValue.status;
                break;
            }
        }
        int lowestCal = 4;
        if(_dimenStates[_setConfig][0] || _dimenStates[_setConfig][1]) {
            if(_accCal < lowestCal) lowestCal = _accCal;
        }
        if(_dimenStates[_setConfig][2]) {
            if(_gyroCal < lowestCal) lowestCal = _gyroCal;
        }
        if(_dimenStates[_setConfig][3]) {
            if(_magCal < lowestCal) lowestCal = _magCal;
        }

        // lowestCal = 1;
        if (lowestCal == 0 || lowestCal == 1) {
            _pixel.clear();
            _pixel.setPixelColor(_pixNum,35,8,0);
            _pixel.show();
        }
        else if (lowestCal == 2) {
            _pixel.clear();
            _pixel.setPixelColor(_pixNum,12,12,0);
            _pixel.show();
        }
        else if (lowestCal == 3) {
            _pixel.clear();
            _pixel.setPixelColor(_pixNum,0,25,0);
            _pixel.show();
            for(int i = 0; i < 5; i++) {
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,6,22,20);
                _pixel.show();
                delay(100);
                _pixel.clear();
                _pixel.show();
                delay(100);
            }
            return;
        }
    }
}

void lpCallback(void){lpFlag = true;}