#include "IR.h"
#include "config.h"
#include "Logger.h"

// Inicialização das variáveis estáticas e voláteis
volatile uint16_t IR::rawBuffer[64] = {0};
volatile uint8_t IR::rawCount = 0;
volatile uint32_t IR::lastInterruptTime = 0;
volatile bool IR::frameReady = false;

void IR::begin() {
    pinMode(PIN_IR_RECV, INPUT_PULLUP);
    
    noInterrupts();
    rawCount = 0;
    frameReady = false;
    lastInterruptTime = micros();
    interrupts();
    
    // Associa a interrupção no pino D2 (INT0) para qualquer mudança de estado (CHANGE)
    attachInterrupt(digitalPinToInterrupt(PIN_IR_RECV), handleInterrupt, CHANGE);
    
    Logger::info(F("Modulo IR Inicializado no pino %d (Interrupcao CHANGE)."), PIN_IR_RECV);
}

void IR::handleInterrupt() {
    uint32_t now = micros();
    uint32_t duration = now - lastInterruptTime;

    if (frameReady) {
        lastInterruptTime = now;
        return; // Buffer cheio aguardando consumo
    }

    // Filtro de glitch: ignora transições muito curtas (ruídos < 250us)
    // Ao ignorar a transição sem atualizar o lastInterruptTime, a próxima transição
    // acumulará este tempo, efetivamente "reconstruindo" o sinal original filtrado.
    if (duration < 250) {
        return;
    }

    lastInterruptTime = now;

    if (rawCount < 64) {
        rawBuffer[rawCount++] = (uint16_t)duration;
    }
}

void IR::update() {
    // Se recebemos pulsos e ainda não finalizamos o frame
    if (rawCount > 0 && !frameReady) {
        uint32_t now = micros();
        // Se o sinal ficou em nível HIGH por mais de 6000 us (6 ms), a transmissão terminou
        if (now - lastInterruptTime > 6000) {
            frameReady = true;
        }
    }
}

bool IR::isFrameReady() {
    return frameReady;
}

void IR::getFrame(uint16_t* destBuffer, uint8_t& count) {
    noInterrupts();
    count = rawCount;
    for (uint8_t i = 0; i < rawCount; i++) {
        destBuffer[i] = rawBuffer[i];
    }
    interrupts();
}

void IR::resetFrame() {
    noInterrupts();
    rawCount = 0;
    frameReady = false;
    lastInterruptTime = micros();
    interrupts();
}
