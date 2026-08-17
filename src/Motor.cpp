#include "Motor.h"
#include "config.h"
#include "Logger.h"

MotorSpeed Motor::currentSpeed = SPEED_OFF;
MotorSpeed Motor::targetSpeed = SPEED_OFF;
MotorState Motor::state = MOTOR_STATE_IDLE;
unsigned long Motor::transitionStartTime = 0;

void Motor::begin() {
    // Inicializa pinos dos relés
    pinMode(PIN_RELAY_V1, OUTPUT);
    pinMode(PIN_RELAY_V2, OUTPUT);
    pinMode(PIN_RELAY_V3, OUTPUT);
    
    // Inicializa pinos dos LEDs de velocidade (caso configurados)
    if (PIN_LED_V1 != -1) pinMode(PIN_LED_V1, OUTPUT);
    if (PIN_LED_V2 != -1) pinMode(PIN_LED_V2, OUTPUT);
    if (PIN_LED_V3 != -1) pinMode(PIN_LED_V3, OUTPUT);

    // Garante que tudo inicia desligado
    writeRelays(SPEED_OFF);
    updateLEDs(SPEED_OFF);

    currentSpeed = SPEED_OFF;
    targetSpeed = SPEED_OFF;
    state = MOTOR_STATE_IDLE;

    Logger::info(F("Modulo Motor Inicializado (Relés e LEDs OFF)."));
}

void Motor::setSpeed(MotorSpeed speed) {
    if (state == MOTOR_STATE_IDLE && currentSpeed == speed) {
        return; // Sem alteração
    }
    
    if (state == MOTOR_STATE_INTERLOCK && targetSpeed == speed) {
        return; // Já está transitando para essa velocidade
    }

    Logger::info(F("Motor: Requisitada mudanca de velocidade %d -> %d"), (int)currentSpeed, (int)speed);

    // Passo 1 do intertravamento: Desliga todos os relés imediatamente
    writeRelays(SPEED_OFF);
    
    // Prepara transição
    targetSpeed = speed;
    state = MOTOR_STATE_INTERLOCK;
    transitionStartTime = millis();
}

MotorSpeed Motor::getSpeed() {
    return currentSpeed;
}

bool Motor::isTransitioning() {
    return state == MOTOR_STATE_INTERLOCK;
}

void Motor::update() {
    if (state == MOTOR_STATE_INTERLOCK) {
        // Passo 2: Aguarda 100 ms de forma não-bloqueante
        if (millis() - transitionStartTime >= 100) {
            // Passo 3: Liga o relé da nova velocidade
            writeRelays(targetSpeed);
            currentSpeed = targetSpeed;
            state = MOTOR_STATE_IDLE;
            
            updateLEDs(currentSpeed);
            Logger::info(F("Motor: Velocidade definida com sucesso para %d"), (int)currentSpeed);
        }
    }
}

void Motor::writeRelays(MotorSpeed speed) {
    // Garante que nunca existirão duas velocidades ligadas simultaneamente
    digitalWrite(PIN_RELAY_V1, RELAY_OFF);
    digitalWrite(PIN_RELAY_V2, RELAY_OFF);
    digitalWrite(PIN_RELAY_V3, RELAY_OFF);

    switch (speed) {
        case SPEED_V1:
            digitalWrite(PIN_RELAY_V1, RELAY_ON);
            break;
        case SPEED_V2:
            digitalWrite(PIN_RELAY_V2, RELAY_ON);
            break;
        case SPEED_V3:
            digitalWrite(PIN_RELAY_V3, RELAY_ON);
            break;
        case SPEED_OFF:
        default:
            break;
    }
}

void Motor::updateLEDs(MotorSpeed speed) {
    if (PIN_LED_V1 != -1) digitalWrite(PIN_LED_V1, (speed == SPEED_V1) ? LED_ON : LED_OFF_STATE);
    if (PIN_LED_V2 != -1) digitalWrite(PIN_LED_V2, (speed == SPEED_V2) ? LED_ON : LED_OFF_STATE);
    if (PIN_LED_V3 != -1) digitalWrite(PIN_LED_V3, (speed == SPEED_V3) ? LED_ON : LED_OFF_STATE);
}
