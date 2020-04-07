#include "MySensors.h"

MySensors::MySensors(void)
{
    pTsl = new Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT);
    pBmp = new Adafruit_BMP280();
}

MySensors::~MySensors()
{
    delete pTsl;
    delete pBmp;
}

void MySensors::activate(void)
{
    Wire.begin();
    if (pTsl->begin()) {
        pTsl->enableAutoRange(true);    // Auto-gain ... switches automatically between 1x and 16x
        pTsl->setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS); // medium resolution and speed
    } else {
        dprintln(F("Couldn't find TSL2561."));
    }
    if (pBmp->begin(BMP280_ADDRESS_ALT)) {
        pBmp->setSampling(
                Adafruit_BMP280::MODE_NORMAL,       /* Operating Mode. */
                Adafruit_BMP280::SAMPLING_X2,       /* Temp. oversampling */
                Adafruit_BMP280::SAMPLING_X16,      /* Pressure oversampling */
                Adafruit_BMP280::FILTER_X16,        /* Filtering. */
                Adafruit_BMP280::STANDBY_MS_500);   /* Standby time. */
    } else {
        dprintln(F("Couldn't find BMP280."));
    }
}

void MySensors::doSensing(float &illum, float &temp)
{
    uint16_t broadband, ir;
    pTsl->getLuminosity(&broadband, &ir);
    illum = pTsl->calculateLux(broadband, ir);
    temp = pBmp->readTemperature();
    dprint("Luminosity = ");
    dprint(illum);
    dprintln(" lux");

    dprint(F("Temperature = "));
    dprint(temp);
    dprintln(" *C");

#if 0
    dprint(F("Pressure = "));
    dprint(pBmp->readPressure() / 100.0f);
    dprintln(" hPa");

    dprint(F("Approx altitude = "));
    dprint(pBmp->readAltitude(1013.25)); /* Adjusted to local forecast! */
    dprintln(" m");
#endif
    dprintln();
}

void MySensors::inactivate(void)
{
    // do nothing
}

void MySensors::scanI2C(void)
{
    Wire.begin();
    dprintln("----- Scan I2C Devices -----");
    int nDevices = 0;
    for (byte adrs = 1; adrs < 127; adrs++) {
        Wire.beginTransmission(adrs);
        byte error = Wire.endTransmission();
        if (error == 0) {
            dprintf("I2C device found at address 0x%02X !\r\n", adrs);
            nDevices++;
        } else if (error == 4) {
            dprintf("Unknown error at address 0x%02X\r\n", adrs);
        }
    }
    dprint(nDevices);
    dprintln(" device(s) is found.");
    dprintln();
}
