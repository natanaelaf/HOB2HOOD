#ifndef IR_H
#define IR_H

#include <Arduino.h>

class IR {
public:
    static void begin();
    static void update();
    
    static bool isFrameReady();
    static void getFrame(uint16_t* destBuffer, uint8_t& count);
    static void resetFrame();

private:
    static void handleInterrupt();
    
    static volatile uint16_t rawBuffer[64];
    static volatile uint8_t rawCount;
    static volatile uint32_t lastInterruptTime;
    static volatile bool frameReady;
};

#endif // IR_H
