#include "Settings.h"
#include "EEPROMModule.h"
#include "config.h"
#include "Logger.h"

bool Settings::manualMode = false;
bool Settings::lightState = false;
uint16_t Settings::postExhaustionTime = DEFAULT_POST_EXH_MIN;
uint16_t Settings::timerTime = DEFAULT_TIMER_MIN;

void Settings::begin() {
    EEPROMModule::begin();
    manualMode = EEPROMModule::readMode();
    lightState = EEPROMModule::readLightState();
    postExhaustionTime = EEPROMModule::readPostExhaustionTime();
    timerTime = EEPROMModule::readTimerTime();
    
    Logger::info(F("Configuracoes Carregadas:"));
    Logger::info(F("  Modo: %s"), manualMode ? "Manual" : "Automatico");
    Logger::info(F("  Luz: %s"), lightState ? "LIGADA" : "DESLIGADA");
    Logger::info(F("  Pos-Exaustao: %d min"), (int)postExhaustionTime);
    Logger::info(F("  Timer Padrao: %d min"), (int)timerTime);
}

bool Settings::isManualMode() {
    return manualMode;
}

void Settings::setManualMode(bool manual) {
    if (manualMode != manual) {
        manualMode = manual;
        EEPROMModule::writeMode(manual);
        Logger::info(F("Modo de operacao alterado na EEPROM para: %s"), manual ? "Manual" : "Automatico");
    }
}

bool Settings::getLightState() {
    return lightState;
}

void Settings::setLightState(bool on) {
    if (lightState != on) {
        lightState = on;
        EEPROMModule::writeLightState(on);
        Logger::info(F("Estado da lâmpada salvo na EEPROM: %s"), on ? "LIGADA" : "DESLIGADA");
    }
}

uint16_t Settings::getPostExhaustionTime() {
    return postExhaustionTime;
}

void Settings::setPostExhaustionTime(uint16_t minutes) {
    if (postExhaustionTime != minutes) {
        postExhaustionTime = minutes;
        EEPROMModule::writePostExhaustionTime(minutes);
        Logger::info(F("Tempo de pos-exaustao salvo na EEPROM: %d min"), (int)minutes);
    }
}

uint16_t Settings::getTimerTime() {
    return timerTime;
}

void Settings::setTimerTime(uint16_t minutes) {
    if (timerTime != minutes) {
        timerTime = minutes;
        EEPROMModule::writeTimerTime(minutes);
        Logger::info(F("Tempo de timer padrao salvo na EEPROM: %d min"), (int)minutes);
    }
}
