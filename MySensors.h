#pragma once

#include <Wire.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_BMP280.h>

#include "DebugMacros.h"

typedef struct {
    float   illum;
    float   temp;
    float   sound;
} RECORD_T;

class MySensors
{
public:
    MySensors(void);
    ~MySensors();
    void    activate(void);
    bool    doSensing(RECORD_T &record);
    void    inactivate(void);
    static String convert2JSON(RECORD_T &record);
    static void scanI2C(void);

private:
    float   doSensingSound(long duration);
    Adafruit_TSL2561_Unified    *pTsl;
    Adafruit_BMP280             *pBmp;
    bool    isActive;
};
