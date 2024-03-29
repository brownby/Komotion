/*
   Komotion.cpp - Library for Komotion, the Komotion Sensing Platform
   Created by J. Evan Smith and Benjamin Y. Brown
   Active Learning Labs
*/

#include "Arduino.h"
#include "Komotion.h"

SdFat _sd;
SdFile _file;
Adafruit_BNO08x *_bno08x = NULL;
Adafruit_NeoPixel _pixel(1, KOMOTION_NEOPIX, NEO_GRB + NEO_KHZ800);
bool lpFlag = false;
uint8_t resetCount = 0;

Komotion::Komotion(){}

void Komotion::begin(char config[5], bool saveBat){
    
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
    pinMode(BNO08X_ONOFF, OUTPUT);
    digitalWrite(BNO08X_P0, HIGH);
    digitalWrite(BNO08X_P1, HIGH);
    digitalWrite(BNO08X_ONOFF, LOW); // turn on device

    // setup switch, neopixel, other

    pinMode(KOMOTION_SWITCH, INPUT_PULLUP);
    pinMode(KOMOTION_SD_CD, INPUT_PULLUP);

    if (!_saveBat){
        pinMode(KOMOTION_NEOPIX, OUTPUT);
        _pixNum = 0;
        _pixel.begin();
        _pixel.clear();
        _pixel.setPixelColor(_pixNum,0,0,25);
        _pixel.show();
    }

    Serial.begin(115200);
    // while(!Serial){delay(10);}

    Serial.print("attempting to setup SD card...");

    _sd_cs = KOMOTION_SD_CS;
    if (!_sd.begin(_sd_cs, SD_SCK_MHZ(12))) {
        while(!digitalRead(KOMOTION_SD_CD)){
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

    _bno08x = new Adafruit_BNO08x(BNO08X_RESET);

    if (!_bno08x->begin_SPI(BNO08X_CS, BNO08X_INT)) {
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

    LowPower.attachInterruptWakeup(KOMOTION_SWITCH, lpCallback, FALLING);

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

void Komotion::_bnoDetails(void){
    for (int n = 0; n < _bno08x->prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(_bno08x->prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(_bno08x->prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(_bno08x->prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(_bno08x->prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(_bno08x->prodIds.entry[n].swBuildNumber);
    }
}

void Komotion::_setReports(bool configState[], int configRate[]){
    uint8_t calConfig = 0;
    if (configState[0]){
        calConfig |= SH2_CAL_ACCEL;
        Serial.println("Enabling accelerometer");
        if (!_bno08x->enableReport(SH2_ACCELEROMETER, (int)us/configRate[0])) {
            Serial.println("could not enable accelerometer (!)");
        }
    }
    if (configState[1]){
        calConfig |= SH2_CAL_ACCEL;
        Serial.println("Enabling linear acceleration");
        if (!_bno08x->enableReport(SH2_LINEAR_ACCELERATION, (int)us/configRate[1])) {
            Serial.println("could not enable linear acceleration (!)");
        }
    }
    if (configState[2]){
        calConfig |= SH2_CAL_GYRO;
        Serial.println("Enabling gyroscope");
        if (!_bno08x->enableReport(SH2_GYROSCOPE_CALIBRATED, (int)us/configRate[2])) {
            Serial.println("could not enable gyroscope (!)"); 
        }
    }
    if (configState[3]){
        calConfig |= SH2_CAL_MAG;
        Serial.println("Enabling magnetic field");
        if (!_bno08x->enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, (int)us/configRate[3])) {
            Serial.println("could not enable magnetometer (!)");
        }
    }
    if (configState[4]){
        Serial.println("Enabling rotation vector");
        if (!_bno08x->enableReport(SH2_ROTATION_VECTOR, (int)us/configRate[4])) {
            Serial.println("could not enable rotation vector");
        }
    }
    _bno08x->getSensorEvent(&_sensorValue); // need this line to avoid a null pointer exception on the next line
    if(!sh2_setCalConfig(calConfig)) {Serial.print("succesfully set dynamic cal: "); Serial.println(calConfig);}
}

void Komotion::record(void){
    if(lpFlag) {

        // Serial.begin(115200);
        // while(!Serial){delay(10);}

        // 0.2.2: Removing these pin toggles until empty file bug is resolved

        // Reset BNO pins that were pulled down for low power back to appropriate state
        // pinMode(BNO08X_P0, OUTPUT);
        // pinMode(BNO08X_P1, OUTPUT);
        // pinMode(BNO08X_RESET, OUTPUT);
        // pinMode(BNO08X_INT, INPUT_PULLUP);

        // digitalWrite(BNO08X_P0, HIGH);
        // digitalWrite(BNO08X_P1, HIGH);
        // digitalWrite(BNO08X_RESET, HIGH);

        // 0.2.4: Add these toggles back and initialize _bno08x object

        // Reset pins that were pulled low via OUTPUT
        // INT and SPI pins are covered by BNO init methods
        digitalWrite(BNO08X_P0, HIGH);
        digitalWrite(BNO08X_P1, HIGH);
        digitalWrite(BNO08X_RESET, HIGH);

        delay(200);

        digitalWrite(BNO08X_ONOFF, LOW); // turn on BNO
        delay(200);

        // 0.2.5: Adding this check here because sometimes record switch bouncing causes thid if statement to be entered early
        // Not really a problem so long as the BNO is successfully re-initialized, which requires deleting the old pointer and closing the SH2 interface
        if (_bno08x) 
        {
            sh2_close();
            delete _bno08x;
            _bno08x = NULL;
        }

        _bno08x = new Adafruit_BNO08x(BNO08X_RESET);

        if (!_bno08x->begin_SPI(BNO08X_CS, BNO08X_INT)) {
            Serial.println(" failed to initialize BNO08x");
            while(1) {delay(10);}
        }

        lpFlag = false;
    }
    if (_bno08x->wasReset()){
        _setReports(_dimenStates[_setConfig], _dimenRates[_setConfig]);
        resetCount++;
        
        // v0.2.3: BNO reset more than 5 times, probably as a result of SPI timeouts. Blink purple to indicate to users to reset the device
        if (resetCount >= 5)
        {
            while (true)
            {
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,25,0,25);
                _pixel.show();
                delay(100);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(1000);
            }
        }
    }
    if(!digitalRead(KOMOTION_SWITCH)) {
        if(!_recording){
            _recording = !_recording;

            while(!digitalRead(KOMOTION_SD_CD)){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,25);
                _pixel.show();
                delay(100);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(1000);
                if (digitalRead(KOMOTION_SWITCH)) {
                    _recording = !_recording;
                    return;
                }
            }
            
            // define filename, given existing files

            uint8_t _fileNameSize = sizeof(baseName) - 1;
            char _fileName[13] = baseName "01.csv";
            _fileIndex = 1;

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
            while(!_file.open(_fileName, FILE_WRITE))
            {
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,25);
                _pixel.show();
                delay(100);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(1000);
            }
            if (!_saveBat){
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,25,0,0);
                _pixel.show();
            }
            Serial.println("recording");
            _start_time = micros();
        }
        _dataString = "";
        if (!_bno08x->getSensorEvent(&_sensorValue)){ }
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

            // Long blinks for every ten
            for (int i = 0; i < _fileIndex / 10; i++)
            {
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,25,0);
                _pixel.show();
                delay(600);  
                _pixel.clear();
                _pixel.setPixelColor(_pixNum,0,0,0);
                _pixel.show();  
                delay(600);
            }

            // Short blinks for ones
            for (int x = 0; x < _fileIndex % 10; x++) 
            {
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
        digitalWrite(BNO08X_ONOFF, HIGH); // turn off BNO

        delay(200);

        // 0.2.2: Removing these pin toggles until empty file bug is resolved

        // Pull all BNO pins low via pulldowns
        // pinMode(BNO08X_P0, INPUT_PULLDOWN);
        // pinMode(BNO08X_P1, INPUT_PULLDOWN);
        // pinMode(BNO08X_RESET, INPUT_PULLDOWN);
        // pinMode(BNO08X_INT, INPUT_PULLDOWN);
        // pinMode(23, INPUT_PULLDOWN); // MOSI
        // pinMode(22, INPUT_PULLDOWN); // MISO
        // pinMode(24, INPUT_PULLDOWN); // SCK
        // pinMode(A4, INPUT_PULLDOWN); // CS

        // 0.2.4: Add pin toggles back, but this time toggle EVERYTHING connected to the BNO, and delete _bno08x object

        sh2_close();
        delete _bno08x;
        _bno08x = NULL;

        // Pull all BNO pins low via OUTPUT 
        pinMode(23, OUTPUT); // MOSI
        pinMode(22, OUTPUT); // MISO
        pinMode(24, OUTPUT); // SCK
        pinMode(A4, OUTPUT); // CS
        pinMode(BNO08X_INT, OUTPUT);
        digitalWrite(BNO08X_P0, LOW);
        digitalWrite(BNO08X_P1, LOW);
        digitalWrite(BNO08X_RESET, LOW);
        digitalWrite(BNO08X_INT, LOW);
        digitalWrite(23, LOW); 
        digitalWrite(22, LOW);
        digitalWrite(24, LOW);
        digitalWrite(A4, LOW);
        
        // Serial.end();

        resetCount = 0;

        LowPower.sleep();
    }
}

void Komotion::calibrate() {
    while(digitalRead(KOMOTION_SWITCH)) { // keep calibrating until record switch is switched to record
        if (_bno08x->wasReset()) {
            _setReports(_dimenStates[_setConfig], _dimenRates[_setConfig]);

            resetCount++;
        
            // v0.2.4: BNO reset more than 5 times, probably as a result of SPI timeouts. Blink purple to indicate to users to reset the device
            if (resetCount >= 5)
            {
                while (true)
                {
                    _pixel.clear();
                    _pixel.setPixelColor(_pixNum,25,0,25);
                    _pixel.show();
                    delay(100);  
                    _pixel.clear();
                    _pixel.setPixelColor(_pixNum,0,0,0);
                    _pixel.show();  
                    delay(1000);
                }
            }
        }
        
        if (!_bno08x->getSensorEvent(&_sensorValue)){continue;}
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

            resetCount = 0;

            return;
        }
    }

    resetCount = 0;
}

void lpCallback(void)
{
    lpFlag = true;
}