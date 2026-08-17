#include <Arduino.h>
#include "StateMachine.h"

void setup() {
    // Inicializa a máquina de estados central que coordena todos os submódulos
    StateMachine::begin();
}

void loop() {
    // Executa a iteração da máquina de estados (botoes, IR, timers, seguranca, watchdog)
    StateMachine::update();
}
