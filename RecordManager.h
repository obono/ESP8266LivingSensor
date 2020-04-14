#pragma once

#include <EEPROM.h>

#include "DebugMacros.h"

#define RECORD_MAX  15
#define TABLE_SIZE  (RECORD_MAX + 1)

#define getRecordAdrs(idx)  (sizeof(T) * (idx) + TABLE_SIZE)

template <typename T>
class RecordManager
{
public:
    RecordManager(void)
    {
        EEPROM.begin(getRecordAdrs(RECORD_MAX));
        token = EEPROM.read(0);
        recordCount = 0;
        for (int i = 1; i < TABLE_SIZE; i++) {
            if (EEPROM.read(i) != token) {
                recordCount = i;
                break;
            }
        }
    }

    ~RecordManager()
    {
        EEPROM.end();
    }

    void displayInfo(void)
    {
        dprint(F("EEPROM size="));
        dprintln(EEPROM.length());
        dprint(F("Stored record(s)="));
        dprintln(recordCount);
        dprintln();
    }

    int getRecordsCount(void)
    {
        return recordCount;
    }

    bool isFull(void)
    {
        return (recordCount == RECORD_MAX);
    }

    void storeRecord(T &record, bool isForceFirst)
    {
        if (isForceFirst) recordCount = 0;
        EEPROM.put<T>(getRecordAdrs(recordCount), record);
        if (recordCount == 0) {
            uint8_t table[TABLE_SIZE];
            for (int i = 1; i < TABLE_SIZE; table[i++] = EEPROM.read(i)) { ; }
            int idx = 1;
            while (idx < TABLE_SIZE) {
                token = random(256);
                for (idx = 1; idx < TABLE_SIZE && table[idx] != token; idx++) { ; }
            }
        }
        EEPROM.write(recordCount, token);
        dprint(F("Store a record: index="));
        dprintln(recordCount);
        recordCount++;
    }

    String generateRecordsPayload(String pFunc(T &record))
    {
        String ret = "[";
        for (int i = 0; i < recordCount; i++) {
            T record;
            EEPROM.get<T>(getRecordAdrs(i), record);
            ret += pFunc(record);
            ret += (i == recordCount - 1) ? "]" : ",";
        }
        dprint(F("Generate records payload: samples="));
        dprintln(recordCount);
        return ret;
    }

    void clear(void)
    {
        for (int i = 1; i < TABLE_SIZE; i++) {
            EEPROM.write(i, token);
        }
        recordCount = 0;
        dprintln(F("Clear stored records."));
    }

    void finalize(void)
    {
        EEPROM.commit();
        dprintln(F("commit EEPROM"));
    }

private:
    uint8_t token;
    int     recordCount;
};
