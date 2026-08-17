#ifndef EEPROM_MODULE_H
#define EEPROM_MODULE_H

#include <Arduino.h>

class EEPROMModule {
public:
    static void begin();
    static bool isInitialized();
    static void initializeDefault();
    
    static bool readMode(); // false = Auto, true = Manual
    static void writeMode(bool manual);

    static bool readLightState(); // false = OFF, true = ON
    static void writeLightState(bool on);

    static uint16_t readPostExhaustionTime();
    static void writePostExhaustionTime(uint16_t minutes);

    static uint16_t readTimerTime();
    static void writeTimerTime(uint16_t minutes);
};

#endif // EEPROM_MODULE_H
