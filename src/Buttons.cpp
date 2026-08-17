#include "Buttons.h"
#include "config.h"
#include "Logger.h"

// Mapeamento dos pinos dos botões
const uint8_t Buttons::pinMap[BTN_COUNT] = {
    PIN_BTN_OFF,
    PIN_BTN_V1,
    PIN_BTN_V2,
    PIN_BTN_V3,
    PIN_BTN_LIGHT
};

// Inicialização das variáveis estáticas
bool Buttons::lastPinState[BTN_COUNT] = {false};
bool Buttons::debouncedState[BTN_COUNT] = {false};
bool Buttons::prevDebouncedState[BTN_COUNT] = {false};
unsigned long Buttons::lastDebounceTime[BTN_COUNT] = {0};
unsigned long Buttons::pressStartTime[BTN_COUNT] = {0};
bool Buttons::longPressTriggered[BTN_COUNT] = {false};

bool Buttons::eventOffShort = false;
bool Buttons::eventOffLong = false;
bool Buttons::eventV1 = false;
bool Buttons::eventV2 = false;
bool Buttons::eventV3 = false;
bool Buttons::eventLightShort = false;
bool Buttons::eventLightLong = false;

void Buttons::begin() {
    for (int i = 0; i < BTN_COUNT; i++) {
        pinMode(pinMap[i], INPUT_PULLUP);
        // Leitura inicial
        lastPinState[i] = (digitalRead(pinMap[i]) == LOW);
        debouncedState[i] = lastPinState[i];
        prevDebouncedState[i] = lastPinState[i];
        lastDebounceTime[i] = 0;
        pressStartTime[i] = 0;
        longPressTriggered[i] = false;
    }
    clearEvents();
    Logger::info(F("Modulo de Botoes Inicializado (Debounce = 30ms)."));
}

void Buttons::update() {
    // 1. Faz a leitura física e o debounce individual
    for (int i = 0; i < BTN_COUNT; i++) {
        bool pinReading = (digitalRead(pinMap[i]) == LOW); // LOW significa pressionado devido ao Pull-up
        
        if (pinReading != lastPinState[i]) {
            lastDebounceTime[i] = millis();
        }
        lastPinState[i] = pinReading;

        if ((millis() - lastDebounceTime[i]) >= DEBOUNCE_MS) {
            debouncedState[i] = pinReading;
        }
    }

    // 2. Aplica as regras de tratamento de múltiplos botões
    bool rawOff = debouncedState[BTN_OFF];
    bool rawV1 = debouncedState[BTN_V1];
    bool rawV2 = debouncedState[BTN_V2];
    bool rawV3 = debouncedState[BTN_V3];
    bool rawLight = debouncedState[BTN_LIGHT];

    bool filteredState[BTN_COUNT] = {false};

    // A Lâmpada (Luz) é totalmente independente e sempre executa
    filteredState[BTN_LIGHT] = rawLight;

    // Se o OFF for pressionado, ele tem prioridade máxima. As velocidades são ignoradas.
    if (rawOff) {
        filteredState[BTN_OFF] = true;
        // filteredState das velocidades permanece false
    } else {
        // Avalia overlap de velocidades
        // V1+V2, V1+V3, V2+V3 -> Ignora até restar apenas um botão
        bool speedOverlap = (rawV1 && rawV2) || (rawV1 && rawV3) || (rawV2 && rawV3);
        if (!speedOverlap) {
            filteredState[BTN_V1] = rawV1;
            filteredState[BTN_V2] = rawV2;
            filteredState[BTN_V3] = rawV3;
        }
    }

    // 3. Detecção de borda e pressionamento longo baseados nos estados filtrados
    for (int i = 0; i < BTN_COUNT; i++) {
        bool state = filteredState[i];
        bool prevState = prevDebouncedState[i];

        // Detecta borda de descida (pressionou)
        if (state && !prevState) {
            pressStartTime[i] = millis();
            longPressTriggered[i] = false;
        }

        if (state) {
            // Se está pressionado, verifica pressionamento longo (3 segundos)
            if (!longPressTriggered[i] && (millis() - pressStartTime[i] >= LONG_PRESS_MS)) {
                longPressTriggered[i] = true;
                
                if (i == BTN_OFF) {
                    eventOffLong = true;
                    Logger::info(F("Botoes: Botao OFF pressionado por 3s."));
                } else if (i == BTN_LIGHT) {
                    eventLightLong = true;
                    Logger::info(F("Botoes: Botao Lampada pressionado por 3s."));
                }
            }
        } else if (!state && prevState) {
            // Detecta borda de subida (soltou)
            // Só gera evento de clique curto se NÃO disparou o pressionamento longo
            if (!longPressTriggered[i]) {
                if (i == BTN_OFF) {
                    eventOffShort = true;
                } else if (i == BTN_LIGHT) {
                    eventLightShort = true;
                } else if (i == BTN_V1) {
                    eventV1 = true;
                } else if (i == BTN_V2) {
                    eventV2 = true;
                } else if (i == BTN_V3) {
                    eventV3 = true;
                }
            }
        }

        prevDebouncedState[i] = state;
    }
}

bool Buttons::consumeOffShort() {
    bool val = eventOffShort;
    eventOffShort = false;
    return val;
}

bool Buttons::consumeOffLong() {
    bool val = eventOffLong;
    eventOffLong = false;
    return val;
}

bool Buttons::consumeV1() {
    bool val = eventV1;
    eventV1 = false;
    return val;
}

bool Buttons::consumeV2() {
    bool val = eventV2;
    eventV2 = false;
    return val;
}

bool Buttons::consumeV3() {
    bool val = eventV3;
    eventV3 = false;
    return val;
}

bool Buttons::consumeLightShort() {
    bool val = eventLightShort;
    eventLightShort = false;
    return val;
}

bool Buttons::consumeLightLong() {
    bool val = eventLightLong;
    eventLightLong = false;
    return val;
}

void Buttons::clearEvents() {
    eventOffShort = false;
    eventOffLong = false;
    eventV1 = false;
    eventV2 = false;
    eventV3 = false;
    eventLightShort = false;
    eventLightLong = false;
}
