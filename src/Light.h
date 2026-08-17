#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>

enum LightBlinkMode {
    BLINK_NONE,
    BLINK_NORMAL,     // Timer ativo (> 5 min)
    BLINK_WARNING,    // Timer nos últimos 5 min
    BLINK_CRITICAL    // Timer no último minuto
};

class Light {
public:
    static void begin();
    static void setState(bool on);
    static bool getState();
    static void setBlinkMode(LightBlinkMode mode);
    static void setCustomBlinkPeriod(uint16_t periodMs);
    static void update();

private:
    static bool isLightOn;
    static LightBlinkMode blinkMode;
    static uint16_t customBlinkPeriodMs;
    static unsigned long lastBlinkToggle;
    static bool blinkState;
};

#endif // LIGHT_H
