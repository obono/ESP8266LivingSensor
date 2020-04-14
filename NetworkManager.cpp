#include "NetworkManager.h"
#include "Credentials.h"

#define WIFI_BOOT_WAIT      2500
#define WIFI_RETRY_COUNT    10
#define WIFI_RETRY_WAIT     500

#define HTTPS_PORT          443
#define HTTP_CONTENT_TYPE   "application/json"
#define HTTP_RETRY_COUNT    3
#define HTTP_SUCCESS_RESPONSE "Success\r\n"

NetworkManager::NetworkManager(void)
{
    // do nothing
}

NetworkManager::~NetworkManager()
{
    // do nothing
}

bool NetworkManager::setupWiFi(void)
{
    delay(WIFI_BOOT_WAIT);
    WiFi.mode(WIFI_STA);
    WiFi.begin(MY_SSID, MY_PASSWORD);
    bool ret = false;
    for (int i = 0; i < WIFI_RETRY_COUNT; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            ret = true;
            dprintln(F("Connected to SSID \"" MY_SSID "\""));
            break;
        }
        delay(WIFI_RETRY_WAIT);
    }
    return ret;
}

bool NetworkManager::uploadRecords(String &recordsPayload)
{
    HTTPSRedirect* pClient = new HTTPSRedirect(HTTPS_PORT);
    if (pClient == nullptr) return false;

    bool ret = false;
    pClient->setInsecure();
    pClient->setContentTypeHeader(HTTP_CONTENT_TYPE);
    String payload = "{\"token\":\"" GAS_TOKEN "\",\"records\":" + recordsPayload + "}";
    for (int i = 0; !ret && i < HTTP_RETRY_COUNT; i++) {
        dprint(F("Connecting..."));
        if (pClient->connect(GAS_HOST, HTTPS_PORT)) {
            dprint(F("Posting..."));
            bool isCalled = pClient->POST(GAS_PATH, GAS_HOST, payload, false);
            if (isCalled) {
                int httpCode = pClient->getStatusCode();
                dprint(F("HTTP status="));
                dprintln(httpCode);
                if (httpCode == HTTP_CODE_OK) {
                    String response = pClient->getResponseBody();
                    dprint(response);
                    ret = (response != NULL && response.equals(HTTP_SUCCESS_RESPONSE));
                }
            } else {
                dprintln(F("Failed!!"));
            }
        } else {
            dprintln(F("Failed!!"));
        }
    }
    dprintln();

    delete pClient;
    return ret;
}
