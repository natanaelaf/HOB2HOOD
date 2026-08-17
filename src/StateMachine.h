#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

class StateMachine {
public:
    static void begin();
    static void update();
    static bool isSafeMode();
    static bool isTimerProgramming();

private:
    static void toggleMode();
    static void handleButtonEvents();
    static void handleHob2HoodEvents();
    static void updateModeLED();
    static void runSafetyCheck();
    static void enterSafeMode();

    static bool isManual;
    static bool safeModeActive;
    
    // Controle do pisca do LED de modo na transição
    static bool modeBlinkActive;
    static unsigned long lastModeBlinkToggle;
    static uint8_t modeBlinkToggles;
    static bool modeBlinkState;

    // Estado de programação do Timer Manual
    static bool timerProgrammingActive;
    static unsigned long timerProgrammingStartTime;
};

#endif // STATE_MACHINE_H
