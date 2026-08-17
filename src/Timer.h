#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include <Arduino.h>

class TimerModule {
public:
    static void begin();
    static void update();

    // Timer Manual de Desligamento
    static void startManualTimer(uint16_t minutes);
    static void stopManualTimer();
    static bool isManualTimerActive();
    static uint32_t getManualTimerRemainingSeconds();

    // Pós-Exaustão
    static void startPostExhaustion(uint16_t minutes);
    static void stopPostExhaustion();
    static bool isPostExhaustionActive();

private:
    // Dados do Timer Manual
    static bool manualTimerActive;
    static unsigned long manualTimerStartTime;
    static uint32_t manualTimerDurationSeconds;

    // Dados da Pós-Exaustão
    static bool postExhaustionActive;
    static unsigned long postExhaustionStartTime;
    static uint32_t postExhaustionDurationSeconds;
};

#endif // TIMER_MODULE_H
