#include "Watchdog.h"
#include <avr/wdt.h>
#include "Logger.h"

void Watchdog::begin() {
    // Configura e habilita o watchdog com timeout de 2 segundos
    wdt_enable(WDTO_2S);
    Logger::info(F("Watchdog de hardware habilitado (Timeout = 2s)."));
}

void Watchdog::feed() {
    wdt_reset();
}
