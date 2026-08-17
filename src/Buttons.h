#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum ButtonId {
    BTN_OFF = 0,
    BTN_V1 = 1,
    BTN_V2 = 2,
    BTN_V3 = 3,
    BTN_LIGHT = 4,
    BTN_COUNT = 5
};

class Buttons {
public:
    static void begin();
    static void update();
    
    // Consulta e consome eventos
    static bool consumeOffShort();
    static bool consumeOffLong();
    static bool consumeV1();
    static bool consumeV2();
    static bool consumeV3();
    static bool consumeLightShort();
    static bool consumeLightLong();

    static void clearEvents();

private:
    static const uint8_t pinMap[BTN_COUNT];
    
    // Dados de debounce para cada botão
    static bool lastPinState[BTN_COUNT];
    static bool debouncedState[BTN_COUNT];
    static bool prevDebouncedState[BTN_COUNT];
    static unsigned long lastDebounceTime[BTN_COUNT];
    
    // Dados de tempo de pressionamento
    static unsigned long pressStartTime[BTN_COUNT];
    static bool longPressTriggered[BTN_COUNT];

    // Flags de eventos
    static bool eventOffShort;
    static bool eventOffLong;
    static bool eventV1;
    static bool eventV2;
    static bool eventV3;
    static bool eventLightShort;
    static bool eventLightLong;
};

#endif // BUTTONS_H
