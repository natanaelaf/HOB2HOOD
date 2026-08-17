#include "EEPROMModule.h"
#include <EEPROM.h>
#include "config.h"
#include "Logger.h"

void EEPROMModule::begin() {
    // No AVR (Arduino Uno), o hardware de EEPROM não necessita de begin() explícito.
    if (!isInitialized()) {
        initializeDefault();
    } else {
        Logger::info(F("Configuracoes recuperadas da EEPROM."));
    }
}

bool EEPROMModule::isInitialized() {
    return EEPROM.read(EEPROM_ADDR_MAGIC) == EEPROM_MAGIC_BYTE;
}

void EEPROMModule::initializeDefault() {
    Logger::info(F("EEPROM nao inicializada. Gravando padroes..."));
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC_BYTE);
    writeMode(false); // Inicia em Automático
    writeLightState(false); // Inicia com lâmpada desligada
    writePostExhaustionTime(DEFAULT_POST_EXH_MIN); // Pós-exaustão padrão (5 minutos)
    writeTimerTime(DEFAULT_TIMER_MIN); // Timer padrão (30 minutos)
}

bool EEPROMModule::readMode() {
    return EEPROM.read(EEPROM_ADDR_MODE) == 1;
}

void EEPROMModule::writeMode(bool manual) {
    EEPROM.update(EEPROM_ADDR_MODE, manual ? 1 : 0);
}

bool EEPROMModule::readLightState() {
    return EEPROM.read(EEPROM_ADDR_LIGHT) == 1;
}

void EEPROMModule::writeLightState(bool on) {
    EEPROM.update(EEPROM_ADDR_LIGHT, on ? 1 : 0);
}

uint16_t EEPROMModule::readPostExhaustionTime() {
    uint16_t val = 0;
    EEPROM.get(EEPROM_ADDR_POST_EXH, val);
    // Validação de segurança para valores corrompidos
    if (val == 0 || val > 1440) {
        val = DEFAULT_POST_EXH_MIN;
    }
    return val;
}

void EEPROMModule::writePostExhaustionTime(uint16_t minutes) {
    EEPROM.put(EEPROM_ADDR_POST_EXH, minutes);
}

uint16_t EEPROMModule::readTimerTime() {
    uint16_t val = 0;
    EEPROM.get(EEPROM_ADDR_TIMER, val);
    // Validação de segurança para valores corrompidos
    if (val == 0 || val > 1440) {
        val = DEFAULT_TIMER_MIN;
    }
    return val;
}

void EEPROMModule::writeTimerTime(uint16_t minutes) {
    EEPROM.put(EEPROM_ADDR_TIMER, minutes);
}
