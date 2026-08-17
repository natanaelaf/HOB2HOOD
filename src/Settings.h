#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

class Settings {
public:
    static void begin();
    
    static bool isManualMode();
    static void setManualMode(bool manual);

    static bool getLightState();
    static void setLightState(bool on);

    static uint16_t getPostExhaustionTime();
    static void setPostExhaustionTime(uint16_t minutes);

    static uint16_t getTimerTime();
    static void setTimerTime(uint16_t minutes);

private:
    static bool manualMode;
    static bool lightState;
    static uint16_t postExhaustionTime;
    static uint16_t timerTime;
};

#endif // SETTINGS_H
