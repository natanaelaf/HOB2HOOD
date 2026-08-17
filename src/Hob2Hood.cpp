#include "Hob2Hood.h"
#include "IR.h"
#include "config.h"
#include "Logger.h"
#include "Light.h"

// Inicialização das variáveis estáticas
Hob2HoodCmd Hob2Hood::lastCommand = H2H_CMD_NONE;
unsigned long Hob2Hood::lastCommandTime = 0;
Hob2HoodCmd Hob2Hood::previousRawCommand = H2H_CMD_NONE;
unsigned long Hob2Hood::previousRawCommandTime = 0;
Hob2HoodCmd Hob2Hood::lastValidatedCommand = H2H_CMD_NONE;

void Hob2Hood::begin() {
    IR::begin();
    lastCommand = H2H_CMD_NONE;
    lastCommandTime = 0;
    previousRawCommand = H2H_CMD_NONE;
    previousRawCommandTime = 0;
    lastValidatedCommand = H2H_CMD_NONE;
    Logger::info(F("Modulo Hob2Hood Inicializado."));
}

bool Hob2Hood::decodeFrame(uint16_t* buffer, uint8_t count, uint32_t& cmdVal) {
    // 1. Se tiver pelo menos 51 transições, tenta decodificar como protocolo 24 bits (Elektor)
    if (count >= 51) {
        uint16_t headerMark = buffer[1];
        uint16_t headerSpace = buffer[2];

        // Validação do Header
        if (headerMark >= IR_TOLERANCE_MIN(IR_HEADER_MARK) && headerMark <= IR_TOLERANCE_MAX(IR_HEADER_MARK) &&
            headerSpace >= IR_TOLERANCE_MIN(IR_HEADER_SPACE) && headerSpace <= IR_TOLERANCE_MAX(IR_HEADER_SPACE)) {

            uint32_t decodedValue = 0;
            bool ok = true;

            // Decodifica os 24 bits
            for (uint8_t i = 0; i < 24; i++) {
                uint16_t mark = buffer[3 + 2 * i];
                uint16_t space = buffer[4 + 2 * i];

                // Validação da duração do Mark
                if (mark < IR_TOLERANCE_MIN(IR_BIT_MARK) || mark > IR_TOLERANCE_MAX(IR_BIT_MARK)) {
                    ok = false;
                    break;
                }

                // Validação da duração do Space (Decodificação lógica)
                bool isZero = (space >= IR_TOLERANCE_MIN(IR_ZERO_SPACE) && space <= IR_TOLERANCE_MAX(IR_ZERO_SPACE));
                bool isOne = (space >= IR_TOLERANCE_MIN(IR_ONE_SPACE) && space <= IR_TOLERANCE_MAX(IR_ONE_SPACE));

                decodedValue <<= 1;
                if (isOne) {
                    decodedValue |= 1;
                } else if (!isZero) {
                    ok = false;
                    break; // Timing do espaço inválido
                }
            }

            if (ok) {
                // Validação do CRC implícito (repetição/relacionamento dos três bytes)
                uint8_t b1 = (decodedValue >> 16) & 0xFF;
                uint8_t b2 = (decodedValue >> 8) & 0xFF;
                uint8_t b3 = decodedValue & 0xFF;

                if (b2 == (uint8_t)(b1 - 1) && b3 == (uint8_t)(b2 - 1)) {
                    cmdVal = decodedValue;
                    return true;
                }
            }
        }
    }

    // 2. Fallback: Se for sinal curto ou falhar na validação do protocolo 24 bits,
    // calcula o hash FNV-1a (usado pelo IRremote no JameZUK / microcontrolador.net)
    if (count >= 8) {
        cmdVal = decodeHash(buffer, count);
        return true;
    }

    return false;
}

uint32_t Hob2Hood::decodeHash(uint16_t* buffer, uint8_t count) {
    uint32_t hash = 2166136261UL; // FNV_BASIS_32
    for (uint8_t i = 1; i + 2 < count; i++) {
        int a = buffer[i];
        int b = buffer[i+2];
        int val;
        if (b < a * 0.8) {
            val = 0; // shorter
        } else if (a < b * 0.8) {
            val = 2; // longer
        } else {
            val = 1; // equal
        }
        hash = (hash * 16777619UL) ^ val; // FNV_PRIME_32
    }
    return hash;
}

Hob2HoodCmd Hob2Hood::mapValueToCommand(uint32_t val) {
    switch (val) {
        // Códigos do protocolo estruturado 24 bits (Elektor)
        case 0x2D2C2B: return H2H_CMD_ON;
        case 0x2A2928: return H2H_CMD_OFF;
        case 0x272625: return H2H_CMD_V0;
        case 0x939291: return H2H_CMD_V1;
        case 0x908F8E: return H2H_CMD_V2;
        case 0x1E1D1C: return H2H_CMD_V3;
        case 0x8D8C8B: return H2H_CMD_V4;

        // Códigos Hash IRremote do JameZUK / microcontrolador.net
        case 0x055303A3: return H2H_CMD_V0;   // Fan Off
        case 0xEA62BC7C: return H2H_CMD_V0;   // Fan Off (ou outro comando recebido do cooktop do usuário)
        case 0xE3C01BE2: return H2H_CMD_V1;   // Speed 1
        case 0xD051C301: return H2H_CMD_V2;   // Speed 2
        case 0xC22FFFD7: return H2H_CMD_V3;   // Speed 3
        case 0xB9121B29: return H2H_CMD_V4;   // Speed 4
        case 0xE208293C: return H2H_CMD_ON;   // Light On / Cooktop On
        case 0x24ACF947: return H2H_CMD_OFF;  // Light Off / Cooktop Off
        
        default: return H2H_CMD_NONE;
    }
}

void Hob2Hood::update() {
    IR::update();

    if (IR::isFrameReady()) {
        uint16_t buffer[64];
        uint8_t count = 0;
        IR::getFrame(buffer, count);

        // Imprime informações básicas e o dump dos timings
        Logger::info(F("IR: Frame capturado. Pulsos = %d"), (int)count);
        
        Serial.print(F("[IR RAW TIMINGS] "));
        for (uint8_t i = 0; i < count; i++) {
            Serial.print(buffer[i]);
            if (i < count - 1) {
                Serial.print(F(", "));
            }
        }
        Serial.println();

        uint32_t val = 0;
        if (decodeFrame(buffer, count, val)) {
            Hob2HoodCmd cmd = mapValueToCommand(val);
            Logger::info(F("IR Decodificado com sucesso: 0x%08lX (CMD %d)"), (unsigned long)val, (int)cmd);
            if (cmd != H2H_CMD_NONE) {
                unsigned long now = millis();
                lastCommandTime = now; // Sempre atualiza o timestamp de recepção ativa do canal
                
                // Aceita o comando se for diferente do último processado
                if (cmd != lastValidatedCommand) {
                    lastCommand = cmd;
                    lastValidatedCommand = cmd;
                    Logger::info(F("Hob2Hood: Comando recebido e validado: 0x%08lX (CMD %d)"), (unsigned long)val, (int)cmd);
                } else {
                    Logger::info(F("Hob2Hood: Comando duplicado ignorado (ja esta ativo)."));
                }
            }
        } else {
            Logger::warn(F("IR Falha: Nao foi possivel decodificar o frame (timings invalidos ou muito curto)."));
        }
        
        IR::resetFrame(); // Prepara para a próxima captura
    }
}

Hob2HoodCmd Hob2Hood::consumeCommand() {
    Hob2HoodCmd val = lastCommand;
    lastCommand = H2H_CMD_NONE;
    return val;
}
