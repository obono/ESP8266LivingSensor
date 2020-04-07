#pragma once

#include <Wire.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_BMP280.h>

#include "DebugMacros.h"

class MySensors
{
public:
    MySensors(void);
    ~MySensors();
    void    activate(void);
    void    doSensing(float &illum, float &temp);
    void    inactivate(void);
    static void scanI2C(void);

private:
    Adafruit_TSL2561_Unified    *pTsl;
    Adafruit_BMP280             *pBmp;
};
