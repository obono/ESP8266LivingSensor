#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "HTTPSRedirect.h"
#include "DebugMacros.h"

class NetworkManager
{
public:
    NetworkManager(void);
    ~NetworkManager();
    bool    setupWiFi(void);
    bool    uploadRecords(String &recordsPayload);

private:
};
