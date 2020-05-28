#include "MySensors.h"

#define PIN_ENABLE_MIC  14
#define PIN_TOUT        17

#define MIC_SAMPLE_MS   1000
#define MIC_LEVEL_SCALE 64

#define STRING_BUFFER   64

MySensors::MySensors(void)
{
    pTsl = new Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT);
    pBmp = new Adafruit_BMP280();
    pinMode(PIN_ENABLE_MIC, OUTPUT);
    digitalWrite(PIN_ENABLE_MIC, LOW);
}

MySensors::~MySensors()
{
    delete pTsl;
    delete pBmp;
    digitalWrite(PIN_ENABLE_MIC, LOW);
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
                Adafruit_BMP280::MODE_FORCED,       // Operating Mode
                Adafruit_BMP280::SAMPLING_X1,       // Temp. oversampling
                Adafruit_BMP280::SAMPLING_NONE,     // Pressure oversampling
                Adafruit_BMP280::FILTER_OFF,        // Filtering
                Adafruit_BMP280::STANDBY_MS_4000);  // Standby time
    } else {
        dprintln(F("Couldn't find BMP280."));
    }
    digitalWrite(PIN_ENABLE_MIC, HIGH);
    isActive = true;
}

bool MySensors::doSensing(RECORD_T &record)
{
    if (!isActive) return false;

    uint16_t broadband, ir;
    pTsl->getLuminosity(&broadband, &ir);
    record.illum = pTsl->calculateLux(broadband, ir);
    record.temp = pBmp->readTemperature();
    record.sound = doSensingSound(MIC_SAMPLE_MS);

    dprint(F("Luminosity = "));
    dprint(record.illum);
    dprintln(F(" lux"));

    dprint(F("Temperature = "));
    dprint(record.temp);
    dprintln(F(" *C"));

    dprint(F("Sound level = "));
    dprintln(record.sound);

    dprintln();
    return true;
}

float MySensors::doSensingSound(long duration)
{
    long targetMillis = millis() + duration;
    long diffTotal = 0;
    int lastValue = analogRead(PIN_TOUT), samples = 0;
    while (millis() < targetMillis) {
        delay(1);
        int currentValue = analogRead(PIN_TOUT);
        diffTotal += abs(currentValue - lastValue);
        lastValue = currentValue;
        samples++;
    }
    return diffTotal * MIC_LEVEL_SCALE / (float)samples;
}

void MySensors::inactivate(void)
{
    isActive = false;
}

String MySensors::convert2JSON(RECORD_T &record)
{
    char buf[STRING_BUFFER];
    sprintf(buf, "{\"illum\":%.2f,\"temp\":%.2f,\"sound\":%.2f}", record.illum, record.temp, record.sound);
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
