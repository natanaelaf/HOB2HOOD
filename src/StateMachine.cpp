#include "StateMachine.h"
#include "config.h"
#include "Buttons.h"
#include "Motor.h"
#include "Light.h"
#include "Hob2Hood.h"
#include "Timer.h"
#include "Settings.h"
#include "Watchdog.h"
#include "Logger.h"
#include <avr/wdt.h>

// Inicialização das variáveis estáticas
bool StateMachine::isManual = false;
bool StateMachine::safeModeActive = false;
bool StateMachine::modeBlinkActive = false;
unsigned long StateMachine::lastModeBlinkToggle = 0;
uint8_t StateMachine::modeBlinkToggles = 0;
bool StateMachine::modeBlinkState = false;
bool StateMachine::timerProgrammingActive = false;
unsigned long StateMachine::timerProgrammingStartTime = 0;

void StateMachine::begin() {
    wdt_disable(); // Desativa o watchdog imediatamente para evitar boot loop
    Logger::begin(115200);
    Logger::info(F("IE60P Smart Controller v1.0 - Inicializando..."));

    // Inicializa pino do LED OFF
    pinMode(PIN_LED_OFF, OUTPUT);

    // Inicializa Configurações e EEPROM primeiro
    Settings::begin();
    
    // Recupera o modo inicial
    isManual = Settings::isManualMode();
    digitalWrite(PIN_LED_OFF, isManual ? LED_ON : LED_OFF_STATE);

    // Inicializa os demais módulos
    Buttons::begin();
    Motor::begin();
    Light::begin();
    Hob2Hood::begin();
    TimerModule::begin();
    
    safeModeActive = false;
    modeBlinkActive = false;
    timerProgrammingActive = false;

    // Habilita Watchdog no final
    Watchdog::begin();
    
    Logger::info(F("Sistema Inicializado com Sucesso."));
}

bool StateMachine::isTimerProgramming() {
    return timerProgrammingActive;
}

void StateMachine::toggleMode() {
    isManual = !isManual;
    Settings::setManualMode(isManual);
    
    // Dispara animação assíncrona de 3 piscadas no LED OFF
    modeBlinkActive = true;
    lastModeBlinkToggle = millis();
    modeBlinkToggles = 0;
    modeBlinkState = true;
    digitalWrite(PIN_LED_OFF, modeBlinkState ? LED_ON : LED_OFF_STATE);
    
    Logger::info(F("Modo de operacao alterado para: %s"), isManual ? "MANUAL" : "AUTOMATICO");
}

void StateMachine::updateModeLED() {
    if (modeBlinkActive) {
        if (millis() - lastModeBlinkToggle >= 200) { // Alterna a cada 200ms
            lastModeBlinkToggle = millis();
            modeBlinkState = !modeBlinkState;
            digitalWrite(PIN_LED_OFF, modeBlinkState ? LED_ON : LED_OFF_STATE);
            
            modeBlinkToggles++;
            if (modeBlinkToggles >= 6) { // 6 alternâncias = 3 piscadas completas
                modeBlinkActive = false;
                // Deixa no estado estático correspondente ao modo
                digitalWrite(PIN_LED_OFF, isManual ? LED_ON : LED_OFF_STATE);
            }
        }
    } else {
        // Estado estático padrão
        digitalWrite(PIN_LED_OFF, isManual ? LED_ON : LED_OFF_STATE);
    }
}

