# User Guide

Complete user manual for operating the IE60P Smart Range Hood Controller.

## Table of Contents

1. [Safety & Warnings](#safety--warnings)
2. [Overview](#overview)
3. [Operating Modes](#operating-modes)
4. [Control Methods](#control-methods)
5. [Feature Description](#feature-description)
6. [LED Indicators](#led-indicators)
7. [Timer Programming](#timer-programming)
8. [Settings & Configuration](#settings--configuration)
9. [Tips & Best Practices](#tips--best-practices)
10. [Frequently Asked Questions](#frequently-asked-questions)

---

## Safety & Warnings

### ⚠️ SAFETY WARNINGS

**ELECTRICAL HAZARD**
- System operates at 5V logic level but controls high-voltage AC/DC loads
- Ensure all AC wiring is properly insulated and grounded
- Do NOT touch relay contacts while powered
- Keep away from moisture and water
- Never operate in wet environments

**MOTOR HAZARD**
- Range hood motor runs at high speed
- Never insert objects into ventilation path
- Keep away from loose hair and clothing
- Ensure proper ventilation while in operation
- Motor may continue running after OFF command (post-exhaust mode)

**POWER HANDLING**
- Use only 5V regulated power supply
- Do NOT exceed 5V (risk of component damage)
- Do NOT reverse polarity (destruction risk)
- Ensure power supply has adequate current capacity (2A minimum)

**MAINTENANCE**
- Unplug power before maintenance
- Keep IR receiver lens clean (gentle cloth only)
- Do NOT attempt to repair during warranty period
- Contact manufacturer for service

---

## Overview

### System Purpose

The IE60P Smart Range Hood Controller is an intelligent ventilation management system that:

1. **Automates** kitchen ventilation based on cooktop activity (Hob2Hood mode)
2. **Provides** manual control via physical buttons or IR remote
3. **Manages** lighting and timer functionality
4. **Protects** against common hazards with safety features
5. **Learns** user preferences (saves settings in memory)

### Components

```
┌─────────────────────────────────────────────────────┐
│          IE60P Range Hood Controller                │
│  ┌───────────────────────────────────────────────┐  │
│  │  Indicator LEDs                               │  │
│  │  [●] Mode    [●] V1    [●] V2    [●] V3      │  │
│  │  [●] Light                                    │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │  Control Buttons                              │  │
│  │   [OFF]     [V1]  [V2]  [V3]     [LIGHT]     │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │  IR Receiver (front-facing)                   │  │
│  │        █ █ █                                  │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### Quick Start (30 seconds)

1. **Power On**: Connect 5V power supply
2. **Select Speed**: Press V1, V2, or V3 button
3. **Turn Off**: Press OFF button
4. **Control Light**: Press LIGHT button to toggle light on/off

---

## Operating Modes

### Mode 1: Automatic (Hob2Hood) Mode

**Activation:**
- System default on first use
- Mode LED (OFF button) is **OFF** (not lit)

**Behavior:**
- System automatically responds to compatible cooktop (Hob2Hood, Bosch ConnectKit, etc.)
- When cooktop turns on: Range hood automatically starts and matches cooktop speed
- When cooktop turns off: Range hood runs 5 minutes at low speed (post-exhaust) then stops
- Manual buttons override automatic control

**Advantages:**
- ✓ Fully automatic operation
- ✓ No manual intervention needed
- ✓ Coordinates with cooktop activity
- ✓ Automatic post-exhaust removes odors

**Typical Usage:**
- Set it and forget it
- Manual buttons work anytime to override

### Mode 2: Manual Mode

**Activation:**
- Press and hold OFF button for 3 seconds
- System LED (OFF button) will **blink 3 times** to confirm
- Mode LED now stays **ON** (lit continuously)

**Behavior:**
- All Hob2Hood commands are ignored
- Full manual control via physical buttons
- Post-exhaust disabled
- Perfect for situations where Hob2Hood is unavailable

**Advantages:**
- ✓ Complete manual control
- ✓ Independent from cooktop
- ✓ Immediate response to buttons
- ✓ Useful when IR receiver has obstructions

**Typical Usage:**
- Gas cooktops without Hob2Hood capability
- When you prefer manual control
- Troubleshooting cooktop communication issues

### Mode Switching

```
OFF button press duration:
    < 3 seconds     →  Turn off hood (short press)
    > 3 seconds     →  Switch mode (long press, mode LED blinks)

Button hold duration shown by progress:
    0s  ──┬─────────────────┬──  3s
    ·   OFF button held   ·
    (nothing)            (mode switches)
```

---

## Control Methods

### Method 1: Physical Buttons

Five buttons control all functions:

```
┌────────────────────────────┐
│     [OFF]    Main Control  │
│     (Top Center)           │
│                            │
│  [V1] [V2] [V3]  Speed     │
│  (Left to Right)           │
│                            │
│    [LIGHT]    Light/Timer  │
│    (Bottom)                │
└────────────────────────────┘
```

**Button Functions:**

| Button | Single Press | Hold (3+ sec) |
|--------|--------------|---------------|
| **OFF** | Stop motor & timers | Switch mode (Auto/Manual) |
| **V1** | Speed Level 1 (low) | - |
| **V2** | Speed Level 2 (medium) | - |
| **V3** | Speed Level 3 (high) | - |
| **LIGHT** | Toggle light on/off | Enter timer programming |

### Method 2: IR Remote (Hob2Hood Compatible)

Use any Hob2Hood-compatible remote control:

**Supported Commands:**
- **ON**: Starts at low speed (V1)
- **OFF**: Stops, triggers post-exhaust (5 min)
- **V1, V2, V3, V4**: Direct speed selection
- **Speed Up/Down**: Adjusts speed smoothly

**Range & Requirements:**
- Range: 3-5 meters (10-16 feet typical)
- Line of sight required
- Avoid direct sunlight on receiver
- Works only in Automatic mode

**Compatibility:**
- ✓ Hob2Hood protocol
- ✓ Bosch ConnectKit remote
- ✓ Most modern cooktop remotes
- ✗ Samsung, GE proprietary remotes (check compatibility)

---

## Feature Description

### Speed Control

Three speed levels for motor operation:

| Speed | Symbol | Use Case | Motor Sound |
|-------|--------|----------|------------|
| **OFF** | ◯ | No ventilation | Silent |
| **V1** | ▬ | Light cooking (simmering) | Low hum |
| **V2** | ▬▬ | Normal cooking (boiling) | Medium noise |
| **V3** | ▬▬▬ | Heavy cooking (high heat/frying) | Loud |
| **Boost** | (automatic) | Post-exhaust low speed | Very low |

**Quick Speed Selection:**
1. Press desired button (V1, V2, or V3)
2. Motor starts immediately
3. Speed LED illuminates
4. Motor runs until OFF button pressed

### Light Control

**Switching Light On/Off:**
1. Press LIGHT button (single click)
2. Light relay energizes/de-energizes
3. LIGHT LED indicates state:
   - **LED ON**: Light is on
   - **LED OFF**: Light is off

**Light During Timer:**
- Timer runs after light OFF
- LIGHT LED blinks pattern:
  - Slow (0.5s): Timer > 5 minutes remaining
  - Fast (0.3s): Timer < 5 minutes remaining
  - Very fast (0.1s): Timer < 1 minute remaining

### Post-Exhaust Feature (Automatic Mode Only)

**What Happens:**
1. Cooktop sends OFF command via Hob2Hood
2. System switches motor to V1 (low speed)
3. Timer counts down 5 minutes (default)
4. After 5 minutes: Motor stops completely

**Purpose:**
- Removes cooking odors and steam
- Protects against moisture accumulation
- Automatic process (no intervention needed)

**Override:**
- Press OFF button: Stops post-exhaust immediately
- Press any speed button: Cancels and restarts motor

### Manual Timer Feature

**Activation:**
1. Hold LIGHT button for 3 seconds
2. System enters "Timer Programming Mode"
3. LIGHT LED starts blinking
4. Select timer duration:
   - **Press V1**: 1 minute
   - **Press V2**: 2 minutes
   - **Press V3**: 3 minutes
   - **Press LIGHT**: 5 minutes (default)
   - **Press OFF**: Cancel

**Active Timer Behavior:**
- Motor runs at selected speed
- LIGHT LED blinks (pattern changes as time elapses)
- Hob2Hood commands ignored (manual timer has priority)
- Timer counts down automatically
- When timer expires: Motor stops, LED stops blinking

**Timer Display:**
Serial Monitor shows remaining time (for debugging):
```
[INFO] Timer: 5 minutes remaining
[INFO] Timer: 4 minutes remaining
...
[INFO] Timer: 0 minutes remaining - stopping motor
```

---

## LED Indicators

### LED Status Map

Five LEDs provide real-time feedback:

```
┌──────────────────────────────────┐
│  LED OFF    (Red)                │
│  Status: Indicates current MODE  │
│  ✓ ON   = Manual Mode            │
│  ✗ OFF  = Automatic Mode         │
│  ⊗ BLINK= Mode switching         │
│                                  │
│  LED V1/V2/V3  (Green)           │
│  Status: Indicates SPEED         │
│  ✓ ON   = Speed active           │
│  ✗ OFF  = Speed inactive         │
│  (Only one LED ON at a time)     │
│                                  │
│  LED LIGHT  (Yellow)             │
│  Status: Indicates LIGHT & TIMER │
│  ✓ ON   = Light relay on         │
│  ◐ BLINK= Timer active           │
│  ✗ OFF  = Light off, no timer    │
│                                  │
│  Blink Patterns:                 │
│  Slow    (500ms) = Timer >5 min  │
│  Fast    (300ms) = Timer <5 min  │
│  Rapid   (100ms) = Timer <1 min  │
└──────────────────────────────────┘
```

### LED Reference Chart

| LED | State | Meaning | Action |
|-----|-------|---------|--------|
| OFF | ON | Manual mode active | Press to switch modes |
| OFF | OFF | Auto mode active | IR commands accepted |
| OFF | Blinking | Mode switch in progress | Wait 1-2 seconds |
| V1/V2/V3 | ON | Speed active | Press OFF to stop |
| V1/V2/V3 | OFF | Speed inactive | Press button to activate |
| LIGHT | ON | Light relay energized | Press to toggle off |
| LIGHT | OFF | Light relay de-energized | Press to toggle on |
| LIGHT | Slow blink | Long timer remaining | Timer will expire soon |
| LIGHT | Fast blink | 5 minutes remaining | Timer nearly expired |
| LIGHT | Rapid blink | <1 minute remaining | Timer expiring soon |

---

## Timer Programming

### Step-by-Step Timer Setup

**Scenario: You want 10 minutes of ventilation**

```
1. Hold LIGHT button (3 seconds)
   └─> LIGHT LED starts blinking
   └─> Serial: "Timer: Modo de programacao ativado..."

2. Press V3 button (3 minutes)
   └─> Motor runs at speed 3
   └─> Timer counts: 3 min ⏱

3. [Timer runs for 3 minutes]
   └─> LIGHT LED blinks FAST (< 5 min)

4. [Timer expires]
   └─> Motor stops
   └─> LIGHT LED stops blinking
```

### Quick Timer Shortcuts

**Common Durations:**

| Goal | Steps | Duration |
|------|-------|----------|
| 1 minute vent | V1 | 1 min |
| 2 minutes vent | V2 | 2 min |
| 3 minutes vent | V3 | 3 min |
| 5 minutes vent | LIGHT | 5 min (default) |
| Custom time | Multiple timers | Combine V1+V2+V3 |

**Combine Multiple Timers:**
```
Example: You want 8 minutes total

1. Timer #1: Press V3 (3 min)
   └─> Runs for 3 minutes
2. When expired, immediately:
3. Timer #2: Hold LIGHT (enter programming)
4. Press V3 again (3 min)
   └─> Runs for another 3 minutes
5. When expired:
6. Timer #3: Hold LIGHT
7. Press V2 (2 min)
   └─> Runs for final 2 minutes
Total: 3 + 3 + 2 = 8 minutes
```

### Canceling Timer

**While Timer Running:**
1. Press OFF button (any time)
2. Motor stops immediately
3. LIGHT LED stops blinking

**After Timer Expires:**
- Automatic shutoff (nothing to cancel)

---

## Settings & Configuration

### Default Settings

System comes with factory defaults:

| Setting | Value | Description |
|---------|-------|-------------|
| Mode | Automatic | Hob2Hood enabled |
| Light | OFF | Light relay de-energized |
| Timer Duration | 5 min | Default for manual timer |
| Post-Exhaust | 5 min | Duration after Hob2Hood OFF |

### Changing Settings

#### Via Physical Control

**Change Operating Mode:**
1. Press OFF button for 3+ seconds
2. Mode LED blinks 3 times
3. Mode switched successfully

**Change Timer Defaults:**
- Default timer selectable via V1/V2/V3/LIGHT during timer setup
- Defaults are suggested, not mandatory

#### Via Serial Terminal (Advanced)

If connected to development PC:

1. Open Serial Terminal (115200 baud)
2. Observe log messages:
   ```
   [INFO] System initialized
   [INFO] Mode: AUTOMATIC
   [INFO] Timer default: 5 minutes
   [INFO] Post-Exhaust: 5 minutes
   ```

3. Send test commands:
   ```
   ! Set timer to 30 minutes
   [DEBUG] Timer set to 30 minutes
   [DEBUG] EEPROM write successful
   ```

#### Via EEPROM Programming (Advanced)

If you know Arduino: Edit `src/config.h` and recompile:

```cpp
// Adjust defaults
#define DEFAULT_TIMER_MIN     30    // Change from 5 to 30
#define DEFAULT_POST_EXH_MIN  10    // Change from 5 to 10
```

Then recompile and upload firmware.

### Saving Custom Settings

Settings automatically save to internal memory (EEPROM):

```
Power off and on again:
├─ System restarts
├─ Loads saved settings
└─ Resumes with last-known state

This happens automatically - no action needed!
```

**What Persists (Saved):**
- ✓ Operating mode (Auto/Manual)
- ✓ Light state (On/Off)
- ✓ Default timer duration
- ✓ Post-exhaust duration

**What Resets (Not Saved):**
- ✗ Active timer countdown
- ✗ Current speed (stops on power loss)
- ✗ Temporary state

---

## Tips & Best Practices

### For Best Performance

**1. Keep IR Receiver Clear**
- Ensure direct line of sight to remote
- Keep lens clean (dust reduces range)
- Avoid pointing remote at receiver through glass

**2. Use Manual Mode for Guests**
- Non-technical users may not know mode
- Manual mode prevents unexpected behavior
- Buttons are self-explanatory

**3. Optimize Timer for Your Cooking**
- Learn how long cooking typically takes
- Set timer accordingly
- Post-exhaust adds automatic 5 min

**4. Regular Maintenance**
- Clean filters monthly
- Wipe IR receiver lens weekly
- Check for error messages in serial

### Troubleshooting Common Issues

**Motor won't start:**
- Check if OFF mode (LED OFF OFF should blink or show state)
- Verify power supply connected
- Press V1 button to test

**Light won't turn on:**
- Try toggling LIGHT button twice
- Check LED indicator
- Verify power to relay module

**Timer programs but doesn't run:**
- Ensure you released LIGHT button after 3 seconds
- LED should start blinking
- Try pressing a speed button

**IR remote not responding:**
- Switch to Manual mode first
- Ensure IR receiver lens is clean
- Check remote batteries
- Try different speed levels

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for more issues.

---

## Frequently Asked Questions

### General Operation

**Q: What happens if I press multiple buttons at once?**
A: System processes one button at a time. If multiple pressed simultaneously, fastest debounce wins (typically first button).

**Q: Can I use this with non-Hob2Hood cooktops?**
A: Yes! Switch to Manual mode. Use physical buttons for full control without cooktop communication.

**Q: How long do timers last?**
A: Maximum 65,535 minutes (~45 years). Practical limit is 999 minutes (16 hours).

**Q: Does the light relay consume a lot of power?**
A: No. Relay is rated for 10A @ 250V AC. Typical light draws <100W.

### Modes & Features

**Q: What's the difference between Manual and Automatic modes?**
A: Automatic responds to Hob2Hood cooktop commands. Manual is controlled entirely by buttons.

**Q: Why does the motor keep running after I press OFF?**
A: Post-exhaust is running! It's 5 minutes of low-speed ventilation to clear odors. Press OFF again to stop immediately, or wait 5 minutes.

**Q: Can I stop post-exhaust early?**
A: Yes. Press OFF button to cancel immediately, or press any speed button to restart at that speed.

**Q: What's the "Timer Programming Mode"?**
A: Hold LIGHT 3 seconds to enter. Then press V1/V2/V3/LIGHT to set duration (1/2/3/5 minutes). Press OFF to cancel.

### Settings & Memory

**Q: Do my settings survive power loss?**
A: Yes! Operating mode and light state are saved in internal memory. They restore when power returns.

**Q: How do I reset to factory defaults?**
A: Power off, wait 10 seconds, power on. If EEPROM corrupted, use serial connection to reinitialize.

**Q: Can I change the default timer duration?**
A: Yes. During timer programming, any button sets that duration as default (saved). Next timer uses it.

### Technical Support

**Q: Where can I find detailed technical information?**
A: See [ARCHITECTURE.md](ARCHITECTURE.md) for system design and [MODULES.md](MODULES.md) for component details.

**Q: How do I access serial debug logs?**
A: Connect USB cable to computer, open Serial Monitor (115200 baud) in Arduino IDE.

**Q: What should I do if system doesn't respond?**
A: 1) Power cycle (off/on), 2) Check LED indicators, 3) See [TROUBLESHOOTING.md](TROUBLESHOOTING.md), 4) Contact support.

---

## Contact & Support

**Documentation:**
- Technical Details: [ARCHITECTURE.md](ARCHITECTURE.md)
- Hardware Setup: [HARDWARE_SETUP.md](HARDWARE_SETUP.md)
- Troubleshooting: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

**For Issues:**
1. Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md) first
2. Review relevant documentation section
3. Capture serial debug output (if possible)
4. Contact manufacturer with:
   - Description of issue
   - Serial output (if available)
   - LED indicator states
   - Steps to reproduce

