# Installation Guide

Complete step-by-step installation and setup guide for the IE60P Smart Range Hood Controller.

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Software Installation](#software-installation)
3. [Hardware Assembly](#hardware-assembly)
4. [Firmware Compilation](#firmware-compilation)
5. [Firmware Upload](#firmware-upload)
6. [First-Run Configuration](#first-run-configuration)
7. [Verification Steps](#verification-steps)

---

## System Requirements

### Development Environment

**Option 1: Arduino IDE (Recommended for Beginners)**
- Arduino IDE 1.8.13 or later
- Windows 7, 8, 10, 11
- Administrator access for driver installation
- 200 MB free disk space

**Option 2: PlatformIO (Recommended for Advanced Users)**
- VS Code or other editor
- PlatformIO CLI or extension
- Python 3.6+
- 500 MB free disk space

### Hardware Requirements

**Minimum:**
- Arduino UNO board (ATmega328P)
- USB Type-B cable
- Computer with USB port
- 5V power supply (2A)

**Complete Setup:**
- All items above plus:
- 5 push-buttons
- 4-channel 5V relay module
- 38 kHz IR receiver module
- 5 LEDs with 220Ω resistors
- Connecting wires and breadboard
- See [Hardware Setup Guide](HARDWARE_SETUP.md) for complete bill of materials

---

## Software Installation

### Step 1: Install Arduino IDE

**Windows:**

1. Visit [Arduino Official Site](https://www.arduino.cc/en/software)
2. Download "Windows Installer" (latest version)
3. Run installer with Administrator privileges
4. Accept license agreement
5. Choose installation directory
6. Select component options:
   - ✓ Arduino IDE
   - ✓ USB driver
7. Complete installation

**Verify Installation:**
- Launch Arduino IDE from Start menu
- Should show version number in title bar

### Step 2: Install Board Package

Arduino IDE comes with UNO support by default. To verify:

1. Open Arduino IDE
2. Go to `Tools` → `Board` → `Arduino AVR Boards`
3. You should see "Arduino UNO" listed
4. If not found, see "Troubleshooting" section

### Step 3: Install Serial Driver (if needed)

**For Arduino UNO with CH340 chip:**

1. Plug Arduino into USB port
2. Device Manager shows "Unknown Device" or "COM error"
3. Download CH340 driver:
   - [Windows CH340 Driver](http://www.wch.cn/download/CH341SER_ZIP.html)
4. Extract and run installer
5. Restart computer
6. Verify in Device Manager:
   - Ports (COM & LPT) → "USB-SERIAL CH340" → COM4 (or higher)

---

## Hardware Assembly

### Quick Assembly Checklist

**Phase 1: Planning**
- [ ] Obtain all components (see Hardware Setup)
- [ ] Prepare breadboard and connecting wires
- [ ] Label each connection
- [ ] Create a test rig before final installation

**Phase 2: Basic Connections**
- [ ] Connect 5V power supply positive to breadboard +rail
- [ ] Connect 5V power supply negative to breadboard -rail (GND)
- [ ] Power Arduino from same GND rail
- [ ] Add 100µF capacitor across 5V and GND

**Phase 3: Button Connections**
- [ ] Connect all button GND pins to common GND rail
- [ ] Connect button V1 to Arduino Pin 4
- [ ] Connect button V2 to Arduino Pin 5
- [ ] Connect button V3 to Arduino Pin 6
- [ ] Connect button Light to Arduino Pin 7
- [ ] Connect button OFF to Arduino Pin 3

**Phase 4: LED Connections**
- [ ] LED OFF: Anode via 220Ω to Pin 8, Cathode to GND
- [ ] LED V1: Anode via 220Ω to Pin 9, Cathode to GND
- [ ] LED V2: Anode via 220Ω to Pin A0, Cathode to GND
- [ ] LED V3: Anode via 220Ω to Pin A1, Cathode to GND
- [ ] LED Light: Anode via 220Ω to Pin A2, Cathode to GND

**Phase 5: Relay Connections**
- [ ] Connect relay GND to Arduino GND rail
- [ ] Connect relay +5V to power supply 5V
- [ ] Connect relay PIN1 to Arduino Pin 10
- [ ] Connect relay PIN2 to Arduino Pin 11
- [ ] Connect relay PIN3 to Arduino Pin 12
- [ ] Connect relay PIN4 to Arduino Pin 13

**Phase 6: IR Receiver**
- [ ] Connect IR GND to Arduino GND
- [ ] Connect IR +5V to power supply 5V
- [ ] Connect IR OUT to Arduino Pin 2

---

## Firmware Compilation

### Method 1: Using Arduino IDE

**Step-by-Step:**

1. **Open Project:**
   - Launch Arduino IDE
   - Click `File` → `Open...`
   - Navigate to `src/main.cpp` in IE60P folder
   - All source files in `src/` should appear as tabs

2. **Select Board:**
   - Go to `Tools` → `Board` → `Arduino AVR Boards`
   - Select `Arduino UNO`

3. **Verify Compilation:**
   - Click `Sketch` → `Verify/Compile`
   - Progress bar appears at bottom
   - Success message: "Sketch uses X bytes (Y% of 30720 bytes)"
   - Error message: Check [Troubleshooting](#troubleshooting)

4. **View Compilation Details:**
   - Press `Ctrl+Alt+V` for verbose mode
   - Re-compile to see detailed output
   - Look for any warnings (yellow text)

**Expected Output:**
```
Compiling sketch...
[===   ] 30%
[=====   ] 50%
[====   ] 100%

Compiling libraries...

Linking everything together...
Sketch uses 28764 bytes (93%) of program storage space.
Maximum is 30720 bytes.
Global variables use 1864 bytes (91%) of dynamic memory.
Leaving 154 bytes for local variables.
Maximum is 2048 bytes.
```

### Method 2: Using PlatformIO

**Step-by-Step:**

1. **Install PlatformIO CLI:**
   ```bash
   pip install platformio
   ```

2. **Verify Installation:**
   ```bash
   platformio --version
   ```

3. **Navigate to Project:**
   ```bash
   cd path/to/IE60P
   ```

4. **Compile:**
   ```bash
   platformio run
   ```

5. **View Output:**
   ```
   Processing uno (platform: atmelavr; board: uno; framework: arduino)
   ────────────────────────────────────────────────────────────
   ENVIRONMENT        STATUS    DURATION
   ────────────────────────────────────────────────────────────
   uno                SUCCESS   00:00:15
   ```

---

## Firmware Upload

### Step 1: Connect Arduino

1. Connect Arduino UNO to computer via USB cable
2. Wait 2 seconds for device recognition
3. LED on Arduino should illuminate (power indicator)
4. No "Unknown Device" errors in Device Manager

### Step 2: Select COM Port

**Arduino IDE:**
1. `Tools` → `Port`
2. Select your COM port (e.g., `COM4`)
3. Click to confirm

**PlatformIO:**
- Auto-detects COM port
- Or specify manually:
  ```bash
  platformio run --target upload --upload-port COM4
  ```

### Step 3: Upload Firmware

**Arduino IDE Method:**

1. Ensure code compiled successfully (see above)
2. Click `Sketch` → `Upload` (or Ctrl+U)
3. Monitor bottom status bar:
   ```
   Uploading...
   Compiling sketch... ✓
   Uploading sketch... ✓
   
   Done uploading.
   ```
4. Progress LED on Arduino blinks rapidly during upload
5. Upload completes in ~10 seconds

**PlatformIO Method:**

```bash
platformio run --target upload
```

Expected output:
```
Uploading .pio/build/uno/firmware.hex
[████████████████████] 100%

Successfully uploaded.
```

### Step 4: Verify Upload

**Check Serial Output:**
1. Open Serial Monitor:
   - Arduino IDE: `Tools` → `Serial Monitor` (or Ctrl+Shift+M)
   - Set baud rate to `115200`
2. Press Arduino reset button
3. Should see startup messages:
   ```
   [INFO] IE60P Smart Controller v1.0 - Inicializando...
   [DEBUG] Buttons: Initialized
   [DEBUG] Motor: Initialized
   ...
   [INFO] Sistema Inicializado com Sucesso.
   ```

If nothing appears:
- Check COM port is correct
- Close/reopen Serial Monitor
- Check baud rate is 115200
- See Troubleshooting section

---

## First-Run Configuration

### Initial Setup Process

**Step 1: Power On**
- Connect 5V power supply
- Arduino LED (power) should illuminate
- All module LEDs may flash briefly

**Step 2: Check Initial State**
- Observe serial output (should show initialization)
- LED OFF indicates current mode:
  - **LED ON (lit)**: Manual mode (default if EEPROM not initialized)
  - **LED OFF (dark)**: Automatic mode
- All other LEDs should be off

**Step 3: Test Button Inputs**
Press each button and check Serial Monitor:

```bash
OFF button short-press:
[INFO] Botoes: Clique OFF. Desligando motor e timers.

V1 button press:
[INFO] Botoes: Clique V1.
[DEBUG] Motor speed set to: 1

Light button press:
[INFO] Botoes: Clique Lampada. Alternando estado.
```

**Step 4: Test IR Receiver**
Point Hob2Hood-compatible remote at IR receiver:

```bash
[DEBUG] IR frame ready: 48 elements
[INFO] Hob2Hood: Processando comando 2
```

If no IR frames detected:
- Check IR receiver power (should be 5V)
- Verify connection to Pin 2
- See [Troubleshooting](#troubleshooting)

### EEPROM Initialization

First-run automatic behavior:
1. Arduino checks EEPROM magic byte (address 0x00)
2. If not found (0x5A):
   - Initializes all EEPROM values to defaults
   - Writes magic byte to validate
   - Logs: `[DEBUG] EEPROM initialized with defaults`
3. If found:
   - Loads previous settings (mode, timers, light state)
   - Resumes with last-known configuration

**Default Values:**
```
Mode:             Auto (0)
Light:            OFF (0)
Post-Exhaust:     5 minutes
Timer:            30 minutes
```

---

## Verification Steps

### Test Checklist

#### Power & Basic Functionality

- [ ] Arduino powers on (power LED illuminates)
- [ ] Serial connection works at 115200 baud
- [ ] Startup messages appear in Serial Monitor
- [ ] No watchdog resets occur (no unexpected reboots)

#### Button Input Tests

- [ ] OFF button: Motor stops, timers clear
- [ ] V1/V2/V3 buttons: Motor speed changes, LEDs update
- [ ] Light button: Light relay toggles, LED updates
- [ ] OFF long-press (3s): Mode LED blinks 3 times, logs show mode change

#### LED Output Tests

- [ ] LED OFF: Indicates current mode (Manual=ON, Auto=OFF)
- [ ] LED V1/V2/V3: Lights up when speed active
- [ ] LED Light: Lights on/off with light relay

#### Motor Relay Tests

- [ ] Relay contacts switch (audible click when commanded)
- [ ] Only one relay active at a time (no simultaneous speeds)
- [ ] Transition delay works (~500 ms between speed changes)
- [ ] Relay LEDs reflect relay state

#### IR Receiver Tests

- [ ] IR module receives Hob2Hood commands (frames logged)
- [ ] Commands decoded correctly (speed commands trigger motor changes)
- [ ] IR ignored in Manual mode
- [ ] IR blocked when manual timer active

#### Timer Functionality

- [ ] Manual timer starts (Light button long-press 3s)
- [ ] Speed buttons configure timer (V1=1min, V2=2min, V3=3min, Light=5min)
- [ ] Timer LED blinks with pattern (NORMAL → WARNING → CRITICAL)
- [ ] Timer expires and stops (motor turns off automatically)
- [ ] Manual timer blocks IR commands

#### EEPROM Persistence

- [ ] Power cycle system
- [ ] Settings retained (mode, light state)
- [ ] Timers default to configured values
- [ ] No unexpected resets

#### Safety Features

- [ ] Watchdog timer feeds regularly (check serial logs)
- [ ] No watchdog-triggered resets
- [ ] Motor stops immediately on OFF button
- [ ] Post-exhaust timer runs for configured duration

### Performance Baseline

Record these values for reference:

```
Current Draw (Idle):           _____ mA
Current Draw (All LEDs):       _____ mA
Current Draw (One Relay):      _____ mA
Loop Frequency:                _____ Hz
Serial Debug Output Level:     _____ (DEBUG/INFO/WARN)
```

### Commissioning Report

Document any deviations:

```
TEST                           RESULT              NOTES
─────────────────────────────────────────────────────────
Power-on behavior              ✓ Pass / ✗ Fail     ____________
Button debouncing              ✓ Pass / ✗ Fail     ____________
Motor relay operation          ✓ Pass / ✗ Fail     ____________
IR reception                   ✓ Pass / ✗ Fail     ____________
Timer functionality            ✓ Pass / ✗ Fail     ____________
LED indicators                 ✓ Pass / ✗ Fail     ____________
EEPROM persistence            ✓ Pass / ✗ Fail     ____________
Watchdog protection           ✓ Pass / ✗ Fail     ____________
```

---

## Post-Installation

### Configuration Options

After successful installation, you can customize:

**Via EEPROM (Persistent):**
- Default timer duration (30 minutes)
- Post-exhaust duration (5 minutes)
- Light state (on/off)
- Operating mode (auto/manual)

**Access via serial terminal:**
```bash
# Read current timer setting
[Request: readTimerTime]

# Set timer to 60 minutes
[Command: setTimerTime(60)]
```

**Firmware Changes:**
- IR protocol timing (if using non-standard remote)
- Debounce duration
- Button behavior
- See source code comments for customization points

### Regular Maintenance

**Monthly:**
- [ ] Clean IR receiver lens (use soft cloth)
- [ ] Check for error messages in Serial Monitor
- [ ] Verify all buttons respond correctly

**Quarterly:**
- [ ] Check relay contacts for pitting
- [ ] Inspect wiring for corrosion
- [ ] Test full range of motor speeds
- [ ] Verify timer durations are correct

**Annually:**
- [ ] Firmware update (if new version available)
- [ ] EEPROM checksum verification
- [ ] Full system test (all features)

---

## Troubleshooting Installation Issues

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed solutions to common problems during installation.

Common quick fixes:

| Problem | Solution |
|---------|----------|
| "Board not found" | Select `Arduino UNO` in Tools → Board |
| USB driver error | Install CH340 drivers (see Step 3 above) |
| Compilation error | Check all source files in `src/` folder |
| Upload fails | Check COM port, reset Arduino, try again |
| No serial output | Check baud rate (must be 115200) |

