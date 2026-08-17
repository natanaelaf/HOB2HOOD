# Hardware Setup Guide

Complete hardware configuration guide for the IE60P Smart Range Hood Controller.

## Table of Contents

1. [Bill of Materials](#bill-of-materials)
2. [Microcontroller Specifications](#microcontroller-specifications)
3. [Pin Configuration](#pin-configuration)
4. [Wiring Diagram](#wiring-diagram)
5. [Relay Module Configuration](#relay-module-configuration)
6. [Button Interface](#button-interface)
7. [IR Receiver Setup](#ir-receiver-setup)
8. [LED Indicators](#led-indicators)
9. [Power Supply](#power-supply)
10. [Testing & Verification](#testing--verification)

---

## Bill of Materials

### Core Components

| Component | Quantity | Part Number (Example) | Notes |
|-----------|----------|----------------------|-------|
| Arduino UNO | 1 | A000066 | ATmega328P-based microcontroller |
| USB Type-B Cable | 1 | - | For programming and power (during development) |
| 5V Power Supply | 1 | - | For production deployment (2A minimum) |

### Input Components

| Component | Quantity | Notes |
|-----------|----------|-------|
| IR Receiver Module (38 kHz) | 1 | Pin configuration: GND, +5V, Signal |
| Push-button Switch | 5 | Momentary, SPST (Single-Pole Single-Throw) |
| Resistor 10 kΩ | 5 | For pull-down circuits (optional, if not using internal pull-ups) |

### Output Components

| Component | Quantity | Notes |
|-----------|----------|-------|
| Relay Module (5V) | 4 | Each relay controls one function |
| LED 5mm | 5 | Red or yellow indicators |
| Resistor 220 Ω | 5 | Current limiting for LEDs |

### Optional Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| Capacitor 100 µF | 1 | Power supply stabilization |
| Capacitor 10 µF | 1 | IR receiver stabilization |
| Resistor 1 kΩ | 3 | IR receiver pull-down (if needed) |

---

## Microcontroller Specifications

### Arduino UNO Board

```
Microcontroller:  ATmega328P
Operating Voltage: 5V
Input Voltage: 7-12V (recommended) or USB
Digital I/O Pins: 14 (6 with PWM)
Analog Input Pins: 6
DC Current per I/O Pin: 40 mA (recommended 20 mA)
Flash Memory: 32 KB (bootloader uses 2 KB)
SRAM: 2 KB
EEPROM: 1 KB
Clock Speed: 16 MHz
```

### Pin Usage Summary

**Total Pins Used:** 13 out of 20 (including Vcc/GND)

| Category | Count | Pins |
|----------|-------|------|
| Input (Buttons) | 5 | Digital 3-7 |
| Input (IR) | 1 | Digital 2 (INT0) |
| Output (Relays) | 3 | Digital 10-12 |
| Output (LEDs) | 5 | Digital 8-9, Analog 0-2 |
| Power (5V) | 1 | Vcc |
| Ground | 1 | GND |

---

## Pin Configuration

### Digital Pins (0-13)

| Pin | Function | Mode | Logic | Connection |
|-----|----------|------|-------|------------|
| 0 | Serial RX | INPUT | - | USB/Debug |
| 1 | Serial TX | OUTPUT | - | USB/Debug |
| 2 | IR Receiver | INPUT | - | 38 kHz IR module |
| 3 | Button OFF | INPUT_PULLUP | LOW=pressed | Button to GND |
| 4 | Button V1 | INPUT_PULLUP | LOW=pressed | Button to GND |
| 5 | Button V2 | INPUT_PULLUP | LOW=pressed | Button to GND |
| 6 | Button V3 | INPUT_PULLUP | LOW=pressed | Button to GND |
| 7 | Button Light | INPUT_PULLUP | LOW=pressed | Button to GND |
| 8 | LED OFF | OUTPUT | HIGH=on | LED anode via 220Ω |
| 9 | LED V1 | OUTPUT | HIGH=on | LED anode via 220Ω |
| 10 | Relay V1 | OUTPUT | LOW=energized | Relay coil |
| 11 | Relay V2 | OUTPUT | LOW=energized | Relay coil |
| 12 | Relay V3 | OUTPUT | LOW=energized | Relay coil |
| 13 | Relay Light | OUTPUT | LOW=energized | Relay coil |

### Analog Pins (A0-A5)

When used as digital, analog pins can serve as GPIO:

| Pin | Function | Mode | Logic | Connection |
|-----|----------|------|-------|------------|
| A0 | LED V2 | OUTPUT | HIGH=on | LED anode via 220Ω |
| A1 | LED V3 | OUTPUT | HIGH=on | LED anode via 220Ω |
| A2 | LED Light | OUTPUT | HIGH=on | LED anode via 220Ω |
| A3-A5 | Unused | - | - | Available for expansion |

---

## Wiring Diagram

### Overall System Diagram

```
┌─────────────────────────────────────────────────────────┐
│                   Arduino UNO                            │
│  ┌──────────────────────────────────────────────────┐   │
│  │ Digital Pins:  0-13                             │   │
│  │ Analog Pins:   A0-A5 (used as digital)         │   │
│  │ Power:         5V, 3.3V, GND                   │   │
│  └──────────────────────────────────────────────────┘   │
│                                                          │
├──────────────────────────────────────────────────────────┤
│                    Inputs                                │
│  Pin 2  ────────> IR Receiver (38 kHz)                  │
│  Pin 3  ────────> Button OFF                            │
│  Pin 4  ────────> Button V1                             │
│  Pin 5  ────────> Button V2                             │
│  Pin 6  ────────> Button V3                             │
│  Pin 7  ────────> Button Light                          │
│                                                          │
├──────────────────────────────────────────────────────────┤
│                    Outputs                               │
│  Pin 8  ────────> LED OFF (220Ω resistor)               │
│  Pin 9  ────────> LED V1 (220Ω resistor)                │
│  Pin A0 ────────> LED V2 (220Ω resistor)                │
│  Pin A1 ────────> LED V3 (220Ω resistor)                │
│  Pin A2 ────────> LED Light (220Ω resistor)             │
│  Pin 10 ────────> Relay V1                              │
│  Pin 11 ────────> Relay V2                              │
│  Pin 12 ────────> Relay V3                              │
│  Pin 13 ────────> Relay Light                           │
│                                                          │
└─────────────────────────────────────────────────────────┘
         │              │              │
         ▼              ▼              ▼
    [5V Supply]   [GND Rail]    [Load Circuits]
```

### Button Wiring (Typical)

```
    5V (through pull-up resistor on UNO)
     │
    ┌┴┐
    │ │ 10kΩ (internal to Arduino)
    │ │
     └┬────────────────┬──────── Pin 3/4/5/6/7
       │               │
      [ ]  ← Button    │
       │               │
    [===] ← Switch    │
       │               │
      GND ─────────────┘

When button pressed: Pin reads LOW (0V)
When button released: Pin reads HIGH (5V via pull-up)
```

### LED Wiring

```
    Pin (HIGH output)
       │
       ├──────┐
       │      │
      ⛔  ← LED anode (longer leg)
      │ │
      │ │ 220Ω
      │ │  resistor
       │ │
      LED cathode (shorter leg)
       │
      GND
      
When Pin=HIGH: LED on (current flows through resistor)
When Pin=LOW: LED off (no current)
```

### Relay Module Wiring

```
Relay Module (4-Channel 5V):
┌─────────────────────────────┐
│  Relay Module               │
│  ┌─────────────────────┐    │
│  │   Signal Inputs     │    │
│  │  Pin 1  ◄───────────┼────┼─ Arduino Pin 10
│  │  Pin 2  ◄───────────┼────┼─ Arduino Pin 11
│  │  Pin 3  ◄───────────┼────┼─ Arduino Pin 12
│  │  Pin 4  ◄───────────┼────┼─ Arduino Pin 13
│  │  GND    ◄───────────┼────┼─ Arduino GND
│  │  VCC    ◄───────────┼────┼─ 5V Power
│  └─────────────────────┘    │
│                             │
│  ┌─────────────────────┐    │
│  │  Relay Contacts     │    │
│  │  Relay 1 (V1)       │    │
│  │    COM ─────────────┼──┐ │
│  │    NO  ─────────────┼──┼─┼─ To Motor Speed V1
│  │    NC  ─────────────┼──┘ │
│  └─────────────────────┘    │
│  [Similar for relays 2-4]   │
└─────────────────────────────┘

Logic:
  Pin=LOW (Arduino output 0V)  → Relay coil energized → NO contact closes
  Pin=HIGH (Arduino output 5V) → Relay coil off → NO contact opens
```

### IR Receiver Module Wiring

```
IR Receiver Module (38 kHz):
┌──────────────┐
│ (Black Dome) │  ← Receiver lens
│              │
│   GND  ◄─────┼───── Arduino GND
│   +5V  ◄─────┼───── 5V Power
│   OUT  ◄─────┼───── Arduino Pin 2 (INT0)
└──────────────┘

With Optional Stabilization Capacitor:
    5V Power
      │
      ├─────┬──────────────┐
      │     │              │
      │    ⊡ 100µF Cap    │
      │     │              │
      └─────┼──────────────┘
            │
       ┌────┴────┐
       │   +5V   │
       │   GND   │
       │   OUT   │ IR Module
       └─────────┘
             │
          Pin 2
```

---

## Relay Module Configuration

### 4-Channel Relay Module (JQC-3FF-5V Standard)

**Specifications:**
- Voltage: 5V coil
- Current: 70 mA per relay
- Contact Rating: 250V AC / 30V DC (10A typical)
- Type: SPDT (Single Pole Double Throw)

**Pin Configuration:**

```
Input Side (Signal from Arduino):
  Pin 1  ← Signal for Relay 1
  Pin 2  ← Signal for Relay 2
  Pin 3  ← Signal for Relay 3
  Pin 4  ← Signal for Relay 4
  GND    ← Common ground
  VCC    ← 5V power supply

Relay Contacts (Load Side):
  Each relay has 3 pins:
  COM (Common)
  NO (Normally Open)
  NC (Normally Closed)
```

**Control Logic:**
- **Active-Low**: Pin=LOW (0V) → Relay ON, Pin=HIGH (5V) → Relay OFF
- This matches Arduino GPIO behavior (default)

**Safety Considerations:**
- Never switch AC mains voltage directly
- Use relay contacts to control AC/DC motor load
- Ensure adequate current capacity (relay typically 10A max)
- Add flyback diode if controlling inductive loads

---

## Button Interface

### Push-Button Specifications

- Type: Momentary SPST (Single-Pole Single-Throw)
- Rating: 12V 100mA (typical)
- Contact Resistance: <50 mΩ
- Bounce Time: <10 ms (hardware bounce)

### Button Debouncing

Arduino-side debouncing (software):
- 30 ms debounce window
- Requires 2 consecutive stable reads
- Rejects spurious noise

### Button Placement

```
Off Button:     Main shutdown/mode toggle
V1 Button:      Speed Level 1 (low)
V2 Button:      Speed Level 2 (medium)
V3 Button:      Speed Level 3 (high)
Light Button:   Light toggle / Timer programming

Physical Layout (suggested):
┌─────────────────────────────┐
│                             │
│         [ OFF ]             │
│                             │
│  [ V1 ]  [ V2 ]  [ V3 ]     │
│                             │
│        [ LIGHT ]            │
│                             │
└─────────────────────────────┘
```

---

## IR Receiver Setup

### Hob2Hood Protocol Specifications

- **Carrier Frequency**: 38 kHz (26.3 µs period)
- **Modulation**: Pulse width (PWM)
- **Frame Length**: ~50-70 ms
- **Reception Range**: 3-5 meters typical
- **Angle**: 30° wide acceptance

### IR Receiver Module

Common module: **TSOP38238** or compatible

**Pin Configuration:**
```
View from front (with dome facing you):
   ┌─────────────┐
   │ (Black Dome)│
   └─┬─────────┬─┘
     │    │    │
    GND  +5V  OUT
    (1)  (2)  (3)
```

**Wiring to Arduino:**
```
Pin 1 (GND)  → Arduino GND
Pin 2 (+5V)  → 5V Power (via capacitor)
Pin 3 (OUT)  → Arduino Pin 2 (INT0 interrupt)
```

### Optimal Placement

- Mount on front panel
- Avoid direct sunlight
- Avoid fluorescent lamp interference
- Keep away from high-frequency noise sources

### Testing IR Reception

Connect USB serial monitor and observe logs:
```
[DEBUG] IR frame ready: 64 elements
[INFO] Hob2Hood: Command received: V1
```

If no frames detected:
1. Check receiver power (should be 5V)
2. Verify connection to Pin 2
3. Test with remote control at 1 meter distance
4. Check receiver lens for obstructions

---

## LED Indicators

### LED Specifications

- Type: 5mm through-hole
- Forward Voltage: 2.0V (red), 2.1V (yellow)
- Typical Current: 10-20 mA
- Luminosity: 30-50 mcd (typical)

### LED Circuit

```
Current Limiting Resistor Calculation:
R = (Vcc - Vf) / I_led
R = (5V - 2.0V) / 0.015A = 200Ω

Use 220Ω standard resistor (nearest value)
```

### LED Functions

| LED | GPIO | Color | Function |
|-----|------|-------|----------|
| OFF | Pin 8 | Red | Mode indicator (ON = Manual, OFF = Auto) |
| V1 | Pin 9 | Green | Speed 1 active |
| V2 | A0 | Green | Speed 2 active |
| V3 | A1 | Green | Speed 3 active |
| Light | A2 | Yellow | Light relay active / Timer feedback |

### LED Status States

**Manual Mode:**
- LED OFF = ON (continuous)
- LED V1/V2/V3 = reflects active speed
- LED Light = blinking pattern for timer

**Automatic Mode:**
- LED OFF = OFF (continuous)
- LED V1/V2/V3 = reflects active speed
- LED Light = blinking pattern for timer

---

## Power Supply

### Requirements

**Development (USB Power):**
- Voltage: 5V (USB-supplied)
- Current: <500 mA (USB limit)
- Note: Sufficient for testing without relays active

**Production (External Power):**
- Voltage: 5V regulated
- Current: 2A minimum (continuous)
- Type: DC regulated supply
- Connector: Barrel jack (2.1 mm ID, 5.5 mm OD) or soldered leads

### Current Budget

```
Microcontroller (at rest):       30 mA
IR Receiver (receiving):         10 mA
Serial Debug (if enabled):       5 mA
All LEDs (all on):              50 mA
Relay Module (1 relay active):   70 mA
Relay Module (4 relays active): 280 mA
────────────────────────────────────
Typical Peak (all active):      445 mA
Typical Idle (no relays):       95 mA
```

### Power Supply Recommendation

- **Brand**: Meanwell, TDK-Lambda, or equivalent
- **Rating**: 5V @ 3A (provides margin)
- **Protection**: Over-current, thermal, over-voltage
- **Ripple**: <100 mV (good quality)

### Power Connection

```
5V Supply Positive
     │
     ├─────── Relay Module VCC
     ├─────── Arduino VIN (via diode)
     └─────── 100µF Capacitor (positive terminal)

GND
     │
     ├─────── Relay Module GND
     ├─────── Arduino GND
     └─────── 100µF Capacitor (negative terminal)
```

### Stabilization Capacitor

Add 100µF electrolytic capacitor across 5V and GND:
- Reduces power supply ripple
- Absorbs current spikes from relay switching
- Protects against brownout conditions

---

## Testing & Verification

### Pre-Power-Up Checklist

Before connecting power:

- [ ] All GPIO pins connected correctly (check schematic)
- [ ] No short circuits between 5V and GND
- [ ] All relay contacts not shorted
- [ ] LED resistors properly rated (220Ω)
- [ ] IR receiver lens clean
- [ ] Power supply voltage set to 5V

### Power-Up Test

1. Connect USB for programming
2. Compile and upload firmware
3. Open Serial Monitor at 115200 baud
4. Check for initialization messages:
   ```
   [INFO] IE60P Smart Controller v1.0 - Inicializando...
   [INFO] Sistema Inicializado com Sucesso.
   ```

### Input Testing

**Button Testing:**
- Press each button 5 times
- Check Serial Monitor for events
- Expected output:
  ```
  [INFO] Botoes: Clique V1.
  [DEBUG] Button debounce timeout: 30ms
  ```

**IR Testing:**
- Point Hob2Hood remote at IR receiver
- Press buttons on remote
- Expected output:
  ```
  [DEBUG] IR frame ready: 48 elements
  [INFO] Hob2Hood: Processando comando 3
  ```

### Output Testing

**LED Testing:**
- Connect each LED in sequence
- Manually set GPIO HIGH/LOW from serial
- Check LED illuminates correctly

**Relay Testing:**
- Connect relay contacts to test load (LED + resistor)
- Command relay active via GPIO
- Check load turns on/off
- Listen for relay click sound

### Safety Testing

**Current Measurement:**
- Measure current draw with all LEDs on
- Should be <50 mA
- Measure with one relay active
- Should be ~70 mA

**Thermal Check:**
- Run at full load for 10 minutes
- Check for heating at:
  - Relay module (should be cool)
  - Resistors (should be warm, not hot)
  - Microcontroller (should be room temp)

---

## Troubleshooting Reference

| Issue | Likely Cause | Solution |
|-------|--------------|----------|
| No serial output | USB driver missing | Install CH340 drivers |
| Buttons not detected | Pin shorts | Verify GPIO continuity |
| LEDs not lighting | Reverse polarity | Check anode/cathode |
| IR not working | Receiver power off | Check +5V to module |
| Relays not clicking | Low current | Check power supply capacity |
| Relay chatter | Excessive switching | Add stabilization capacitor |

