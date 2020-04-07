#include <Arduino.h>

#include "MyNetworkManager.h"
#include "MySensors.h"
#include "DebugMacros.h"

#define LOOP_DELAY  (60 * 1000)

static MyNetworkManager netMan;
static MySensors        sensors;

static void displayChipInformation(void);

/*-----------------------------------------------------------------------------------------------*/

void setup()
{
#ifdef DEBUG || DEBUG_HTTPS_REDIRECT
    Serial.begin(SERIAL_BAUD_RATE);
    displayChipInformation();
    MySensors::scanI2C();
#endif
    netMan.setupWiFi();
    sensors.activate();
}

void loop()
{
    float illum, temp;
    sensors.doSensing(illum, temp);
    netMan.doPost(illum, temp);
    delay(LOOP_DELAY);
}

static void displayChipInformation(void)
{
    dprintln("----- ESP-WROOM-02 ( ESP8266 ) Chip Infomation -----");
    dprintln();

    dprint("Core Version = ");
    dprintln(ESP.getCoreVersion());

    dprint("CPU Frequency = ");
    dprint(ESP.getCpuFreqMHz());
    dprintln(" MHz");

    dprint("ChipID = ");
    dprintln(ESP.getChipId(), HEX);

    dprint("Flash ID = ");
    dprintln(ESP.getFlashChipId(), HEX);

    dprint("SDK version = ");
    dprintln(ESP.getSdkVersion());

    dprint("Boot version = ");
    dprintln(ESP.getBootVersion());

    dprint("Boot Mode = ");
    dprintln(ESP.getBootMode());

    dprint("Flash Chip IDE Size = ");
    dprint(ESP.getFlashChipSize());
    dprintln(" byte");

    dprint("Flash Chip Real Size = ");
    dprint(ESP.getFlashChipRealSize());
    dprintln(" byte");

    dprint("Flash Frequency = ");
    dprint(ESP.getFlashChipSpeed());
    dprintln(" Hz");

    String mode_str;
    switch (ESP.getFlashChipMode()) {
        case 0: mode_str = "QIO";       break;
        case 1: mode_str = "QOUT";      break;
        case 2: mode_str = "DIO";       break;
        case 3: mode_str = "DOUT";      break;
        case 4: mode_str = "Unknown";   break;
        default:                        break;
    }
    dprintln("Flash Chip Mode = " + mode_str);

    dprint("Free Heap Size = ");
    dprintln(ESP.getFreeHeap());

    dprint("Free Sketch Size = ");
    dprintln(ESP.getFreeSketchSpace());

    dprint("Sketch Size = ");
    dprintln(ESP.getSketchSize());

    uint8_t mac[6];
    WiFi.macAddress(mac);
    dprintf("WiFi StationAP Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    WiFi.softAPmacAddress(mac);
    dprintf("WiFi SoftAP Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    dprintln();
    dprintln();
}