void StateMachine::handleButtonEvents() {
    // Se a programação do timer está ativa, interceptamos os botões para configurar o tempo
    if (timerProgrammingActive) {
        if (Buttons::consumeV1()) {
            timerProgrammingActive = false;
            TimerModule::startManualTimer(1);
            Logger::info(F("Timer: Velocidade 1 selecionada. Timer configurado para 1 min."));
            return;
        } else if (Buttons::consumeV2()) {
            timerProgrammingActive = false;
            TimerModule::startManualTimer(2);
            Logger::info(F("Timer: Velocidade 2 selecionada. Timer configurado para 2 min."));
            return;
        } else if (Buttons::consumeV3()) {
            timerProgrammingActive = false;
            TimerModule::startManualTimer(3);
            Logger::info(F("Timer: Velocidade 3 selecionada. Timer configurado para 3 min."));
            return;
        } else if (Buttons::consumeLightShort()) {
            timerProgrammingActive = false;
            TimerModule::startManualTimer(5);
            Logger::info(F("Timer: Botao Luz pressionado. Timer configurado para o padrao de 5 min."));
            return;
        } else if (Buttons::consumeOffShort()) {
            timerProgrammingActive = false;
            Logger::info(F("Timer: Botao OFF pressionado. Programacao cancelada."));
            return;
        }
        // Descarta pressionamentos longos acidentais enquanto programa
        Buttons::consumeOffLong();
        Buttons::consumeLightLong();
        return;
    }

    // 1. Botão OFF
    if (Buttons::consumeOffLong()) {
        // OFF pressionado por 3s altera o modo
        toggleMode();
    } else if (Buttons::consumeOffShort()) {
        Logger::info(F("Botoes: Clique OFF. Desligando motor e timers."));
        Motor::setSpeed(SPEED_OFF);
        TimerModule::stopManualTimer();
        TimerModule::stopPostExhaustion();
    }

    // 2. Velocidades (Só processa se o OFF não foi acionado na mesma rodada)
    if (Buttons::consumeV1()) {
        Logger::info(F("Botoes: Clique V1."));
        TimerModule::stopPostExhaustion(); // Comando manual cancela pós-exaustão
        Motor::setSpeed(SPEED_V1);
    } else if (Buttons::consumeV2()) {
        Logger::info(F("Botoes: Clique V2."));
        TimerModule::stopPostExhaustion();
        Motor::setSpeed(SPEED_V2);
    } else if (Buttons::consumeV3()) {
        Logger::info(F("Botoes: Clique V3."));
        TimerModule::stopPostExhaustion();
        Motor::setSpeed(SPEED_V3);
    }

    // 3. Lâmpada
    if (Buttons::consumeLightLong()) {
        // Lâmpada por 3s entra no modo de programação do timer
        timerProgrammingActive = true;
        timerProgrammingStartTime = millis();
        Logger::info(F("Timer: Modo de programacao ativado. Aguardando velocidade (V1=1min, V2=2min, V3=3min, Luz=5min)..."));
    } else if (Buttons::consumeLightShort()) {
        Logger::info(F("Botoes: Clique Lampada. Alternando estado."));
        Light::setState(!Light::getState());
    }
}

void StateMachine::handleHob2HoodEvents() {
    Hob2HoodCmd cmd = Hob2Hood::consumeCommand();
    
    if (cmd == H2H_CMD_NONE) {
        return;
    }

    // Regra 1: Hob2Hood é ignorado no Modo Manual
    if (isManual) {
        Logger::info(F("Hob2Hood: Comando recebido mas ignorado (Modo Manual Ativo)."));
        return;
    }

    // Regra 2: Prioridade do Timer
    // Se o Timer Manual estiver ativo, ignora comandos Hob2Hood
    if (TimerModule::isManualTimerActive()) {
        Logger::info(F("Hob2Hood: Comando ignorado (Timer Manual esta ativo)."));
        return;
    }

    Logger::info(F("Hob2Hood: Processando comando %d no Modo Automatico."), (int)cmd);

    switch (cmd) {
        case H2H_CMD_ON:
            TimerModule::stopPostExhaustion();
            Light::setState(true);
            break;
            
        case H2H_CMD_OFF:
            Light::setState(false);
            // Pós-exaustão: se o motor estiver ligado, vai para V1 por tempo configurado
            if (Motor::getSpeed() != SPEED_OFF && !TimerModule::isPostExhaustionActive()) {
                Motor::setSpeed(SPEED_V1);
                TimerModule::startPostExhaustion(Settings::getPostExhaustionTime());
            } else if (Motor::getSpeed() == SPEED_OFF) {
                Motor::setSpeed(SPEED_OFF);
            }
            break;
            
        case H2H_CMD_V0:
            // V0 é apenas desligar o ventilador (Fan Off) manualmente no painel do cooktop:
            // - Desliga o motor imediatamente (sem pós-exaustão)
            // - Mantém o estado atual da lâmpada (não desliga a luz)
            Motor::setSpeed(SPEED_OFF);
            TimerModule::stopPostExhaustion();
            break;
            
        case H2H_CMD_V1:
            TimerModule::stopPostExhaustion();
            Motor::setSpeed(SPEED_V1);
            break;
            
        case H2H_CMD_V2:
            TimerModule::stopPostExhaustion();
            Motor::setSpeed(SPEED_V2);
            break;
            
        case H2H_CMD_V3:
        case H2H_CMD_V4:
            TimerModule::stopPostExhaustion();
            Motor::setSpeed(SPEED_V3);
            break;
            
        default:
            break;
    }
}

