#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>

class Watchdog {
public:
    static void begin();
    static void feed();
};

#endif // WATCHDOG_H
