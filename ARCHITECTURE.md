# System Architecture

## Table of Contents
1. [High-Level Architecture](#high-level-architecture)
2. [Module Overview](#module-overview)
3. [Control Flow](#control-flow)
4. [State Management](#state-management)
5. [Communication Protocols](#communication-protocols)
6. [Memory Layout](#memory-layout)

---

## High-Level Architecture

The IE60P controller is built on a **modular, event-driven architecture** with a central state machine that orchestrates all subsystems:

```
┌────────────────────────────────────────────────┐
│         Arduino UNO (ATmega328P)               │
├────────────────────────────────────────────────┤
│                                                │
│  ┌──────────────────────────────────────────┐  │
│  │    StateMachine (Main Coordinator)       │  │
│  │  • Mode management (Auto/Manual)         │  │
│  │  • Event routing                         │  │
│  │  • LED feedback                          │  │
│  │  • Safety checks                         │  │
│  └────────────┬─────────────────────────────┘  │
│               │                                │
│  ┌────────────┴──────────────────────────────┐ │
│  │                                           │ │
│  │  Input Modules      Output Modules        │ │
│  │  ────────────       ─────────────         │ │
│  │  • Buttons          • Motor (Relays)      │ │
│  │  • IR Receiver      • Light (LEDs)        │ │
│  │  • Settings         • Logger              │ │
│  │                                           │ │
│  │  Service Modules                          │ │
│  │  ────────────                             │ │
│  │  • Timer            • Watchdog            │ │
│  │  • EEPROM           • Settings Cache      │ │
│  │                                           │ │
│  └───────────────────────────────────────────┘ │
│                                                │
│  ┌───────────────────────────────────────────┐ │
│  │  Hardware Peripherals                    │ │
│  │  ─────────────────────                   │ │
│  │  GPIO pins, Timers, UART                 │ │
│  └───────────────────────────────────────────┘ │
│                                                │
└────────────────────────────────────────────────┘
```

### Design Principles

1. **Modularity**: Each function (buttons, motor, IR, etc.) is encapsulated in its own class
2. **Event-Driven**: Modules generate events that the StateMachine consumes
3. **Stateless Polling**: No interrupts except IR receiver; main loop polls all modules
4. **Safety-First**: Watchdog protection, debouncing, and mode safeguards
5. **Persistent State**: EEPROM stores configuration; RAM stores runtime state
6. **Transparent Logging**: All state changes are logged to Serial

---

## Module Overview

### Core Modules

#### 1. **StateMachine** (Central Coordinator)
- **Responsibility**: Route events, manage modes, coordinate subsystems
- **Input**: Button events, IR commands, timers
- **Output**: Motor speed changes, LED updates, mode toggles
- **Key Logic**:
  - Ignores Hob2Hood commands in Manual mode
  - Blocks Hob2Hood when manual timer is active
  - Manages mode-switch LED animation

#### 2. **Buttons** (Input Handler)
- **Responsibility**: Debounce physical buttons, generate events
- **Inputs**: 5 GPIO pins (OFF, V1, V2, V3, Light)
- **Outputs**: 7 event flags (short/long press for OFF and Light, short for speeds)
- **Implementation**: Debounced state machine per button (30ms debounce)

#### 3. **Motor** (Relay Control)
- **Responsibility**: Control fan speed via relay outputs
- **Inputs**: Speed commands (OFF, V1, V2, V3)
- **Outputs**: 3 GPIO pins for relays (V1, V2, V3)
- **Interlock**: Ensures only one speed relay active at a time
- **Protection**: Soft delay between transitions to prevent relay chatter

#### 4. **Light** (LED Control)
- **Responsibility**: Control light relay and visual feedback
- **Features**: Basic on/off, blinking patterns for timer feedback
- **Blink Modes**:
  - BLINK_NONE: Off
  - BLINK_NORMAL: Timer active (>5 min) - slow blink
  - BLINK_WARNING: Last 5 minutes - faster blink
  - BLINK_CRITICAL: Last minute - very fast blink

#### 5. **Hob2Hood** (IR Command Decoder)
- **Responsibility**: Decode IR protocol and extract commands
- **Protocol**: JVC-like 38 kHz carrier, 526 µs time units
- **Commands**: ON, OFF, V0-V4 speed commands
- **Safety**: Hash-based validation, command deduplication window

#### 6. **IR** (Interrupt-Driven IR Receiver)
- **Responsibility**: Capture IR pulse timing via INT0 (Pin 2)
- **Method**: Interrupt-driven capture to raw timing buffer
- **Output**: 64-element timing array for frame decoding
- **Synchronization**: Marks frame as ready after silence timeout

#### 7. **Timer** (Duration Management)
- **Responsibility**: Track manual timer and post-exhaust durations
- **Features**:
  - Manual timer with custom duration
  - Post-exhaust timer (low-speed auto-shutdown)
  - Non-blocking countdown (checked each loop iteration)

#### 8. **Settings** (Configuration Cache)
- **Responsibility**: Runtime cache of persistent settings
- **Persistent**: Mode (Auto/Manual), Light state, Timer durations
- **Backend**: Backed by EEPROM module
- **Sync**: Lazy write to EEPROM on change

#### 9. **EEPROMModule** (Persistent Storage)
- **Responsibility**: Read/write EEPROM with magic byte validation
- **Addresses**:
  - `0x00`: Magic byte (0x5A)
  - `0x01`: Mode (0=Auto, 1=Manual)
  - `0x02`: Light state (0=OFF, 1=ON)
  - `0x03-0x04`: Post-exhaust time (uint16_t)
  - `0x05-0x06`: Timer duration (uint16_t)

#### 10. **Logger** (Serial Logging)
- **Responsibility**: Printf-style debug logging
- **Levels**: DEBUG, INFO, WARN, ERROR
- **Capability**: Format strings in RAM or Flash (PROGMEM)
- **Output**: Serial UART at 115200 baud

#### 11. **Watchdog** (Safety Protection)
- **Responsibility**: Detect and recover from firmware lockups
- **Method**: Hardware watchdog timer (ATmega328P built-in)
- **Timeout**: ~4 seconds (system resets if not "fed")
- **Feed Point**: Called every StateMachine::update() cycle

---

## Control Flow

### Initialization Sequence

```
main.cpp::setup()
    │
    └─> StateMachine::begin()
        ├─> Logger::begin()           [Serial UART @ 115200]
        ├─> Settings::begin()         [Load from EEPROM]
        ├─> Buttons::begin()          [Setup GPIO inputs with pullup]
        ├─> Motor::begin()            [Setup GPIO outputs for relays]
        ├─> Light::begin()            [Setup GPIO output for light relay]
        ├─> Hob2Hood::begin()         [Setup IR frame buffer]
        ├─> IR::begin()               [Attach INT0 interrupt for IR pin]
        ├─> TimerModule::begin()      [Initialize timers]
        ├─> Watchdog::begin()         [Enable hardware watchdog]
        └─> Log: "System Initialized"
```

### Main Loop (Polling Cycle)

```
main.cpp::loop()
    │
    └─> StateMachine::update()
        ├─> IR::update()                     [Check for new frame]
        ├─> Hob2Hood::update()               [Decode if frame ready]
        ├─> Buttons::update()                [Debounce & detect presses]
        ├─> handleHob2HoodEvents()           [Process IR commands]
        ├─> handleButtonEvents()             [Process button presses]
        ├─> Motor::update()                  [Handle transition timing]
        ├─> Light::update()                  [Update blinking state]
        ├─> TimerModule::update()            [Decrement active timers]
        ├─> updateModeLED()                  [Animate mode-switch LED]
        ├─> runSafetyCheck()                 [Validation checks]
        ├─> Watchdog::feed()                 [Reset watchdog counter]
        └─> [Repeat at ~100 Hz on 16 MHz clock]
```

### Event Processing Example: OFF Button Press

```
1. User presses OFF button
   ├─> Pin 3 goes LOW (internal pullup engaged)
   
2. Buttons::update() (next loop)
   ├─> Raw pin read = LOW
   ├─> Wait 30 ms for debounce
   ├─> Confirm stable LOW
   ├─> Detect state change LOW (new)
   ├─> Start long-press timer
   
3. If button released <3s:
   ├─> Duration < LONG_PRESS_MS
   ├─> Set eventOffShort = true
   ├─> Log: "OFF short press"
   
4. StateMachine::handleButtonEvents()
   ├─> Call consumeOffShort()
   ├─> Clears eventOffShort and returns true
   ├─> Execute: Motor::setSpeed(SPEED_OFF)
   ├─> Execute: TimerModule::stopManualTimer()
   ├─> Log: "Button OFF: Stopping motor and timers"
```

---

## State Management

### Modes

The system operates in two distinct modes, stored in RAM and persisted to EEPROM:

| Aspect | Auto Mode | Manual Mode |
|--------|-----------|-------------|
| **Hob2Hood Enabled** | Yes | No (ignored) |
| **Button Control** | Full | Full |
| **Post-Exhaust** | Yes (on Hob2Hood OFF) | No (button OFF stops immediately) |
| **LED OFF State** | LOW (off) | HIGH (on) |
| **Mode Indicator** | Serial log | LED OFF state |
| **Switch Trigger** | Long-press OFF (3s) | Long-press OFF (3s) |

### Runtime State Flags

| Flag | Purpose | Scope |
|------|---------|-------|
| `StateMachine::isManual` | Current mode | RAM (persistent via Settings) |
| `StateMachine::safeModeActive` | Emergency safety state | RAM only |
| `Motor::currentSpeed` | Active motor speed | RAM |
| `Light::isLightOn` | Light relay state | RAM (persistent via Settings) |
| `TimerModule::manualTimerActive` | Manual timer running | RAM |
| `TimerModule::postExhaustionActive` | Post-exhaust running | RAM |

### Watchdog State

- **Enabled**: After all modules initialized
- **Timeout**: ~4 seconds (ATmega328P default)
- **Fed By**: `StateMachine::update()` → `Watchdog::feed()` each cycle
- **Reset**: If watchdog fires, system restarts from `setup()`

---

## Communication Protocols

### IR Protocol (Hob2Hood / JVC-like)

**Physical Layer:**
- Carrier: 38 kHz
- Receiver: Active-LOW on Pin 2 (INT0)
- Timing: 526 µs base unit
- Tolerance: ±35% on all timings

**Frame Structure:**
```
[HEADER_MARK] [HEADER_SPACE] [BIT_0] [BIT_1] ... [BIT_31] [STOP_BIT]
   2104 µs        1052 µs      pairs                        526 µs
```

**Bit Encoding:**
- **Logical 1**: 526 µs mark + 1578 µs space
- **Logical 0**: 526 µs mark + 526 µs space

**Commands:**
| Value | Command | Effect |
|-------|---------|--------|
| 0 | H2H_CMD_NONE | No operation |
| 1 | H2H_CMD_ON | Turn on (light + motor auto-detect speed) |
| 2 | H2H_CMD_OFF | Turn off (trigger post-exhaust) |
| 3-7 | H2H_CMD_V0 to V4 | Speed levels 0-4 |

### Serial Protocol (Logging)

- **Baud**: 115200 bps
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Format**: `[LEVEL] [Timestamp] Message\r\n`
- **Levels**: DEBUG, INFO, WARN, ERROR

Example:
```
[INFO] IE60P Smart Controller v1.0 - Inicializando...
[DEBUG] Pin 2 (IR): Interrupt configured
[INFO] Sistema Inicializado com Sucesso.
```

### GPIO Pin Mapping

**Inputs:**
| Pin | Function | Mode | Notes |
|-----|----------|------|-------|
| 2 | IR Receiver | INPUT | INT0 interrupt-driven |
| 3 | Button OFF | INPUT_PULLUP | Active LOW |
| 4 | Button V1 | INPUT_PULLUP | Active LOW |
| 5 | Button V2 | INPUT_PULLUP | Active LOW |
| 6 | Button V3 | INPUT_PULLUP | Active LOW |
| 7 | Button Light | INPUT_PULLUP | Active LOW |

**Outputs:**
| Pin | Function | Mode | Active |
|-----|----------|------|--------|
| 8 | LED OFF / Mode | OUTPUT | HIGH |
| 9 | LED V1 | OUTPUT | HIGH |
| 10 | Relay V1 | OUTPUT | LOW |
| 11 | Relay V2 | OUTPUT | LOW |
| 12 | Relay V3 | OUTPUT | LOW |
| 13 | Relay Light | OUTPUT | LOW |
| A0 | LED V2 | OUTPUT | HIGH |
| A1 | LED V3 | OUTPUT | HIGH |
| A2 | LED Light | OUTPUT | HIGH |

---

## Memory Layout

### EEPROM Map (Arduino UNO has 1024 bytes)

```
Offset  Size   Name              Type      Range        Default
─────────────────────────────────────────────────────────────────
0x00    1      Magic Byte        uint8_t   0x5A         0x5A
0x01    1      Mode              bool      0/1          0 (Auto)
0x02    1      Light State       bool      0/1          0 (OFF)
0x03    2      Post-Exhaust      uint16_t  0-65535 min  5 min
0x05    2      Timer Duration    uint16_t  0-65535 min  30 min
0x07    1017   Reserved/Free     -         -            -
─────────────────────────────────────────────────────────────────
```

### Flash Memory Usage (Arduino UNO has 32 KB)

```
Typical breakdown (approximate):
- Bootloader:     2 KB (0x7800-0x7FFF)
- Firmware Code:  ~28 KB (0x0000-0x7800)
  - Modules: 15 KB
  - Libraries: 8 KB
  - Strings (PROGMEM): 5 KB
```

### RAM Layout (Arduino UNO has 2 KB)

```
Typical breakdown:
- Stack:               ~256 bytes (grows downward)
- Global/Static:      ~800 bytes
  - Module states
  - Event flags
  - Timers
  - Buffers
- Heap:                ~200 bytes (grows upward, rarely used)
```

---

## Interrupt Handling

### INT0 (IR Receiver) - Only Interrupt Used

```c
ISR(INT0_vect)
{
    // Captures edge timing in microseconds
    IR::handleInterrupt();  // Raw timing -> buffer
}
```

**Rationale**: Tight timing requirements for IR pulse measurement (±3% accuracy)

**All Other Processing**: Polled in main loop (non-blocking, predictable)

---

## Timing Guarantees

### Loop Frequency
- **Target**: ~100 Hz (10 ms per cycle)
- **Actual**: ~8-12 ms depending on module activity
- **Jitter**: <1 ms (acceptable for debouncing/timing)

### Debounce Window
- **Duration**: 30 ms
- **Samples**: ~3-4 per cycle
- **Confidence**: 2+ consecutive stable reads required

### Long-Press Detection
- **Threshold**: 3000 ms
- **Resolution**: +/- 50 ms (acceptable)

### Motor Interlock Transition
- **Duration**: ~500 ms between speed changes
- **Purpose**: Allow relay coils to release/settle

---

## Error Handling & Safety

### Watchdog Recovery
```
If StateMachine::update() not called for ~4s:
1. Hardware watchdog timer expires
2. ATmega328P forces full system reset
3. Bootloader runs
4. setup() reinitializes all modules
5. Watchdog disabled during init, then re-enabled
```

### EEPROM Corruption Recovery
```
If Magic Byte (0x5A) not found:
1. EEPROMModule::initializeDefault() called
2. All default values written
3. System continues with defaults
```

### IR Frame Validation
```
If frame timeout or decode fails:
1. Frame marked invalid
2. Hob2Hood::consumeCommand() returns H2H_CMD_NONE
3. Command ignored (no state change)
4. No error raised (graceful degradation)
```

---

## Performance Considerations

### CPU Utilization
- Typically <30% average (8-12 ms loops @ 100 Hz)
- Peak: ~80% during IR frame decoding
- Watchdog prevents runaway loops

### Power Consumption
- Typical: ~200-300 mA (relay/LED active)
- Idle: ~50 mA (microcontroller + pull-ups)
- Standby: Not implemented (always monitoring)

### Timing Accuracy
- Clock: 16 MHz ATmega328P (±10% nominal)
- Debounce: ±50 ms accuracy sufficient
- IR Timing: ±3% required (firmware timing compensation)

