#include "MyNetworkManager.h"
#include "Credentials.h"

#define HTTPS_PORT  443
#define PAYLOAD_BUF 256

MyNetworkManager::MyNetworkManager(void)
{
    pWiFiMulti = new ESP8266WiFiMulti();
}

MyNetworkManager::~MyNetworkManager()
{
    delete pWiFiMulti;
}

void MyNetworkManager::setupWiFi(void)
{
    delay(5000);
    WiFi.mode(WIFI_STA);
    pWiFiMulti->addAP(MY_SSID, MY_PASSWORD);
}

void MyNetworkManager::doPost(float illum, float temp)
{
    if (pWiFiMulti->run() != WL_CONNECTED) return;
    HTTPSRedirect* pClient = new HTTPSRedirect(HTTPS_PORT);
    if (pClient == nullptr) return;

    pClient->setInsecure();
    pClient->setContentTypeHeader("application/json");
    if (pClient->connect(GAS_HOST, HTTPS_PORT)) {
        dprintln("Calling API...");
        char payload[PAYLOAD_BUF];
        sprintf(payload, "{\"token\":\"" GAS_TOKEN "\",\"records\":[{\"illum\":%.2f,\"temp\":%.2f}]}", illum, temp);
        bool isCalled = pClient->POST(GAS_PATH, GAS_HOST, payload, false);
        if (isCalled) {
            int httpCode = pClient->getStatusCode();
            if (httpCode == HTTP_CODE_OK) {
                String response = pClient->getResponseBody();
                dprint(response);
            } else {
                dprint("Error! status=");
                dprintln(httpCode);
            }
        } else {
            dprintln("Fatal Error!!");
        }
    } else {
        dprintln("Connecting failed!!");
    }
    dprintln();

    delete pClient;
    pClient = nullptr;
}
