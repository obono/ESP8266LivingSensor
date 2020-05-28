#include <Arduino.h>

#include <user_interface.h>

#include "MySensors.h"
#include "NetworkManager.h"
#include "DebugMacros.h"

#define SENSORS_WAIT_MS         500 // a half minute
#define LOOP_INTERVAL_MS        (3 * 60 * 1000UL) // 3 minutes

static void displayChipInformation(void);

static MySensors        sensors;
static NetworkManager   netMan;
static bool isBooted;

/*-----------------------------------------------------------------------------------------------*/

void setup()
{
    isBooted = !(ESP.getResetInfoPtr()->reason == rst_reason::REASON_DEEP_SLEEP_AWAKE);
#ifdef DEBUG || DEBUG_HTTPS_REDIRECT
    Serial.begin(SERIAL_BAUD_RATE);
    if (isBooted) {
        displayChipInformation();
        MySensors::scanI2C();
    }
#endif
}

void loop()
{
    sensors.activate();
    delay(SENSORS_WAIT_MS - millis());
    RECORD_T record;
    bool isSuccess = sensors.doSensing(record);
    sensors.inactivate();
    if (!isSuccess) {
        dprintln(F("Failed to sense..."));
        goto bail;
    }

    isSuccess = netMan.setupWiFi();
    if (isSuccess) {
        String recordsPayload = "[" + MySensors::convert2JSON(record) + "]";
        isSuccess = netMan.uploadRecords(recordsPayload);
        if (!isSuccess) {
            dprintln(F("Failed to upload..."));
        }
    } else {
        dprintln(F("Failed to setup Wi-Fi..."));
    }

bail:
    long spentTime = millis();
    dprint("Spent Time (ms)=");
    dprintln(spentTime);

    if (isSuccess) {
        dprintln(F("Deep sleep."));
        ESP.deepSleep((LOOP_INTERVAL_MS - spentTime) * 1000UL, WAKE_RF_DEFAULT);
    } else {
        dprintln(F("Restart!"));
        ESP.restart();
    }
    delay(1000);
    dprintln(F("Why can you came here?"));
}

static void displayChipInformation(void)
{
    dprintln(F("----- ESP-WROOM-02 ( ESP8266 ) Chip Infomation -----"));
    dprintln();

    dprint(F("Core Version = "));
    dprintln(ESP.getCoreVersion());

    dprint(F("CPU Frequency = "));
    dprint(ESP.getCpuFreqMHz());
    dprintln(F(" MHz"));

    dprint(F("ChipID = "));
    dprintln(ESP.getChipId(), HEX);

    dprint(F("Flash ID = "));
    dprintln(ESP.getFlashChipId(), HEX);

    dprint(F("SDK version = "));
    dprintln(ESP.getSdkVersion());

    dprint(F("Boot version = "));
    dprintln(ESP.getBootVersion());

    dprint(F("Boot Mode = "));
    dprintln(ESP.getBootMode());

    dprint(F("Flash Chip IDE Size = "));
    dprint(ESP.getFlashChipSize());
    dprintln(F(" byte"));

    dprint(F("Flash Chip Real Size = "));
    dprint(ESP.getFlashChipRealSize());
    dprintln(F(" byte"));

    dprint(F("Flash Frequency = "));
    dprint(ESP.getFlashChipSpeed());
    dprintln(F(" Hz"));

    const __FlashStringHelper *mode_str;
    switch (ESP.getFlashChipMode()) {
        case 0: mode_str = F("QIO");    break;
        case 1: mode_str = F("QOUT");   break;
        case 2: mode_str = F("DIO");    break;
        case 3: mode_str = F("DOUT");   break;
        case 4: mode_str = F("Unknown");break;
        default:                        break;
    }
    dprint(F("Flash Chip Mode = "));
    dprintln(mode_str);

    dprint(F("Free Heap Size = "));
    dprintln(ESP.getFreeHeap());

    dprint(F("Free Sketch Size = "));
    dprintln(ESP.getFreeSketchSpace());

    dprint(F("Sketch Size = "));
    dprintln(ESP.getSketchSize());

    uint8_t mac[6];
    WiFi.macAddress(mac);
    dprint(F("WiFi StationAP Mac Address = "));
    dprintf("%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    WiFi.softAPmacAddress(mac);
    dprint(F("WiFi SoftAP Mac Address = "));
    dprintf("%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    dprintln();
    dprintln();
}
