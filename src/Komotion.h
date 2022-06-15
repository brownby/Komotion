/*
   Komotion.h - Library for Komotion Sensing Platform
   Created by J. Evan Smith and Bejamin Y. Brown
   Active Learning Labs

   last revised 28 April 2022
*/

#ifndef KOMOTION_h
#define KOMOTION_h

#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h>
#include <array>
#include <Adafruit_NeoPixel.h>
#include <ArduinoLowPower.h>
#include "ALL_Adafruit_BNO08x.h"

#define BNO08X_P0 9
#define BNO08X_P1 6
#define BNO08X_CS A4
#define BNO08X_INT A5
#define BNO08X_RESET 5

#define KOMOTION_SWITCH A1
#define KOMOTION_NEOPIX A0
#define KOMOTION_SD_CS 4
#define KOMOTION_SD_CD 7

#define us 1000000.0
#define baseName "data"

class Komotion
{
    public:
        Komotion();
        void begin(char config[5], bool saveBat);
        void record(void);
        void calibrate(void);
    private:
        void _bnoDetails(void);
        void _setReports(bool configState[], int configRate[]);
        bool _saveBat;
        bool _recording;
        bool _dimenStates[31][5];
        bool _dimenState[5];
        int _sd_cs;
        int _pixNum;
        int _setConfig;  
        int _fileIndex;
        int _countCompare;
        int _dimenRates[31][5];
        unsigned long _start_time;
        String _dataString;
        char _config[6];
        char _fileName[13];
        uint8_t _fileNameSize;
        sh2_SensorValue_t _sensorValue;
        int _accCal;
        int _gyroCal;
        int _magCal;
};

void lpCallback(void);

#endif