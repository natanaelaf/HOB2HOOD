# API Reference

Complete API reference for all IE60P modules.

## Table of Contents

- [Enums](#enums)
- [StateMachine](#statemachine)
- [Buttons](#buttons)
- [Motor](#motor)
- [Light](#light)
- [Hob2Hood](#hob2hood)
- [IR](#ir)
- [Timer](#timer)
- [Logger](#logger)
- [Settings](#settings)
- [EEPROMModule](#eeprommodule)
- [Watchdog](#watchdog)

---

## Enums

### ButtonId

```cpp
enum ButtonId {
    BTN_OFF = 0,      // OFF button
    BTN_V1 = 1,       // Speed 1 button
    BTN_V2 = 2,       // Speed 2 button
    BTN_V3 = 3,       // Speed 3 button
    BTN_LIGHT = 4,    // Light button
    BTN_COUNT = 5     // Total button count
};
```

### MotorSpeed

```cpp
enum MotorSpeed {
    SPEED_OFF = 0,   // Motor off
    SPEED_V1 = 1,    // Speed level 1 (low)
    SPEED_V2 = 2,    // Speed level 2 (medium)
    SPEED_V3 = 3     // Speed level 3 (high)
};
```

### MotorState

```cpp
enum MotorState {
    MOTOR_STATE_IDLE,       // No transition active
    MOTOR_STATE_INTERLOCK   // Waiting for relay settle (500 ms)
};
```

### LightBlinkMode

```cpp
enum LightBlinkMode {
    BLINK_NONE,      // No blinking (static)
    BLINK_NORMAL,    // Slow blink (~500 ms) - timer > 5 min
    BLINK_WARNING,   // Fast blink (~300 ms) - timer ≤ 5 min
    BLINK_CRITICAL   // Very fast (~100 ms) - timer ≤ 1 min
};
```

### Hob2HoodCmd

```cpp
enum Hob2HoodCmd {
    H2H_CMD_NONE = 0,   // No command
    H2H_CMD_ON,         // Turn on
    H2H_CMD_OFF,        // Turn off
    H2H_CMD_V0,         // Speed 0
    H2H_CMD_V1,         // Speed 1
    H2H_CMD_V2,         // Speed 2
    H2H_CMD_V3,         // Speed 3
    H2H_CMD_V4          // Speed 4
};
```

### LogLevel

```cpp
enum LogLevel {
    LOG_LEVEL_DEBUG,   // Debug messages
    LOG_LEVEL_INFO,    // Informational
    LOG_LEVEL_WARN,    // Warnings
    LOG_LEVEL_ERROR    // Errors
};
```

---

## StateMachine

Central state coordinator.

### Methods

```cpp
// Initialization
static void begin();

// Main update loop (call every ~10 ms)
static void update();

// Query state
static bool isSafeMode();
static bool isTimerProgramming();
```

### Example Usage

```cpp
void setup() {
    StateMachine::begin();
}

void loop() {
    StateMachine::update();
}
```

---

## Buttons

Physical button input handler.

### Methods

```cpp
// Initialization (called by StateMachine::begin)
static void begin();

// Update debounce state (call every loop iteration)
static void update();

// Check and consume button events
static bool consumeOffShort();
static bool consumeOffLong();
static bool consumeV1();
static bool consumeV2();
static bool consumeV3();
static bool consumeLightShort();
static bool consumeLightLong();

// Clear all pending events
static void clearEvents();
```

### Example Usage

```cpp
void handleButtons() {
    if (Buttons::consumeOffShort()) {
        Logger::info(F("OFF button pressed"));
        // Handle OFF
    }
    
    if (Buttons::consumeV1()) {
        Logger::info(F("V1 button pressed"));
        Motor::setSpeed(SPEED_V1);
    }
}
```

### Notes

- Speed buttons only report short-press (no long-press)
- OFF and Light buttons support both short and long-press
- Events consumed on return (single read only)

---

## Motor

Motor relay control.

### Methods

```cpp
// Initialization (called by StateMachine::begin)
static void begin();

// Motor control
static void setSpeed(MotorSpeed speed);
static MotorSpeed getSpeed();

// Update transition state machine (call every loop)
static void update();

// Query
static bool isTransitioning();
```

### Example Usage

```cpp
// Set motor to speed 2
Motor::setSpeed(SPEED_V2);

// Check if transitioning
if (!Motor::isTransitioning()) {
    // Safe to change speed again
}

// Get current speed
MotorSpeed current = Motor::getSpeed();
if (current == SPEED_OFF) {
    Logger::info(F("Motor is off"));
}
```

### Relay Outputs

| Speed | GPIO Pin |
|-------|----------|
| V1 | 10 (LOW active) |
| V2 | 11 (LOW active) |
| V3 | 12 (LOW active) |

### Timing

- Transition delay: 500 ms (relay interlock)
- LED updates: Immediate

---

## Light

Light relay and LED control.

### Methods

```cpp
// Initialization (called by StateMachine::begin)
static void begin();

// Light control
static void setState(bool on);
static bool getState();

// Blinking control
static void setBlinkMode(LightBlinkMode mode);
static void setCustomBlinkPeriod(uint16_t periodMs);

// Update blink state (call every loop)
static void update();
```

### Example Usage

```cpp
// Turn light on
Light::setState(true);

// Turn light off
Light::setState(false);

// Set blinking for timer feedback
Light::setBlinkMode(BLINK_WARNING);

// Custom blink period (milliseconds)
Light::setCustomBlinkPeriod(200);
```

### Blink Periods

| Mode | Period | Use Case |
|------|--------|----------|
| BLINK_NONE | N/A | Static |
| BLINK_NORMAL | ~500 ms | Timer > 5 min |
| BLINK_WARNING | ~300 ms | Timer ≤ 5 min |
| BLINK_CRITICAL | ~100 ms | Timer ≤ 1 min |

### GPIO Pins

- Relay: Pin 13 (LOW active)
- LED: Pin A2 (HIGH active)

---

## Hob2Hood

IR command decoder.

### Methods

```cpp
// Initialization
static void begin();

// Process incoming IR frames (call every loop)
static void update();

// Get decoded command
static Hob2HoodCmd consumeCommand();
```

### Example Usage

```cpp
void handleIR() {
    Hob2HoodCmd cmd = Hob2Hood::consumeCommand();
    
    switch (cmd) {
        case H2H_CMD_ON:
            Logger::info(F("Hob2Hood ON"));
            break;
        case H2H_CMD_OFF:
            Logger::info(F("Hob2Hood OFF"));
            break;
        case H2H_CMD_V1:
            Logger::info(F("Hob2Hood Speed 1"));
            Motor::setSpeed(SPEED_V1);
            break;
        default:
            break;
    }
}
```

### Protocol Details

- Carrier: 38 kHz
- Tolerance: ±35% on all timing
- Deduplication: 500 ms window

---

## IR

Interrupt-driven IR receiver.

### Methods

```cpp
// Initialization (called by StateMachine::begin)
static void begin();

// Check for new frames (call every loop)
static void update();

// Frame query
static bool isFrameReady();
static void getFrame(uint16_t* destBuffer, uint8_t& count);
static void resetFrame();
```

### Example Usage

```cpp
if (IR::isFrameReady()) {
    uint16_t buffer[64];
    uint8_t count;
    
    IR::getFrame(buffer, count);
    Logger::info(F("Frame ready: %d elements"), count);
    
    // Process frame...
    
    IR::resetFrame();
}
```

### Frame Buffer

- Array size: 64 uint16_t elements
- Units: Microseconds
- First element: Header mark (~2104 µs)
- Last element: Stop bit / trailing space

---

## Timer

Countdown timer management.

### Methods

```cpp
// Initialization (called by StateMachine::begin)
static void begin();

// Update timers (call every loop)
static void update();

// Manual timer control
static void startManualTimer(uint16_t minutes);
static void stopManualTimer();
static bool isManualTimerActive();
static uint32_t getManualTimerRemainingSeconds();

// Post-exhaust timer control
static void startPostExhaustion(uint16_t minutes);
static void stopPostExhaustion();
static bool isPostExhaustionActive();
```

### Example Usage

```cpp
// Start 30-minute timer
TimerModule::startManualTimer(30);

// Check remaining time
uint32_t remaining = TimerModule::getManualTimerRemainingSeconds();
Logger::info(F("Timer: %d seconds remaining"), remaining);

// Stop timer
TimerModule::stopManualTimer();

// Post-exhaust
TimerModule::startPostExhaustion(5);  // 5 minutes
if (TimerModule::isPostExhaustionActive()) {
    Logger::info(F("Post-exhaust active"));
}
```

### Defaults

- Manual timer: 30 minutes (from EEPROM)
- Post-exhaust: 5 minutes (from EEPROM)
- Max duration: 65535 minutes (~45 years)

---

## Logger

Serial logging system.

### Methods

```cpp
// Initialization (call once at startup)
static void begin(unsigned long baudRate = 115200);

// Logging (RAM format strings)
static void debug(const char* format, ...);
static void info(const char* format, ...);
static void warn(const char* format, ...);
static void error(const char* format, ...);

// Logging (Flash format strings, use F() macro)
static void debug(const __FlashStringHelper* format, ...);
static void info(const __FlashStringHelper* format, ...);
static void warn(const __FlashStringHelper* format, ...);
static void error(const __FlashStringHelper* format, ...);
```

### Example Usage

```cpp
// Log to INFO level
Logger::info(F("System started in %s mode"), manual ? "Manual" : "Auto");

// Log with multiple parameters
Logger::debug(F("Motor speed: %d, Timer: %d seconds"), 
              (int)Motor::getSpeed(), 
              (int)TimerModule::getManualTimerRemainingSeconds());

// Error logging
Logger::error(F("EEPROM validation failed"));
```

### Format Specifiers

| Specifier | Type |
|-----------|------|
| `%d` | int |
| `%u` | unsigned int |
| `%ld` | long |
| `%lld` | long long |
| `%s` | char* (string) |
| `%c` | char |
| `%x` | hexadecimal |
| `%%` | Literal % |

### Notes

- Use `F()` macro to store format strings in Flash (saves RAM)
- Output format: `[LEVEL] Message\r\n`
- Baud rate: Default 115200

---

## Settings

Configuration cache.

### Methods

```cpp
// Initialization (load from EEPROM)
static void begin();

// Mode management
static bool isManualMode();
static void setManualMode(bool manual);

// Light state
static bool getLightState();
static void setLightState(bool on);

// Timer durations
static uint16_t getPostExhaustionTime();
static void setPostExhaustionTime(uint16_t minutes);

static uint16_t getTimerTime();
static void setTimerTime(uint16_t minutes);
```

### Example Usage

```cpp
// Check mode
if (Settings::isManualMode()) {
    Logger::info(F("Manual mode"));
} else {
    Logger::info(F("Automatic mode"));
}

// Set defaults
Settings::setPostExhaustionTime(10);  // 10 minutes
Settings::setTimerTime(60);            // 60 minutes
```

### Persistence

All setters write immediately to EEPROM (~3.3 ms per value)

---

## EEPROMModule

Low-level persistent storage.

### Methods

```cpp
// Initialization
static void begin();

// Query initialization state
static bool isInitialized();

// Reset to defaults
static void initializeDefault();

// Mode (bool: true=Manual, false=Auto)
static bool readMode();
static void writeMode(bool manual);

// Light state (bool: true=ON, false=OFF)
static bool readLightState();
static void writeLightState(bool on);

// Post-exhaust duration (minutes)
static uint16_t readPostExhaustionTime();
static void writePostExhaustionTime(uint16_t minutes);

// Timer duration (minutes)
static uint16_t readTimerTime();
static void writeTimerTime(uint16_t minutes);
```

### Example Usage

```cpp
// Initialize if needed
if (!EEPROMModule::isInitialized()) {
    EEPROMModule::initializeDefault();
}

// Read/write settings
EEPROMModule::writeMode(true);
bool mode = EEPROMModule::readMode();
```

### Memory Addresses

```
0x00: Magic Byte (0x5A)
0x01: Mode (bool)
0x02: Light State (bool)
0x03: Post-Exhaust Low Byte
0x04: Post-Exhaust High Byte
0x05: Timer Low Byte
0x06: Timer High Byte
```

### Timing

- Read: ~3 µs per byte
- Write: ~3.3 ms per byte

---

## Watchdog

Hardware watchdog protection.

### Methods

```cpp
// Enable watchdog (~4 second timeout)
static void begin();

// Reset watchdog counter (prevent reset)
static void feed();
```

### Example Usage

```cpp
// Called once at startup
Watchdog::begin();

// Called every loop iteration
Watchdog::feed();
```

### Timeout Behavior

If `feed()` not called for ~4 seconds:
1. Hardware triggers system reset
2. Bootloader runs
3. `setup()` re-executes
4. All modules reinitialize

**Note:** Watchdog automatically disabled at startup to prevent boot loops

---

## Summary Quick Reference

| Class | Key Methods | Call Frequency |
|-------|------------|-----------------|
| StateMachine | `begin()`, `update()` | `update()` every ~10 ms |
| Buttons | `update()`, `consumeXxx()` | `update()` every loop |
| Motor | `begin()`, `setSpeed()`, `update()` | `update()` every loop |
| Light | `begin()`, `setState()`, `update()` | `update()` every loop |
| Hob2Hood | `begin()`, `update()`, `consumeCommand()` | `update()` every loop |
| IR | `begin()`, `update()`, `isFrameReady()` | `update()` every loop |
| Timer | `begin()`, `update()`, `startXxx()` | `update()` every loop |
| Logger | `begin()`, `info/debug/warn/error()` | As needed |
| Settings | `begin()`, `getXxx()`, `setXxx()` | On config change |
| EEPROMModule | `begin()`, `readXxx()`, `writeXxx()` | Via Settings wrapper |
| Watchdog | `begin()`, `feed()` | `feed()` every loop |

