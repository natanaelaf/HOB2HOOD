# Troubleshooting Guide

Comprehensive troubleshooting guide for common issues with the IE60P Smart Range Hood Controller.

## Table of Contents

1. [Diagnostic Steps](#diagnostic-steps)
2. [Power & Connection Issues](#power--connection-issues)
3. [Input Device Problems](#input-device-problems)
4. [Output Device Problems](#output-device-problems)
5. [Firmware & Software Issues](#firmware--software-issues)
6. [Communication Issues](#communication-issues)
7. [Safety & Protection](#safety--protection)
8. [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Diagnostic Steps

### Before Troubleshooting

Perform these checks first:

1. **Visual Inspection**
   - [ ] All connections secure
   - [ ] No obvious damage
   - [ ] No burning smell (immediate power off if present)
   - [ ] All LEDs accounted for

2. **Power Check**
   - [ ] 5V power supply plugged in
   - [ ] Power supply indicator light on
   - [ ] Arduino power LED (green) illuminated
   - [ ] No tripped breakers

3. **Serial Connection**
   - [ ] USB cable firmly inserted
   - [ ] No "Unknown Device" errors in Device Manager
   - [ ] Serial Monitor running at 115200 baud
   - [ ] Can see startup messages

4. **Log Review**
   - [ ] Open Serial Monitor
   - [ ] Press Arduino reset button
   - [ ] Record all startup messages
   - [ ] Look for [ERROR] or [WARN] tags

### Gather Diagnostic Information

Create a diagnostic report:

```
Device: ___________
Firmware Version: ___________
EEPROM State: ___________
Current Mode: ___________

Recent Changes:
- Last firmware update: ___________
- Last hardware modification: ___________
- Recent user actions: ___________

Serial Output (First 10 lines):
[1] ___________
[2] ___________
...
```

---

## Power & Connection Issues

### Symptom: No Power (Nothing Happens)

**Checklist:**

- [ ] Is power supply plugged into wall outlet?
- [ ] Does power supply have ON/OFF switch? (check it's ON)
- [ ] Correct voltage selected on power supply? (should be 5V)
- [ ] Power LED on supply illuminated?
- [ ] Barrel jack cable firmly inserted into Arduino?

**Diagnostic Commands:**

```bash
# Test Arduino power independently (USB)
1. Connect Arduino to PC via USB only (no external power)
2. Check power LED on Arduino
3. Should illuminate even without external supply
```

**Solution:**

| Finding | Fix |
|---------|-----|
| Power supply unplugged | Plug into outlet |
| Power supply off | Turn ON switch |
| Loose barrel jack | Reseat cable firmly |
| Wrong voltage | Check supply output (should be 5V DC) |
| Bad power cable | Try different cable |
| Blown fuse in supply | Replace fuse (if accessible) |

**Still Not Working?**
- Test power supply on another device
- Try different power cable
- Check for visible damage to barrel jack
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

### Symptom: No Serial Output (Arduino Powered but No Communication)

**Checklist:**

- [ ] Arduino USB cable connected?
- [ ] Correct COM port selected in Arduino IDE?
- [ ] Serial Monitor baud rate set to 115200?
- [ ] Arduino not in bootloader mode (no solid LED)?

**Diagnostic:**

```bash
# Check COM port in Device Manager
Windows:
1. Right-click Start menu → Device Manager
2. Expand "Ports (COM & LPT)"
3. Look for "Arduino UNO" or "USB Serial Device"
4. Note the COM number
```

**Solution:**

| Finding | Fix |
|---------|-----|
| COM port not visible | Install CH340 drivers (see [Installation Guide](INSTALLATION.md)) |
| Wrong COM port selected | Match Device Manager to Arduino IDE |
| Baud rate wrong | Change to 115200 in Serial Monitor |
| Stale connection | Close/reopen Serial Monitor |

**Test Fix:**

```bash
1. Open Device Manager
2. Note COM port (e.g., COM4)
3. Open Arduino IDE Serial Monitor
4. Select same COM port and 115200 baud
5. Press Arduino reset button
6. Should see [INFO] messages
```

**Still Not Working?**
- Try different USB cable
- Try different USB port on computer
- Restart Arduino IDE
- Restart computer
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Input Device Problems

### Symptom: Button Not Responding

**Checklist:**

- [ ] Button LED illuminates when pressed?
- [ ] Serial Monitor shows button event?
- [ ] Button physically clicks/tactile feedback?
- [ ] Button wire firmly connected?

**Diagnostic:**

```bash
# Test individual button
1. Open Serial Monitor (115200)
2. Press button repeatedly
3. Check for log messages:
   [DEBUG] Button debounce: XXms
   [INFO] Botoes: Clique V1.
   
If no message appears:
4. Check if Serial Monitor has errors logged
5. Try pressing other buttons (test all 5)
6. Record which button(s) not working
```

**Solution:**

| Finding | Fix |
|---------|-----|
| Button not clicking | Check if button mechanically stuck |
| No Serial messages | Check button wiring (GPIO connection) |
| LED not lighting | Check LED polarity or resistor |
| Intermittent response | Button wire loose or corroded |

**Wiring Check:**

```
Button OFF (Pin 3) Test:
1. Power off system
2. Disconnect button wire from Pin 3
3. Measure resistance between button terminals:
   - Button open: ~infinite (>10MΩ)
   - Button pressed: <1Ω
4. Reconnect and power on
```

**Still Not Working?**
- Check pin not damaged (try different pin)
- Replace button with known-good one
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

### Symptom: IR Remote Not Responding

**Checklist:**

- [ ] IR receiver power LED illuminated (if present)?
- [ ] IR receiver lens clean (no dust/fingerprints)?
- [ ] Remote batteries fresh?
- [ ] System in Automatic mode (LED OFF is OFF)?
- [ ] Manual timer not active (LED Light not blinking)?

**Diagnostic:**

```bash
# Test IR reception
1. Open Serial Monitor (115200)
2. Point remote at IR receiver
3. Press remote button
4. Check for messages:
   [DEBUG] IR frame ready: 48 elements
   [INFO] Hob2Hood: Processando comando X
   
If no frame received:
5. Try pressing different remote buttons (test several)
6. Try different Hob2Hood remotes if available
7. Move closer to receiver (0.5 meters away)
```

**Solution:**

| Finding | Fix |
|---------|-----|
| No IR frames received | Check IR receiver power (+5V) |
| IR receiver dim | Clean lens with soft cloth |
| Wrong mode | Switch to Automatic mode (OFF LED should be OFF) |
| Timer active | Stop manual timer or wait for expiry |
| Remote doesn't work | Test remote on TV (verify it works) |

**IR Power Check:**

```
With multimeter:
1. Power on system
2. Test IR module pin 2 (+5V):
   - Should read +4.7 to +5.3V
3. Test IR module pin 1 (GND):
   - Should read 0V (reference)
   
If not correct voltage:
- Check power supply connection
- Test relay module power
- Check wiring from relay to IR
```

**Still Not Working?**
- Try different remote (borrow one if possible)
- Replace IR receiver module
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Output Device Problems

### Symptom: Motor Not Running

**Checklist:**

- [ ] Motor relay clicking (audible sound)?
- [ ] Motor relay power LEDs on?
- [ ] Motor speed LED illuminated (V1/V2/V3)?
- [ ] Load connected to relay (fan motor, etc.)?

**Diagnostic:**

```bash
# Test motor control
1. Open Serial Monitor (115200)
2. Press V1 button
3. Check Serial output:
   [INFO] Botoes: Clique V1.
   [DEBUG] Motor speed set to: 1
   
4. Check for relay clicking sound (audible)
5. If no click: relay not energizing
6. If clicks but no motor: check motor connections
```

**Solution:**

| Finding | Fix |
|---------|-----|
| No relay click | Check relay power supply (+5V) |
| Click but no motor | Check motor load connections |
| Wrong speed | Check relay contact (NO/NC) |
| Intermittent operation | Check wiring for corrosion |

**Relay Module Test:**

```
With multimeter:
1. Relay should click when motor activated
2. Measure relay coil voltage (should be 5V when active)
3. Measure relay contact resistance:
   - Open (inactive): >10MΩ
   - Closed (active): <1Ω
   
If no clicks or wrong voltage:
- Check Arduino pin output (should toggle HIGH/LOW)
- Check relay module power
```

**Still Not Working?**
- Test relay with simple 5V circuit (known working)
- Replace relay module
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

### Symptom: Light Not Turning On

**Checklist:**

- [ ] Light LED illuminated when activated?
- [ ] Light relay clicking when activated?
- [ ] Load (light bulb, etc.) connected?
- [ ] Light relay module pin 4 has correct signal?

**Diagnostic:**

```bash
# Test light control
1. Open Serial Monitor
2. Press LIGHT button
3. Check Serial output:
   [INFO] Botoes: Clique Lampada. Alternando estado.
   
4. Observe relay module pin 4 (Light relay):
   - Should blink if relay switching
5. Measure voltage at light relay contacts
```

**Solution:**

| Finding | Fix |
|---------|-----|
| No relay click | Check relay module power |
| Click but no light | Check light load connections |
| LED not lighting | Check LED polarity (anode/cathode) |
| Intermittent | Check wiring for loose connections |

**Light Relay Test:**

Same as motor relay test above, but for Pin 13 (Light relay).

**Still Not Working?**
- Replace light bulb (test with known-good bulb)
- Replace relay module
- Check light load separately
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

### Symptom: LEDs Not Illuminating

**Checklist:**

- [ ] LED physically installed (not loose)?
- [ ] LED polarity correct (anode/cathode)?
- [ ] Current-limiting resistor present (220Ω)?
- [ ] Arduino GPIO pin connected?

**Diagnostic:**

```bash
# Test LED individually
1. Disconnect LED from GPIO pin
2. Connect directly to 5V through 220Ω resistor:
   5V ── 220Ω ── LED anode
           LED cathode ── GND
3. LED should illuminate
4. If not: LED is dead, replace it
5. If yes: Problem is Arduino or wiring
```

**Solution:**

| Finding | Fix |
|---------|-----|
| LED doesn't light on direct test | Replace LED (dead) |
| LED works directly but not on GPIO | Check GPIO pin connection |
| Wrong GPIO pin | Verify pin mapping in code |
| LED wrong direction | Check anode (long) vs cathode (short) |

**LED Polarity Diagram:**

```
Correct polarity:
  5V ──┐
       │
     220Ω
       │
  Arduino GPIO (HIGH output)
       │
      ⛔  ← LED (anode/long pin)
      ││
      │└── connects to resistor
      │
  LED cathode (short pin)
       │
      GND

If LED cathode connects to 5V:
  Wrong polarity → LED will NOT light
```

**Still Not Working?**
- Replace LED with known-good one
- Test GPIO pin voltage (should be 5V when HIGH)
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Firmware & Software Issues

### Symptom: System Crashes / Unexpected Resets

**Checklist:**

- [ ] Does watchdog timer fire (~4 second gaps)?
- [ ] Are Serial messages cut off?
- [ ] Does system recover and restart?
- [ ] Any [ERROR] messages in Serial?

**Diagnostic:**

```bash
# Check Serial Monitor for reset patterns
1. Open Serial Monitor (115200)
2. Look for repeating pattern:
   [INFO] Sistema Inicializado com Sucesso.
   [INFO] Sistema Inicializado com Sucesso.
   [INFO] Sistema Inicializado com Sucesso.
   
This indicates repeated resets (watchdog firing)
```

**Solution:**

| Finding | Fix |
|---------|-----|
| Repeating startup messages | Firmware infinite loop (recompile/upload) |
| Occasional resets | Check watchdog feeding (should be every 10ms) |
| Resets during specific action | That action causing loop (test individually) |

**Firmware Reset:**

```bash
# If firmware corrupted, recompile fresh
1. Close Arduino IDE
2. Delete build directory (.pio or build/)
3. Reopen project
4. Recompile (clean build)
5. Upload again
```

**Still Crashing?**
- Test with minimal code (just blink LED)
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

### Symptom: High RAM Usage / Insufficient Memory

**Checklist:**

- [ ] Compilation shows percentage near 100%?
- [ ] System behaves erratically?
- [ ] Serial output corrupted?

**Diagnostic:**

```bash
# Check compilation memory usage
Arduino IDE output should show:
Sketch uses 28764 bytes (93%) of program storage space.
Global variables use 1864 bytes (91%) of dynamic memory.

If SRAM > 95%: Memory critical
```

**Solution:**

Use PROGMEM for strings (Flash storage instead of RAM):

```cpp
// Bad: String in RAM
Logger::info("System initialized");

// Good: String in Flash (PROGMEM)
Logger::info(F("System initialized"));
```

**Still Full?**
- Remove debug logging (if unnecessary)
- Simplify string messages
- Contact support for optimization

---

## Communication Issues

### Symptom: Serial Monitor Shows Garbage Characters

**Checklist:**

- [ ] Serial Monitor baud rate correct (115200)?
- [ ] USB cable firmly inserted?
- [ ] COM port correct?

**Diagnostic:**

```bash
# Check baud rate
Arduino output: "ôü ┌●[INFO]"
Means: Baud rate mismatch
Solution: Set to 115200 (not 9600 or 115200)
```

**Solution:**

| Symptom | Baud Rate to Try |
|---------|-----------------|
| Gibberish | 9600 (if not 115200) |
| Gibberish | 115200 (if not 9600) |
| Half characters | 57600 |

**Test Fix:**

```bash
1. Open Serial Monitor
2. Change baud rate dropdown
3. Try different rates
4. Look for readable output
5. Stick with that rate
```

---

### Symptom: Intermittent Serial Connection Loss

**Checklist:**

- [ ] USB cable working (test with phone charger)?
- [ ] USB port not loose in Arduino?
- [ ] Power supply stable?
- [ ] Computer USB port working?

**Diagnostic:**

```bash
# Check for connection drops
Serial Monitor shows:
1. Messages stop appearing
2. Reconnect message after few seconds
3. Repeats cyclically

This indicates loose/bad cable or port
```

**Solution:**

| Finding | Fix |
|---------|-----|
| Loose barrel jack | Reseat firmly, tighten if adjustable |
| Loose USB cable | Reseat in both ends |
| Intermittent drops | Try different USB port |
| Still drops | Replace cable or test different computer |

---

## Safety & Protection

### Symptom: Watchdog Timer Constantly Resetting

**Checklist:**

- [ ] Serial shows repeated startup messages?
- [ ] LEDs flashing on/off repeatedly?
- [ ] Motor or relays not working at all?

**Diagnostic:**

```bash
# Watchdog reset pattern
Normal operation:
[INFO] System initialized
[INFO] Motor: V1
[INFO] Button OFF pressed

Watchdog issue (repeats):
[INFO] System initialized
[INFO] System initialized
[INFO] System initialized
```

**Causes & Solutions:**

| Cause | Solution |
|-------|----------|
| Firmware infinite loop | Recompile and upload fresh |
| Button continuously pressed | Release button, check for stuck button |
| Interrupthang | Disconnect IR receiver temporarily |
| Slow processing | Simplify code, remove heavy operations |

**Test Fix:**

```bash
1. Disconnect all inputs (buttons, IR)
2. Power on
3. If still resets: firmware issue
4. Reconnect one input at a time
5. Identify which input causes hang
```

---

### Symptom: Relay Clicking/Chattering Continuously

**Checklist:**

- [ ] Audible relay clicking sound persistent?
- [ ] Motor speed LED flashing?
- [ ] Relay getting hot?

**Danger:** Relay coil may burn out!

**Causes & Solutions:**

| Cause | Solution |
|-------|----------|
| Unstable power supply | Check 5V output with multimeter |
| Add stabilization capacitor | Add 100µF across 5V and GND |
| Relay contact bouncing | Add small delay between state changes |
| Motor load too high | Verify motor rated for relay |

**Quick Fix:**

```bash
1. Power off immediately
2. Add 100µF capacitor across 5V/GND
3. Wait 10 seconds
4. Power back on
5. Test if chatter stopped
```

**Still Chattering?**
- Replace relay module
- Check power supply output voltage
- See [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Advanced Troubleshooting

### Multimeter Testing Reference

**Voltage Measurements:**

| Point | Expected | Min | Max | Note |
|-------|----------|-----|-----|------|
| 5V Rail | 5.0V | 4.8V | 5.2V | Power supply output |
| Arduino VCC | 5.0V | 4.8V | 5.2V | At microcontroller pin |
| Relay Module VCC | 5.0V | 4.8V | 5.2V | At relay input power |
| GPIO HIGH | 5.0V | 4.7V | 5.3V | When output is HIGH |
| GPIO LOW | 0V | -0.2V | 0.3V | When output is LOW |
| IR Receiver OUT (idle) | 5.0V | 4.8V | 5.2V | No IR signal |

**Resistance Measurements:**

| Component | Expected | Min | Max |
|-----------|----------|-----|-----|
| Resistor 220Ω | 220Ω | 198Ω | 242Ω |
| Resistor 10kΩ | 10kΩ | 9kΩ | 11kΩ |
| LED (forward) | - | 0.5Ω | 5Ω |
| LED (reverse) | - | >10MΩ | ∞ |
| Button (open) | ∞ | >10MΩ | ∞ |
| Button (pressed) | <1Ω | 0Ω | 1Ω |

### EEPROM Corruption Recovery

**Symptoms:**
- System starts but no stored settings recovered
- Serial shows: `[DEBUG] EEPROM invalid magic byte`
- Settings always default

**Recovery Process:**

```bash
1. Connect via Serial Monitor (115200)
2. Observe message:
   [DEBUG] EEPROM: Initializing default values...
   [INFO] EEPROM: Initialization complete

3. If continues to initialize:
   a) Power cycle (off/on)
   b) Wait 10 seconds
   c) Should see normal startup

4. If still corrupted:
   a) Contact support with serial output
   b) May need firmware recompile with EEPROM reset flag
```

### Complete System Reset

**Warning:** This erases all stored settings!

**Process:**

```bash
# Via Arduino IDE
1. Open Arduino IDE
2. Sketch → Manage Libraries
3. Search "EEPROM"
4. Install official EEPROM library
5. Create new sketch:

#include <EEPROM.h>
void setup() {
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0xFF);  // Erase
    }
    digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {}

6. Upload
7. When complete, reupload IE60P firmware
```

### Component-Level Diagnostics

**Test Arduino UNO GPIO:**

```bash
Simple blink test (just LED + resistor, no complex system):
1. Create minimal sketch:

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}

2. Upload
3. LED should blink once per second
4. If it doesn't: Arduino may be damaged
```

**Test Relay Module Independently:**

```
1. Disconnect from Arduino
2. Apply 5V directly to relay coil
3. Listen for click
4. Measure relay contact with multimeter:
   - Open: >10MΩ
   - Click applied: <1Ω
   
If fails: Relay module damaged
```

---

## When to Contact Support

**Gather this information before contacting:**

```
Device Information:
- Firmware version: ________________
- Board type: Arduino UNO? ________
- Hardware configuration: ________________

Problem Description:
- What doesn't work: ________________
- When it started: ________________
- Changes before problem: ________________

Diagnostic Data:
- Serial output (copy/paste): ________________
- Multimeter readings: ________________
- Specific steps to reproduce: ________________
```

**Issues requiring professional help:**

- ✗ Hardware damage (visible burns, broken traces)
- ✗ Component failures (capacitors, chips)
- ✗ Power supply issues
- ✗ Persistent EEPROM corruption
- ✗ Firmware won't upload after multiple tries

