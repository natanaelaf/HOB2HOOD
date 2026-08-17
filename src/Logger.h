#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

class Logger {
public:
    static void begin(unsigned long baudRate = 115200);
    
    // RAM formatting support
    static void debug(const char* format, ...);
    static void info(const char* format, ...);
    static void warn(const char* format, ...);
    static void error(const char* format, ...);

    // Flash formatting support (using F() macro)
    static void debug(const __FlashStringHelper* format, ...);
    static void info(const __FlashStringHelper* format, ...);
    static void warn(const __FlashStringHelper* format, ...);
    static void error(const __FlashStringHelper* format, ...);

private:
    static void log(LogLevel level, const char* prefix, const char* format, va_list args);
    static void log_P(LogLevel level, const char* prefix, const __FlashStringHelper* format, va_list args);
};

#endif // LOGGER_H
