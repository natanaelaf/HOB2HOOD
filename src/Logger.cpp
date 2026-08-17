#include "Logger.h"
#include <stdarg.h>

void Logger::begin(unsigned long baudRate) {
    Serial.begin(baudRate);
    // Aguarda conexão da porta serial (principalmente útil no Leonardo, mas bom para estabilizar)
    delay(100); 
    info(F("Sistema de Logging Inicializado."));
}

void Logger::log(LogLevel level, const char* prefix, const char* format, va_list args) {
    char buffer[96];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Imprime timestamp em milissegundos
    Serial.print(F("["));
    Serial.print(millis());
    Serial.print(F("] ["));
    Serial.print(prefix);
    Serial.print(F("] "));
    Serial.println(buffer);
}

void Logger::log_P(LogLevel level, const char* prefix, const __FlashStringHelper* format, va_list args) {
    char buffer[96];
    vsnprintf_P(buffer, sizeof(buffer), (PGM_P)format, args);
    
    // Imprime timestamp em milissegundos
    Serial.print(F("["));
    Serial.print(millis());
    Serial.print(F("] ["));
    Serial.print(prefix);
    Serial.print(F("] "));
    Serial.println(buffer);
}

void Logger::debug(const char* format, ...) {
#if 1
    va_list args;
    va_start(args, format);
    log(LOG_LEVEL_DEBUG, "DEBUG", format, args);
    va_end(args);
#endif
}

void Logger::debug(const __FlashStringHelper* format, ...) {
#if 1
    va_list args;
    va_start(args, format);
    log_P(LOG_LEVEL_DEBUG, "DEBUG", format, args);
    va_end(args);
#endif
}

void Logger::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_LEVEL_INFO, "INFO", format, args);
    va_end(args);
}

void Logger::info(const __FlashStringHelper* format, ...) {
    va_list args;
    va_start(args, format);
    log_P(LOG_LEVEL_INFO, "INFO", format, args);
    va_end(args);
}

void Logger::warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_LEVEL_WARN, "WARN", format, args);
    va_end(args);
}

void Logger::warn(const __FlashStringHelper* format, ...) {
    va_list args;
    va_start(args, format);
    log_P(LOG_LEVEL_WARN, "WARN", format, args);
    va_end(args);
}

void Logger::error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_LEVEL_ERROR, "ERROR", format, args);
    va_end(args);
}

void Logger::error(const __FlashStringHelper* format, ...) {
    va_list args;
    va_start(args, format);
    log_P(LOG_LEVEL_ERROR, "ERROR", format, args);
    va_end(args);
}
