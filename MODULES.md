# Module Documentation

Comprehensive documentation of each module in the IE60P system.

## Table of Contents

1. [StateMachine](#statemachine)
2. [Buttons](#buttons)
3. [Motor](#motor)
4. [Light](#light)
5. [Hob2Hood](#hob2hood)
6. [IR](#ir)
7. [Timer](#timer)
8. [Logger](#logger)
9. [Settings](#settings)
10. [EEPROMModule](#eeprommodule)
11. [Watchdog](#watchdog)

---

## StateMachine

### Purpose
Central coordinator that orchestrates all subsystems, manages operational modes, and routes events.

### Public Methods

#### `void begin()`
Initializes the entire system.

**Behavior:**
- Disables watchdog (prevents boot loops)
- Initializes Logger at 115200 baud
- Reads Mode from EEPROM
- Initializes all modules in order
- Sets LED OFF to indicate current mode
- Enables watchdog protection
- Logs "System Initialized"

**Called By:** `setup()` once at boot

#### `void update()`
Main event processing loop. Should be called repeatedly (ideally every ~10 ms).

**Behavior:**
1. Calls `IR::update()` to check for new IR frames
2. Calls `Hob2Hood::update()` to decode frames
3. Calls `Buttons::update()` to debounce inputs
4. Processes IR commands via `handleHob2HoodEvents()`
5. Processes button events via `handleButtonEvents()`
6. Calls `Motor::update()` to handle transitions
7. Calls `Light::update()` to blink patterns
8. Calls `TimerModule::update()` to decrement timers
9. Updates mode LED with animation
10. Runs safety checks
11. Feeds watchdog

**Called By:** `loop()` continuously

#### `bool isSafeMode()`
Returns whether safe mode (emergency shutdown) is active.

**Returns:** `true` if safe mode is active, `false` otherwise

#### `bool isTimerProgramming()`
Returns whether the manual timer programming state is active.

**Behavior:** Called by buttons module to suppress normal button processing during timer setup

**Returns:** `true` if awaiting speed selection for timer, `false` otherwise

### Private Methods

#### `void toggleMode()`
Switches between Manual and Automatic modes.

**Behavior:**
- Flips `isManual` flag
- Writes new mode to Settings (persisted to EEPROM)
- Triggers 3-blink animation on LED OFF
- Logs mode change with new state

#### `void handleButtonEvents()`
Processes physical button presses.

**Logic:**
1. If timer programming active:
   - V1/V2/V3: Start 1/2/3 minute timer
   - Light: Start default timer (5 min)
   - OFF: Cancel programming
   - Consume all events and return
2. OFF long press (3s): Toggle mode
3. OFF short press: Stop motor and all timers
4. V1/V2/V3: Set motor speed (cancel post-exhaust)
5. Light long press (3s): Enter timer programming mode
6. Light short press: Toggle light state

#### `void handleHob2HoodEvents()`
Processes IR commands from Hob2Hood.

**Rules:**
- Ignored if in Manual mode
- Ignored if manual timer is active (button control has priority)
- H2H_CMD_ON: Stop post-exhaust, turn light on
- H2H_CMD_OFF: Turn light off, start post-exhaust (if motor running)
- Speed commands (V1-V4): Set motor to corresponding speed

**Logging:** All commands logged with action taken

#### `void updateModeLED()`
Handles LED OFF animation and static state.

**States:**
- **Blinking**: 3 complete blinks (6 toggles, 200 ms each)
- **Static**: 
  - Manual mode: LED ON (HIGH)
  - Auto mode: LED OFF (LOW)

#### `void runSafetyCheck()`
Validates system state and triggers safe mode if needed.

**Checks:**
- Motor is not transitioning indefinitely
- Timers not running excessively long
- No relay chattering detected

#### `void enterSafeMode()`
Initiates emergency shutdown.

**Behavior:**
- Sets `safeModeActive = true`
- Stops motor (SPEED_OFF)
- Stops all timers
- Turns off light
- Logs error condition
- Awaits manual button press or reset

### State Variables

```cpp
static bool isManual                    // Current mode
static bool safeModeActive              // Emergency shutdown flag
static bool modeBlinkActive             // 3-blink animation in progress
static unsigned long lastModeBlinkToggle // Last blink toggle time
static uint8_t modeBlinkToggles         // Blink counter (0-6 for 3 blinks)
static bool modeBlinkState              // Current blink LED state
static bool timerProgrammingActive      // Awaiting speed selection for timer
static unsigned long timerProgrammingStartTime
```

### Rules & Constraints

- **Button Priority**: Manual button presses always override Hob2Hood commands
- **Mode Isolation**: Automatic-specific features (post-exhaust) disabled in Manual mode
- **Timer Lock**: Once manual timer started, Hob2Hood commands blocked until timer expires or manually stopped
- **Safety Timeout**: If any operation exceeds 1 hour, safe mode triggered

---

## Buttons

### Purpose
Debounces physical button inputs and generates event flags consumed by StateMachine.

### Public Methods

#### `void begin()`
Initializes GPIO pins for all buttons with pull-up configuration.

**Setup:**
- Pins 3, 4, 5, 6, 7 configured as INPUT_PULLUP
- All debounce timers reset
- Event flags cleared

#### `void update()`
Debounces all buttons and detects press/release transitions. Call every loop iteration.

**Algorithm per button:**
1. Read current GPIO state
2. If changed: Start debounce timer
3. After 30 ms stable: Update debounced state
4. Detect press (HIGH→LOW) or release (LOW→HIGH)
5. Measure press duration
6. If ≥3000 ms: Set long-press flag
7. If <3000 ms: Set short-press flag

**Timing:** ~30 ms debounce, ±50 ms long-press accuracy

#### `bool consumeOffShort()`
Checks and clears OFF button short-press event.

**Returns:** `true` if short-press occurred since last call, `false` otherwise

#### `bool consumeOffLong()`
Checks and clears OFF button long-press event.

**Returns:** `true` if long-press occurred since last call, `false` otherwise

#### `bool consumeV1() / V2() / V3()`
Check and clear speed button events (short-press only).

**Returns:** `true` if button pressed since last call, `false` otherwise

#### `bool consumeLightShort()`
Checks and clears Light button short-press event.

**Returns:** `true` if short-press occurred since last call, `false` otherwise

#### `bool consumeLightLong()`
Checks and clears Light button long-press event.

**Returns:** `true` if long-press occurred since last call, `false` otherwise

#### `void clearEvents()`
Manually clears all pending event flags.

**Usage:** Called during mode transitions to prevent stale events

### State Variables

```cpp
static const uint8_t pinMap[BTN_COUNT]  // GPIO pin for each button
static bool lastPinState[BTN_COUNT]     // Previous raw GPIO state
static bool debouncedState[BTN_COUNT]   // Stable debounced state
static bool prevDebouncedState[BTN_COUNT] // Previous debounced state
static unsigned long lastDebounceTime[BTN_COUNT] // Debounce timer

static unsigned long pressStartTime[BTN_COUNT] // When press started
static bool longPressTriggered[BTN_COUNT] // Long-press already fired?

// Event flags (consumed by StateMachine)
static bool eventOffShort, eventOffLong
static bool eventV1, eventV2, eventV3
static bool eventLightShort, eventLightLong
```

### Button Mapping

| Name | Pin | Function | Event(s) |
|------|-----|----------|----------|
| BTN_OFF | 3 | Power/Mode toggle | Short, Long |
| BTN_V1 | 4 | Speed Level 1 | Short only |
| BTN_V2 | 5 | Speed Level 2 | Short only |
| BTN_V3 | 6 | Speed Level 3 | Short only |
| BTN_LIGHT | 7 | Light toggle / Timer programming | Short, Long |

### Timing

- **Debounce**: 30 ms delay for stable reads
- **Long Press Threshold**: 3000 ms (3 seconds)
- **Accuracy**: ±50 ms

---

## Motor

### Purpose
Controls the range hood fan motor via 3 relay outputs, ensuring safe speed transitions.

### Public Methods

#### `void begin()`
Initializes relay control pins (10, 11, 12).

**Setup:**
- Pins 10, 11, 12 configured as OUTPUT
- All relays set to OFF (HIGH, per config)
- Current speed: SPEED_OFF
- State: MOTOR_STATE_IDLE

#### `void setSpeed(MotorSpeed speed)`
Sets target motor speed. Transitions happen in `update()`.

**Valid Speeds:**
- `SPEED_OFF` (0): Motor off
- `SPEED_V1` (1): Low speed (1 relay active)
- `SPEED_V2` (2): Medium speed (1 relay active)
- `SPEED_V3` (3): High speed (1 relay active)

**Behavior:**
- Sets `targetSpeed`
- Initiates transition state machine in `update()`
- Logs speed change request

**Thread-Safe:** Yes (polled each loop iteration)

#### `MotorSpeed getSpeed()`
Returns current active motor speed.

**Returns:** Current speed constant

**Note:** Returns current speed, not target (use to detect active state)

#### `void update()`
Handles motor speed transitions with soft delays.

**State Machine:**
- **MOTOR_STATE_IDLE**: 
  - If `targetSpeed != currentSpeed`: Start transition (set timer)
  - Transition to MOTOR_STATE_INTERLOCK
- **MOTOR_STATE_INTERLOCK**:
  - Wait 500 ms for relay coil to release/settle
  - After delay: Write new relays, update LEDs
  - Transition back to IDLE

**Purpose of Interlock:** Prevents relay chatter and coil damage

#### `bool isTransitioning()`
Checks if motor is currently changing speed.

**Returns:** `true` if interlock delay active, `false` otherwise

### Private Methods

#### `void writeRelays(MotorSpeed speed)`
Sets relay GPIO pins for given speed.

**Logic:**
- All relays default to OFF (HIGH)
- V1: Pin 10 = LOW (relay energized)
- V2: Pin 11 = LOW
- V3: Pin 12 = LOW
- OFF: All HIGH (no relays energized)

**Interlock:** Only one relay energized at a time

#### `void updateLEDs(MotorSpeed speed)`
Reflects motor speed on LED outputs.

**Logic:**
- LED_V1 (Pin 9): HIGH if V1 active
- LED_V2 (Pin A0): HIGH if V2 active
- LED_V3 (Pin A1): HIGH if V3 active

### State Variables

```cpp
static MotorSpeed currentSpeed      // Currently active speed
static MotorSpeed targetSpeed       // Requested speed
static MotorState state             // Transition state machine
static unsigned long transitionStartTime // Interlock timer start
```

### Relay Logic

| Speed | Pin 10 | Pin 11 | Pin 12 | Relay(s) | Effect |
|-------|--------|--------|--------|----------|--------|
| OFF | HIGH | HIGH | HIGH | None | Motor off |
| V1 | LOW | HIGH | HIGH | Relay1 | Low speed |
| V2 | HIGH | LOW | HIGH | Relay2 | Medium speed |
| V3 | HIGH | HIGH | LOW | Relay3 | High speed |

**Note:** LOW activates relay (active-low control)

### Timing

- **Interlock Delay**: 500 ms (allows relay coil to release)
- **Transition Accuracy**: ±100 ms

---

## Light

### Purpose
Controls range hood light via relay and provides visual feedback via LED with configurable blinking patterns.

### Public Methods

#### `void begin()`
Initializes light control pin (13) and LED (A2).

**Setup:**
- Pin 13 (relay): OUTPUT, initialized to OFF (HIGH)
- Pin A2 (LED): OUTPUT, initialized based on stored state
- Load light state from Settings (persistent)

#### `void setState(bool on)`
Turns light on or off.

**Behavior:**
- `on = true`: Set Pin 13 to LOW (relay energized, light on)
- `on = false`: Set Pin 13 to HIGH (relay off, light off)
- Saves state to Settings (persisted to EEPROM)
- Logs state change

#### `bool getState()`
Returns current light on/off state.

**Returns:** `true` if light relay is energized, `false` otherwise

#### `void setBlinkMode(LightBlinkMode mode)`
Configures blinking pattern for timer feedback.

**Modes:**
- `BLINK_NONE`: LED off (static)
- `BLINK_NORMAL`: Slow blink (~500 ms period, timer > 5 min)
- `BLINK_WARNING`: Fast blink (~300 ms period, ≤ 5 min)
- `BLINK_CRITICAL`: Very fast blink (~100 ms period, ≤ 1 min)

#### `void setCustomBlinkPeriod(uint16_t periodMs)`
Sets custom blink period in milliseconds.

**Range:** 50-5000 ms

**Usage:** For specialized feedback (post-exhaust, etc.)

#### `void update()`
Updates LED blinking state based on elapsed time. Call every loop iteration.

**Algorithm:**
- Check elapsed time since last toggle
- If ≥ current blink period: Toggle LED state, reset timer
- Otherwise: No action

**Timing Accuracy:** ±50 ms

### State Variables

```cpp
static bool isLightOn               // Relay state
static LightBlinkMode blinkMode     // Current blinking pattern
static uint16_t customBlinkPeriodMs // Custom period (if set)
static unsigned long lastBlinkToggle // Last toggle timestamp
static bool blinkState              // Current LED state (HIGH/LOW)
```

### Blink Patterns

| Mode | Period | Use Case |
|------|--------|----------|
| BLINK_NONE | N/A | Static, no timer active |
| BLINK_NORMAL | ~500 ms | Timer active, > 5 minutes remaining |
| BLINK_WARNING | ~300 ms | Timer in final 5 minutes |
| BLINK_CRITICAL | ~100 ms | Timer in final minute |

---

## Hob2Hood

### Purpose
Decodes Hob2Hood (JVC-like) IR protocol frames and extracts commands.

### Public Methods

#### `void begin()`
Initializes Hob2Hood decoder state.

**Setup:**
- Clear command buffer
- Initialize timers

#### `void update()`
Processes incoming IR frames from IR module.

**Algorithm:**
1. Check if IR frame ready
2. If ready: Attempt decode
3. If decode successful: Extract command value
4. Apply hash validation
5. Check for duplicate within window (500 ms)
6. Set `lastCommand` if valid
7. Reset IR frame

#### `Hob2HoodCmd consumeCommand()`
Retrieves and clears latest decoded command.

**Returns:** Command constant (see enum below)

**Behavior:**
- Returns `lastCommand` if time < 500 ms (deduplication window)
- Otherwise returns `H2H_CMD_NONE`
- Clears command after return

**Thread-Safe:** Yes (called from StateMachine::update())

### Private Methods

#### `bool decodeFrame(uint16_t* buffer, uint8_t count, uint32_t& cmdVal)`
Attempts to decode IR timing array into 32-bit value.

**Algorithm:**
1. Validate frame has ≥34 elements (header + 16 bits minimum)
2. Verify header mark/space within tolerance
3. Decode 32-bit payload (tolerance check on each bit)
4. Return true if successful, false otherwise

**Tolerance:** ±35% on all timing measurements

#### `uint32_t decodeHash(uint16_t* buffer, uint8_t count)`
Calculates checksum hash of frame timing.

**Purpose:** Validate frame integrity and filter noise

#### `Hob2HoodCmd mapValueToCommand(uint32_t val)`
Maps decoded 32-bit value to command enum.

**Mapping:**
```cpp
val & 0xFF mapping:
0 → H2H_CMD_NONE
1 → H2H_CMD_ON
2 → H2H_CMD_OFF
3 → H2H_CMD_V0
4 → H2H_CMD_V1
5 → H2H_CMD_V2
6 → H2H_CMD_V3
7 → H2H_CMD_V4
else → H2H_CMD_NONE
```

### State Variables

```cpp
static Hob2HoodCmd lastCommand              // Last valid decoded command
static unsigned long lastCommandTime        // When command was decoded
static Hob2HoodCmd previousRawCommand       // Raw command (before validation)
static unsigned long previousRawCommandTime // Raw command timestamp
static Hob2HoodCmd lastValidatedCommand    // Last validated command
```

### Command Enum

```cpp
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
```

### Validation

- **Frame Length**: 34-128 timing elements
- **Header Tolerance**: ±35%
- **Bit Timing Tolerance**: ±35%
- **Deduplication Window**: 500 ms (prevent repeat triggering)
- **Hash Validation**: All bits must match bitwise decode

---

## IR

### Purpose
Interrupt-driven IR receiver that captures 38 kHz Hob2Hood protocol frames.

### Public Methods

#### `void begin()`
Initializes IR receiver on INT0 (Pin 2).

**Setup:**
- Pin 2: INPUT mode
- Attach INT0 interrupt on FALLING edge
- Initialize raw timing buffer
- Set frame-ready flag to false

#### `void update()`
Checks for frame timeout and marks frame ready if threshold met.

**Logic:**
- If receiving: Check if silence detected (>50 ms no pulses)
- If silence detected: Mark frame ready
- If receiving too long (>200 ms): Discard frame

#### `bool isFrameReady()`
Returns whether a complete frame is available for decoding.

**Returns:** `true` if frame buffer has valid data, `false` otherwise

**Note:** Frame remains available until `resetFrame()` called

#### `void getFrame(uint16_t* destBuffer, uint8_t& count)`
Copies captured frame timing array to destination.

**Parameters:**
- `destBuffer`: Destination array (≥64 elements)
- `count`: Number of timing measurements (output)

**Behavior:**
- Copies raw timing buffer to destination
- Sets count to number of valid elements
- Does not clear frame (call `resetFrame()`)

#### `void resetFrame()`
Clears frame buffer and resets receiver state.

**Usage:** Called after frame processed by Hob2Hood module

### Private Methods

#### `void handleInterrupt()`
Interrupt service routine for INT0 (called on every falling/rising edge).

**Timing Capture:**
1. Read current microsecond counter
2. Calculate elapsed time since last edge
3. Append timing to raw buffer
4. If buffer full (64 elements): Mark complete

**Timing Resolution:** ±1 µs (hardware timer)

### State Variables

```cpp
static volatile uint16_t rawBuffer[64]  // Timing array (microseconds)
static volatile uint8_t rawCount        // Number of entries in buffer
static volatile uint32_t lastInterruptTime // Last edge timestamp
static volatile bool frameReady         // Frame complete flag
```

### Frame Format

```
rawBuffer[0]   = Header mark duration (≈2104 µs)
rawBuffer[1]   = Header space duration (≈1052 µs)
rawBuffer[2]   = Bit 0 mark (≈526 µs)
rawBuffer[3]   = Bit 0 space (≈526 or 1578 µs)
...
rawBuffer[n-2] = Last bit mark
rawBuffer[n-1] = Stop bit / trailing space
```

### Timing

- **Carrier Frequency**: 38 kHz (26.3 µs period)
- **Measurement Resolution**: ±1 µs (hardware timer)
- **Frame Timeout**: >50 ms silence or >200 ms total

---

## Timer

### Purpose
Manages two independent countdown timers: manual timer and post-exhaust timer.

### Public Methods

#### `void begin()`
Initializes timer state.

**Setup:**
- Both timers inactive
- Durations set to 0
- Start times cleared

#### `void update()`
Decrements active timers. Call every loop iteration.

**Logic:**
1. If manual timer active: Calculate remaining seconds
2. If remaining ≤ 0: Stop manual timer
3. If post-exhaust active: Calculate remaining seconds
4. If remaining ≤ 0: Stop post-exhaust timer

**Accuracy:** ±100 ms per second

#### `void startManualTimer(uint16_t minutes)`
Starts manual countdown timer.

**Parameters:**
- `minutes`: Duration in minutes (0-65535)

**Behavior:**
- Sets duration in seconds (minutes * 60)
- Records start time
- Sets `manualTimerActive = true`
- Logs timer start

**Effect:** Blocks Hob2Hood commands until timer expires

#### `void stopManualTimer()`
Immediately stops manual timer.

**Behavior:**
- Sets `manualTimerActive = false`
- Logs timer stop

#### `bool isManualTimerActive()`
Checks if manual timer is currently running.

**Returns:** `true` if active and not expired, `false` otherwise

#### `uint32_t getManualTimerRemainingSeconds()`
Returns remaining seconds on manual timer.

**Returns:** Seconds remaining (0 if not active)

#### `void startPostExhaustion(uint16_t minutes)`
Starts post-exhaust timer.

**Usage:** Automatically called by StateMachine on Hob2Hood OFF command

**Behavior:**
- Sets duration in seconds
- Records start time
- Motor should be at SPEED_V1 when called
- Timer expires → motor stops

#### `void stopPostExhaustion()`
Immediately stops post-exhaust timer.

**Usage:** Called on button press or mode change

#### `bool isPostExhaustionActive()`
Checks if post-exhaust timer is running.

**Returns:** `true` if active, `false` otherwise

### State Variables

```cpp
// Manual Timer
static bool manualTimerActive
static unsigned long manualTimerStartTime
static uint32_t manualTimerDurationSeconds

// Post-Exhaust Timer
static bool postExhaustionActive
static unsigned long postExhaustionStartTime
static uint32_t postExhaustionDurationSeconds
```

### Defaults

- **Manual Timer Default**: 30 minutes (from EEPROM)
- **Post-Exhaust Default**: 5 minutes (from EEPROM)

---

## Logger

### Purpose
Printf-style serial logging with multiple severity levels.

### Public Methods

#### `void begin(unsigned long baudRate)`
Initializes serial UART.

**Parameters:**
- `baudRate`: Serial baud rate (default 115200)

**Called By:** StateMachine::begin()

#### `void debug(const char* format, ...)`
Logs DEBUG level message.

**Format String:** Standard printf format with `%d`, `%s`, `%c`, etc.

**Prefix:** `[DEBUG]`

#### `void info(const char* format, ...)`
Logs INFO level message.

**Prefix:** `[INFO]`

#### `void warn(const char* format, ...)`
Logs WARNING level message.

**Prefix:** `[WARN]`

#### `void error(const char* format, ...)`
Logs ERROR level message.

**Prefix:** `[ERROR]`

### Flash Memory Variants

All logging methods have overloads accepting `const __FlashStringHelper*` for format strings stored in Flash (PROGMEM).

**Usage:**
```cpp
Logger::info(F("System ready with mode: %s"), mode ? "Manual" : "Auto");
```

**Benefit:** Saves RAM space for large strings

### Output Format

```
[LEVEL] Message text\r\n
```

**Example:**
```
[INFO] IE60P Smart Controller v1.0 - Inicializando...
[DEBUG] Button V1 pressed
[WARN] Motor transition timeout detected
[ERROR] EEPROM magic byte invalid
```

### Implementation Notes

- Uses `Serial.print()` for output
- All arguments copied to stack (variadic handling)
- Printf formatting done locally (no external library)
- NULL-safe (checks format pointer)

---

## Settings

### Purpose
Runtime cache of persistent settings with lazy EEPROM write-through.

### Public Methods

#### `void begin()`
Loads all settings from EEPROM into RAM cache.

**Behavior:**
- Calls `EEPROMModule::begin()`
- Reads Mode, Light State, timers
- If EEPROM uninitialized: Sets defaults

#### `bool isManualMode()`
Returns current operational mode.

**Returns:** `true` = Manual, `false` = Automatic

#### `void setManualMode(bool manual)`
Sets operational mode and persists to EEPROM.

**Behavior:**
- Updates RAM cache
- Writes to EEPROM
- Logs change

#### `bool getLightState()`
Returns current light relay state.

**Returns:** `true` = ON, `false` = OFF

#### `void setLightState(bool on)`
Sets light state and persists.

#### `uint16_t getPostExhaustionTime()`
Returns post-exhaust duration in minutes.

**Returns:** 0-65535 minutes

#### `void setPostExhaustionTime(uint16_t minutes)`
Sets post-exhaust duration.

#### `uint16_t getTimerTime()`
Returns default manual timer duration in minutes.

#### `void setTimerTime(uint16_t minutes)`
Sets default timer duration.

### State Variables

```cpp
static bool manualMode              // Current mode
static bool lightState              // Light relay state
static uint16_t postExhaustionTime  // Post-exhaust duration (min)
static uint16_t timerTime           // Default timer duration (min)
```

### Persistence

All settings use write-through caching:
- Read from cache (fast)
- Write to cache + EEPROM (slow ~3.3 ms)
- EEPROM write happens synchronously

---

## EEPROMModule

### Purpose
Low-level EEPROM read/write operations with magic byte validation.

### Public Methods

#### `void begin()`
Initializes EEPROM module.

**Behavior:**
- Checks magic byte at 0x00
- If invalid: Calls `initializeDefault()`

#### `bool isInitialized()`
Checks if EEPROM has been previously initialized.

**Returns:** `true` if magic byte valid, `false` otherwise

#### `void initializeDefault()`
Writes all default values to EEPROM.

**Addresses Written:**
- `0x00`: Magic byte (0x5A)
- `0x01`: Mode = 0 (Auto)
- `0x02`: Light = 0 (OFF)
- `0x03-04`: Post-exhaust = 5 min
- `0x05-06`: Timer = 30 min

#### `bool readMode()`
Reads operational mode from EEPROM.

**Returns:** `true` = Manual, `false` = Auto

#### `void writeMode(bool manual)`
Writes operational mode to EEPROM.

#### `bool readLightState()`
Reads light relay state from EEPROM.

#### `void writeLightState(bool on)`
Writes light state to EEPROM.

#### `uint16_t readPostExhaustionTime()`
Reads post-exhaust duration (minutes).

#### `void writePostExhaustionTime(uint16_t minutes)`
Writes post-exhaust duration.

#### `uint16_t readTimerTime()`
Reads default timer duration (minutes).

#### `void writeTimerTime(uint16_t minutes)`
Writes default timer duration.

### Memory Map

```
Offset  Size   Field              Stored As
──────────────────────────────────────────────
0x00    1      Magic Byte         uint8_t
0x01    1      Mode               bool
0x02    1      Light State        bool
0x03-04 2      Post-Exhaust       uint16_t (little-endian)
0x05-06 2      Timer              uint16_t (little-endian)
```

### Timing

- **Read**: ~3 µs per byte
- **Write**: ~3.3 ms per byte (with erase cycle)

### Endianness

- **Architecture**: Little-endian (ATmega328P native)
- **16-bit Storage**: Lower byte at base address, upper byte at base+1

---

## Watchdog

### Purpose
Hardware watchdog protection against firmware lockups.

### Public Methods

#### `void begin()`
Enables hardware watchdog timer.

**Configuration:**
- Timeout: ~4 seconds (ATmega328P default)
- Action: System reset on timeout
- Disable on next boot: Handled by StateMachine::begin()

**Called By:** StateMachine::begin() (after all modules initialized)

#### `void feed()`
Resets watchdog counter.

**Must Be Called:** Every loop iteration (at least every 4 seconds)

**Called By:** StateMachine::update() at end of each cycle

### Watchdog Recovery

If watchdog timer expires:
1. Hardware triggers system reset
2. ATmega328P clears all registers
3. Bootloader runs (1KB at 0x7800)
4. `setup()` called (watchdog disabled initially)
5. All modules reinitialize
6. Watchdog re-enabled

**Recovery Time**: ~1 second total

### Code Snippet

```cpp
// Enable watchdog (~4 second timeout)
wdt_enable(WDTO_4S);

// Reset watchdog counter (prevents reset)
wdt_reset();

// Disable watchdog (for safe reinitialization)
wdt_disable();
```

---

## Summary Table

| Module | Purpose | Loop Call | State Vars | EEPROM |
|--------|---------|-----------|-----------|--------|
| StateMachine | Coordinator | ✓ (main) | 8+ | No |
| Buttons | Input debounce | ✓ | 14+ | No |
| Motor | Relay control | ✓ | 4 | No |
| Light | Light control | ✓ | 5 | Yes (state) |
| Hob2Hood | IR decode | ✓ | 5 | No |
| IR | Capture frames | ✓ | 4 | No |
| Timer | Countdowns | ✓ | 6 | No |
| Logger | Serial output | ✗ | 0 | No |
| Settings | Config cache | ✗ | 4 | Yes (write-through) |
| EEPROM | Persistent store | ✗ | 0 | Yes (primary) |
| Watchdog | Safety | ✓ | 0 | No |

