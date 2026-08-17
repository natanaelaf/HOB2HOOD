#include "Timer.h"
#include "Motor.h"
#include "Light.h"
#include "Logger.h"

// Inicialização das variáveis estáticas
bool TimerModule::manualTimerActive = false;
unsigned long TimerModule::manualTimerStartTime = 0;
uint32_t TimerModule::manualTimerDurationSeconds = 0;

bool TimerModule::postExhaustionActive = false;
unsigned long TimerModule::postExhaustionStartTime = 0;
uint32_t TimerModule::postExhaustionDurationSeconds = 0;

void TimerModule::begin() {
    manualTimerActive = false;
    postExhaustionActive = false;
    Logger::info(F("Modulo Timer Inicializado."));
}

void TimerModule::startManualTimer(uint16_t minutes) {
    manualTimerActive = true;
    manualTimerStartTime = millis();
    manualTimerDurationSeconds = (uint32_t)minutes * 60;
    
    // Se a pós-exaustão estava ativa, cancela para dar prioridade ao timer manual
    if (postExhaustionActive) {
        stopPostExhaustion();
    }
    
    Logger::info(F("Timer: Iniciado timer manual de %d minutos (%ld segundos)."), (int)minutes, manualTimerDurationSeconds);
}

void TimerModule::stopManualTimer() {
    if (manualTimerActive) {
        manualTimerActive = false;
        Light::setBlinkMode(BLINK_NONE);
        Light::setCustomBlinkPeriod(0);
        Logger::info(F("Timer: Timer manual parado."));
    }
}

bool TimerModule::isManualTimerActive() {
    return manualTimerActive;
}

uint32_t TimerModule::getManualTimerRemainingSeconds() {
    if (!manualTimerActive) return 0;
    unsigned long elapsed = (millis() - manualTimerStartTime) / 1000;
    if (elapsed >= manualTimerDurationSeconds) return 0;
    return manualTimerDurationSeconds - elapsed;
}

void TimerModule::startPostExhaustion(uint16_t minutes) {
    // Pós-exaustão só inicia se o timer manual não estiver ativo (prioridades do sistema)
    if (manualTimerActive) {
        Logger::warn(F("Timer: Requisito de pos-exaustao ignorado pois timer manual esta ativo."));
        return;
    }
    
    postExhaustionActive = true;
    postExhaustionStartTime = millis();
    postExhaustionDurationSeconds = (uint32_t)minutes * 60;
    Logger::info(F("Timer: Iniciada pos-exaustao de %d minutos (%ld segundos) na velocidade 1."), (int)minutes, postExhaustionDurationSeconds);
}

void TimerModule::stopPostExhaustion() {
    if (postExhaustionActive) {
        postExhaustionActive = false;
        Logger::info(F("Timer: Pos-exaustao parada."));
    }
}

bool TimerModule::isPostExhaustionActive() {
    return postExhaustionActive;
}

void TimerModule::update() {
    // 1. Processa Timer Manual
    if (manualTimerActive) {
        unsigned long elapsed = (millis() - manualTimerStartTime) / 1000;
        
        if (elapsed >= manualTimerDurationSeconds) {
            Logger::info(F("Timer: Tempo esgotado! Desligando motor e lampada."));
            // Motor OFF, Lâmpada OFF
            Motor::setSpeed(SPEED_OFF);
            Light::setState(false);
            stopManualTimer();
        } else {
            // Atualiza indicação visual (blink do LED) baseado no tempo restante de forma progressiva
            uint32_t remaining = manualTimerDurationSeconds - elapsed;
            uint16_t period = 200; // Período mínimo de pisca (200 ms)
            
            // Define a janela máxima de aceleração (ex: 300 segundos = 5 minutos). 
            // Se o timer total for menor que isso, a janela máxima é o tempo total dele.
            uint32_t maxSeconds = (manualTimerDurationSeconds > 300) ? 300 : manualTimerDurationSeconds;
            
            if (remaining >= maxSeconds) {
                period = 2000; // Começa piscando lento (2 segundos)
            } else if (remaining > 0) {
                // Interpolação linear: reduz o período de 2000ms a 200ms conforme o tempo restante cai de maxSeconds a 0s
                period = 200 + ((uint32_t)remaining * 1800UL / maxSeconds);
            }
            
            Light::setCustomBlinkPeriod(period);
        }
    }

    // 2. Processa Pós-Exaustão
    if (postExhaustionActive) {
        unsigned long elapsed = (millis() - postExhaustionStartTime) / 1000;
        
        if (elapsed >= postExhaustionDurationSeconds) {
            Logger::info(F("Timer: Pos-exaustao finalizada. Desligando motor."));
            Motor::setSpeed(SPEED_OFF);
            stopPostExhaustion();
        }
    }
}
