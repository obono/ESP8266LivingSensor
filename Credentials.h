#pragma once

#define MY_SSID     "SSID"
#define MY_PASSWORD "PASSWORD"
//#define STATIC_ADDRESS

#ifdef STATIC_ADDRESS
IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 169, 1, 1); 
#endif

#define GAS_HOST    "script.google.com"
#define GAS_PATH    "/macros/s/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/exec"
#define GAS_TOKEN   "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
