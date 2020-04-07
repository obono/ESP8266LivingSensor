#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "HTTPSRedirect.h"
#include "DebugMacros.h"

class MyNetworkManager
{
public:
    MyNetworkManager(void);
    ~MyNetworkManager();
    void    setupWiFi(void);
    void    doPost(float illum, float temp);

private:
    ESP8266WiFiMulti    *pWiFiMulti;
};
