#ifndef HOB2HOOD_H
#define HOB2HOOD_H

#include <Arduino.h>

enum Hob2HoodCmd {
    H2H_CMD_NONE = 0,
    H2H_CMD_ON,
    H2H_CMD_OFF,
    H2H_CMD_V0,
    H2H_CMD_V1,
    H2H_CMD_V2,
    H2H_CMD_V3,
    H2H_CMD_V4
};

class Hob2Hood {
public:
    static void begin();
    static void update();
    static Hob2HoodCmd consumeCommand();

private:
    static bool decodeFrame(uint16_t* buffer, uint8_t count, uint32_t& cmdVal);
    static uint32_t decodeHash(uint16_t* buffer, uint8_t count);
    static Hob2HoodCmd mapValueToCommand(uint32_t val);
    
    static Hob2HoodCmd lastCommand;
    static unsigned long lastCommandTime;
    
    static Hob2HoodCmd previousRawCommand;
    static unsigned long previousRawCommandTime;
    static Hob2HoodCmd lastValidatedCommand;
};

#endif // HOB2HOOD_H