void StateMachine::runSafetyCheck() {
    // Verifica estado lógico impossível (mais de um relé de velocidade ativo)
    int activeRelays = 0;
    if (digitalRead(PIN_RELAY_V1) == RELAY_ON) activeRelays++;
    if (digitalRead(PIN_RELAY_V2) == RELAY_ON) activeRelays++;
    if (digitalRead(PIN_RELAY_V3) == RELAY_ON) activeRelays++;

    if (activeRelays > 1) {
        Logger::error(F("SEGURANCA: Estado impossivel detectado (%d reles ativos)! Entrando em Modo Seguro."), activeRelays);
        enterSafeMode();
    }
}

void StateMachine::enterSafeMode() {
    safeModeActive = true;
    
    // Desliga absolutamente tudo imediatamente
    digitalWrite(PIN_RELAY_V1, RELAY_OFF);
    digitalWrite(PIN_RELAY_V2, RELAY_OFF);
    digitalWrite(PIN_RELAY_V3, RELAY_OFF);
    digitalWrite(PIN_RELAY_LIGHT, RELAY_OFF);
    
    // Para timers
    TimerModule::stopManualTimer();
    TimerModule::stopPostExhaustion();
}

bool StateMachine::isSafeMode() {
    return safeModeActive;
}

void StateMachine::update() {
    // Alimenta o Watchdog em todas as iterações
    Watchdog::feed();

    if (safeModeActive) {
        // No Modo Seguro, desliga as saídas e pisca os LEDs de erro de forma síncrona
        digitalWrite(PIN_RELAY_V1, RELAY_OFF);
        digitalWrite(PIN_RELAY_V2, RELAY_OFF);
        digitalWrite(PIN_RELAY_V3, RELAY_OFF);
        digitalWrite(PIN_RELAY_LIGHT, RELAY_OFF);

        unsigned long now = millis();
        bool errorBlink = (now / 200) % 2 == 0; // Pisca rápido a cada 200ms

        // Pisca os LEDs físicos para indicar erro
        digitalWrite(PIN_LED_OFF, errorBlink ? LED_ON : LED_OFF_STATE);
        if (PIN_LED_V1 != -1) digitalWrite(PIN_LED_V1, errorBlink ? LED_ON : LED_OFF_STATE);
        if (PIN_LED_V2 != -1) digitalWrite(PIN_LED_V2, errorBlink ? LED_ON : LED_OFF_STATE);
        if (PIN_LED_V3 != -1) digitalWrite(PIN_LED_V3, errorBlink ? LED_ON : LED_OFF_STATE);
        if (PIN_LED_LIGHT != -1) digitalWrite(PIN_LED_LIGHT, errorBlink ? LED_ON : LED_OFF_STATE);
        return;
    }

    // 1. Atualiza leituras físicas
    Buttons::update();
    Hob2Hood::update();
    
    // Se a programação do timer estiver ativa, verifica estouro de tempo (5 segundos)
    if (timerProgrammingActive) {
        if (millis() - timerProgrammingStartTime >= 5000) {
            timerProgrammingActive = false;
            // Se estourou sem selecionar nada, inicia o timer padrão de 5 minutos
            TimerModule::startManualTimer(5);
            Logger::info(F("Timer: Tempo de programacao esgotado. Iniciando timer padrao de 5 min."));
        }
    }

    // 2. Processa eventos de entrada (Botões físicos têm prioridade sobre o Hob2Hood)
    handleButtonEvents();
    handleHob2HoodEvents();

    // 3. Atualiza máquinas de estados temporais
    Motor::update();
    Light::update();
    TimerModule::update();

    // 4. Atualiza LEDs de indicação
    updateModeLED();

    // 5. Executa checagem de integridade lógica
    runSafetyCheck();
}
