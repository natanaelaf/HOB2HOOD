#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

enum MotorSpeed {
    SPEED_OFF = 0,
    SPEED_V1 = 1,
    SPEED_V2 = 2,
    SPEED_V3 = 3
};

enum MotorState {
    MOTOR_STATE_IDLE,
    MOTOR_STATE_INTERLOCK
};

class Motor {
public:
    static void begin();
    static void setSpeed(MotorSpeed speed);
    static MotorSpeed getSpeed();
    static void update();
    static bool isTransitioning();

private:
    static void writeRelays(MotorSpeed speed);
    static void updateLEDs(MotorSpeed speed);

    static MotorSpeed currentSpeed;
    static MotorSpeed targetSpeed;
    static MotorState state;
    static unsigned long transitionStartTime;
};

#endif // MOTOR_H
