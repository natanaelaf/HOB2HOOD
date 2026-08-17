#include "Light.h"
#include "config.h"
#include "Settings.h"
#include "Logger.h"
#include "StateMachine.h"

bool Light::isLightOn = false;
LightBlinkMode Light::blinkMode = BLINK_NONE;
uint16_t Light::customBlinkPeriodMs = 0;
unsigned long Light::lastBlinkToggle = 0;
bool Light::blinkState = false;

void Light::begin() {
    pinMode(PIN_RELAY_LIGHT, OUTPUT);
    if (PIN_LED_LIGHT != -1) {
        pinMode(PIN_LED_LIGHT, OUTPUT);
    }
    
    // Restaura o estado salvo na EEPROM
    isLightOn = Settings::getLightState();
    
    digitalWrite(PIN_RELAY_LIGHT, isLightOn ? RELAY_ON : RELAY_OFF);
    if (PIN_LED_LIGHT != -1) {
        digitalWrite(PIN_LED_LIGHT, isLightOn ? LED_ON : LED_OFF_STATE);
    }
    
    blinkMode = BLINK_NONE;
    customBlinkPeriodMs = 0;
    Logger::info(F("Modulo Lampada Inicializado. Estado inicial: %s"), isLightOn ? "ON" : "OFF");
}

void Light::setState(bool on) {
    if (isLightOn != on) {
        isLightOn = on;
        Settings::setLightState(on);
        
        // Atualiza a saída imediatamente se não estiver piscando
        if (blinkMode == BLINK_NONE) {
            digitalWrite(PIN_RELAY_LIGHT, isLightOn ? RELAY_ON : RELAY_OFF);
            if (PIN_LED_LIGHT != -1) {
                digitalWrite(PIN_LED_LIGHT, isLightOn ? LED_ON : LED_OFF_STATE);
            }
        }
    }
}

bool Light::getState() {
    return isLightOn;
}

void Light::setBlinkMode(LightBlinkMode mode) {
    if (blinkMode != mode) {
        blinkMode = mode;
        Logger::info(F("Lampada: Modo de pisca alterado para %d"), (int)mode);
    }
}

void Light::setCustomBlinkPeriod(uint16_t periodMs) {
    customBlinkPeriodMs = periodMs;
}

void Light::update() {
    // Garante que o relé da lâmpada está sempre no estado correto (independente de pisca)
    digitalWrite(PIN_RELAY_LIGHT, isLightOn ? RELAY_ON : RELAY_OFF);

    // 1. Se a programação do timer está ativa (LED pisca freneticamente aguardando velocidade)
    if (StateMachine::isTimerProgramming()) {
        unsigned long now = millis();
        // Pisca frenético de indicação de configuração (100ms ON, 100ms OFF = 5Hz)
        bool flashActive = ((now % 200) < 100);
        bool ledState = flashActive ? LED_ON : LED_OFF_STATE;
        
        if (PIN_LED_LIGHT != -1) {
            digitalWrite(PIN_LED_LIGHT, ledState);
        }
        return;
    }

    if (blinkMode == BLINK_NONE && customBlinkPeriodMs == 0) {
        // Sem piscar: LED segue o estado físico da lâmpada
        if (PIN_LED_LIGHT != -1) {
            digitalWrite(PIN_LED_LIGHT, isLightOn ? LED_ON : LED_OFF_STATE);
        }
        blinkState = false;
        lastBlinkToggle = 0;
        return;
    }

    // Lógica de pisca (Brief Flash) periódica sem bloquear
    unsigned long now = millis();
    unsigned long period = 2000; // BLINK_NORMAL
    
    if (customBlinkPeriodMs > 0) {
        period = customBlinkPeriodMs;
    } else {
        if (blinkMode == BLINK_WARNING) {
            period = 1000;
        } else if (blinkMode == BLINK_CRITICAL) {
            period = 250;
        }
    }

    // Ajusta a duração do flash ativo se o período for muito curto
    unsigned long flashDuration = 100;
    if (period < 200) {
        flashDuration = period / 2;
    }

    // Inicialização do controle temporal na primeira rodada
    if (lastBlinkToggle == 0) {
        lastBlinkToggle = now;
        blinkState = false;
    }

    // Máquina de estados baseada em delta de tempo (previne jitter ao alterar o período dinamicamente)
    if (blinkState) {
        // Estado ativo (flash curto de indicação)
        if (now - lastBlinkToggle >= flashDuration) {
            blinkState = false;
            lastBlinkToggle = now;
        }
    } else {
        // Estado inativo (espera o resto do período)
        unsigned long inactiveDuration = period - flashDuration;
        if (now - lastBlinkToggle >= inactiveDuration) {
            blinkState = true;
            lastBlinkToggle = now;
        }
    }

    bool ledState;
    if (blinkState) {
        // Se a luz está ligada, dá um breve flash desligando. Se está desligada, dá um breve flash ligando.
        ledState = isLightOn ? LED_OFF_STATE : LED_ON;
    } else {
        // Estado normal
        ledState = isLightOn ? LED_ON : LED_OFF_STATE;
    }

    if (PIN_LED_LIGHT != -1) {
        digitalWrite(PIN_LED_LIGHT, ledState);
    }
}
