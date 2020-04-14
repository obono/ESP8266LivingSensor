#include "MySensors.h"

#define STRING_BUFFER   64

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
                Adafruit_BMP280::MODE_NORMAL,       // Operating Mode
                Adafruit_BMP280::SAMPLING_X2,       // Temp. oversampling
                Adafruit_BMP280::SAMPLING_X16,      // Pressure oversampling
                Adafruit_BMP280::FILTER_X16,        // Filtering
                Adafruit_BMP280::STANDBY_MS_500);   // Standby time
    } else {
        dprintln(F("Couldn't find BMP280."));
    }
    isActive = true;
}

bool MySensors::doSensing(RECORD_T &record)
{
    if (!isActive) return false;

    uint16_t broadband, ir;
    pTsl->getLuminosity(&broadband, &ir);
    record.illum = pTsl->calculateLux(broadband, ir);
    record.temp = pBmp->readTemperature();

    dprint(F("Luminosity = "));
    dprint(record.illum);
    dprintln(F(" lux"));

    dprint(F("Temperature = "));
    dprint(record.temp);
    dprintln(F(" *C"));

#if 0
    dprint(F("Pressure = "));
    dprint(pBmp->readPressure() / 100.0f);
    dprintln(F(" hPa"));

    dprint(F("Approx altitude = "));
    dprint(pBmp->readAltitude(1013.25)); /* Adjusted to local forecast! */
    dprintln(F(" m"));
#endif
    dprintln();

    return true;
}

void MySensors::inactivate(void)
{
    isActive = false;
}

String MySensors::convert2JSON(RECORD_T &record)
{
    char buf[STRING_BUFFER];
    sprintf(buf, "{\"illum\":%.2f,\"temp\":%.2f}", record.illum, record.temp);
    return String(buf);
}

void MySensors::scanI2C(void)
{
    Wire.begin();
    dprintln(F("----- Scan I2C Devices -----"));
    int nDevices = 0;
    for (byte adrs = 1; adrs < 127; adrs++) {
        Wire.beginTransmission(adrs);
        byte error = Wire.endTransmission();
        if (error == 0) {
            dprint(F("I2C device found at address 0x"));
            dprintf("%02X !\r\n", adrs);
            nDevices++;
        } else if (error == 4) {
            dprint(F("Unknown error at address 0x"));
            dprintf("%02X !\r\n", adrs);
        }
    }
    dprint(nDevices);
    dprintln(F(" device(s) is found."));
    dprintln();
}
