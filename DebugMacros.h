#pragma once

#define SERIAL_BAUD_RATE    74880 // 115200
#define DEBUG
//#define DEBUG_HTTPS_REDIRECT

#ifdef DEBUG
    #define dprint(...)     Serial.print(__VA_ARGS__)
    #define dprintln(...)   Serial.println(__VA_ARGS__)
    #define dprintf(...)    Serial.printf(__VA_ARGS__)
#else
    #define dprint(...)
    #define dprintln(...)
    #define dprintf(...)
#endif

#ifdef DEBUG_HTTPS_REDIRECT
    #define DPRINT(...)     Serial.print(__VA_ARGS__)
    #define DPRINTLN(...)   Serial.println(__VA_ARGS__)
#else
    #define DPRINT(...)
    #define DPRINTLN(...)
#endif
