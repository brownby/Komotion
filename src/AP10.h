/*
   AP10.h - Library for AP10 Sensing Platform
   Created by J. Evan Smith and Bejamin Y. Brown
   Active Learning Labs

   last revised 28 April 2022
*/

#ifndef AP10_h
#define AP10_h

#include "Arduino.h"
#include "SdFat.h"
#include <SPI.h>
#include <Adafruit_BNO08x.h>
#include <Adafruit_NeoPixel.h>

#define BNO08X_P0 9
#define BNO08X_P1 6
#define BNO08X_CS A4
#define BNO08X_INT A5
#define BNO08X_RESET 5

#define AP10_SWITCH A1
#define AP10_NEOPIX A0
#define AP10_SD_CS 4
#define us 1000000.0

class AP10
{
    public:
        AP10();
        void begin(void);
        void record(void);
        char configure[4];      
    private:
        void _bnoDetails(void);
        void _setReports(bool configState[], int configRate[]);
        bool _saveBat;
        bool _recording;
        bool _dimenStates[14][4];
        int _sd_cs;
        int _pixNum;
        int _dimenRates[14][4];
        unsigned long _start_time;
        String _dataString;
        sh2_SensorValue_t _sensorValue;
};

#endif