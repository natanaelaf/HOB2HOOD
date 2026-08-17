#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// CONFIGURAÇÃO DE PINOS (ARDUINO UNO)
// ============================================================================

// Entradas
#define PIN_IR_RECV       2   // Sensor IR 38 kHz (Usa INT0 interrupção externa)
#define PIN_BTN_OFF       3   // Botão OFF (Pull-up interno, aciona em nível baixo)
#define PIN_BTN_V1        4   // Botão Velocidade 1 (Pull-up interno)
#define PIN_BTN_V2        5   // Botão Velocidade 2 (Pull-up interno)
#define PIN_BTN_V3        6   // Botão Velocidade 3 (Pull-up interno)
#define PIN_BTN_LIGHT     7   // Botão Lâmpada (Pull-up interno)

// Saídas - Relés (Nível lógico depende do hardware, assumimos HIGH = LIGADO por padrão)
#define PIN_RELAY_V1      10  // Relé Velocidade 1
#define PIN_RELAY_V2      11  // Relé Velocidade 2
#define PIN_RELAY_V3      12  // Relé Velocidade 3
#define PIN_RELAY_LIGHT   13  // Relé Lâmpada

#define RELAY_ON          LOW
#define RELAY_OFF         HIGH

// Saídas - LEDs (HIGH = LIGADO)
#define PIN_LED_OFF       8   // LED do botão OFF / Indicador de Modo
#define PIN_LED_V1        9   // LED Velocidade 1
#define PIN_LED_V2        A0  // LED Velocidade 2 (Porta analógica usada como digital)
#define PIN_LED_V3        A1  // LED Velocidade 3 (Porta analógica usada como digital)
#define PIN_LED_LIGHT     A2  // LED Lâmpada (Porta analógica usada como digital)

#define LED_ON            HIGH
#define LED_OFF_STATE     LOW

// ============================================================================
// TEMPOS E DEBOUNCE
// ============================================================================
#define DEBOUNCE_MS       30    // Tempo de debounce para os botões físicos
#define LONG_PRESS_MS     3000  // Tempo para pressionamento longo (3 segundos)

// ============================================================================
// TEMPOS DE IR (HOB2HOOD / JVC-LIKE FAST)
// ============================================================================
#define IR_UNIT_US         526   // Unidade de tempo base do protocolo (526 us)
#define IR_HEADER_MARK     2104  // Pulso do cabeçalho (4 * UNIT = 2104 us)
#define IR_HEADER_SPACE    1052  // Espaço do cabeçalho (2 * UNIT = 1052 us)
#define IR_BIT_MARK        526   // Pulso de dados (1 * UNIT = 526 us)
#define IR_ONE_SPACE       1578  // Espaço para nível lógico 1 (3 * UNIT = 1578 us)
#define IR_ZERO_SPACE      526   // Espaço para nível lógico 0 (1 * UNIT = 526 us)

// Tolerância para decodificação (±35%)
#define IR_TOLERANCE_MIN(val)  ((uint16_t)(((uint32_t)(val)) * 65UL / 100UL))
#define IR_TOLERANCE_MAX(val)  ((uint16_t)(((uint32_t)(val)) * 135UL / 100UL))

// Janela de redundância de comando Hob2Hood (receber o mesmo código 2x nesta janela)
#define H2H_DUPLICATE_WINDOW_MS 500

// ============================================================================
// CONFIGURAÇÕES DA EEPROM
// ============================================================================
#define EEPROM_MAGIC_BYTE     0x5A  // Usado para validar se a EEPROM já foi gravada
#define EEPROM_ADDR_MAGIC     0     // Endereço do Magic Byte
#define EEPROM_ADDR_MODE      1     // Endereço do Modo de Operação (0 = Auto, 1 = Manual)
#define EEPROM_ADDR_LIGHT     2     // Endereço do Estado da Lâmpada (0 = OFF, 1 = ON)
#define EEPROM_ADDR_POST_EXH  3     // Endereço do Tempo de Pós-exaustão (uint16_t, 2 bytes)
#define EEPROM_ADDR_TIMER     5     // Endereço do Tempo Padrão do Timer (uint16_t, 2 bytes)

// ============================================================================
// VALORES PADRÃO (FALLBACKS)
// ============================================================================
#define DEFAULT_POST_EXH_MIN  5     // Tempo padrão de pós-exaustão em minutos
#define DEFAULT_TIMER_MIN     30    // Tempo padrão do timer manual em minutos

#endif // CONFIG_H
