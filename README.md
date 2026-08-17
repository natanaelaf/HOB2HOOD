# 🏠 IE60P Smart Range Hood Controller

> An intelligent embedded system for smart range hood control with Hob2Hood integration and IR remote support.

[![Arduino](https://img.shields.io/badge/Platform-Arduino%20UNO-green.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/Build-PlatformIO-orange.svg)](https://platformio.org/)
[![Embedded Systems](https://img.shields.io/badge/Category-Embedded%20Systems-red.svg)](#)

## 📋 Table of Contents

- [Overview](#-overview)
- [Key Features](#key-features)
- [Quick Start](#-quick-start)
- [System Modes](#-system-modes)
- [Documentation](#-documentation)
- [System Architecture](#-system-architecture)
- [Building & Contributing](#-building--contributing)
- [Requirements](#-requirements)
- [License](#-license)
- [Support](#-support--contact)

---

## 🎯 Overview

The **IE60P Smart Range Hood Controller** is an advanced embedded system designed to provide intelligent control over range hood (coifa) units. Built on the Arduino UNO microcontroller platform, it supports both manual control via physical buttons and automatic operation via IR remote commands compatible with Hob2Hood-enabled cooktops.

### ✨ Key Features

- **Dual Operation Modes**: Automatic (Hob2Hood) and Manual modes
- **IR Remote Support**: Decodes Hob2Hood/JVC-like IR protocols
- **Multi-Speed Control**: Three-speed motor control (V1, V2, V3) plus OFF
- **Smart Lighting**: Integrated LED control with adaptive blinking patterns
- **Automatic Exhaust**: Post-exhaust functionality to clear residual steam/odors
- **Programmable Timer**: Manual timer with speed selection
- **Persistent Settings**: EEPROM-backed configuration storage
- **Safety Features**: Hardware watchdog protection against system lockups
- **Comprehensive Logging**: Debug and diagnostic logging via serial interface

### 📊 Specifications

| Specification | Details |
|---|---|
| **Platform** | Arduino UNO (ATmega328P) |
| **Framework** | Arduino IDE compatible |
| **Baud Rate** | 115200 bps (Serial communication) |
| **Power** | 5V/3.3V I/O compatible |
| **IR Receiver** | 38 kHz protocol (Hob2Hood/JVC-like) |
| **Relay Control** | Low-active (RELAY_ON = LOW) |
| **Flash Memory** | ~28KB available for code |
| **RAM** | ~2KB available for runtime state |

---

## 🚀 Quick Start

### 📋 Prerequisites

- Arduino IDE or PlatformIO
- Arduino UNO board
- USB Type-B cable for programming
- Basic soldering skills (for relay/button connections)

### 📥 Installation

1. **Clone or download the project:**
   ```bash
   git clone https://github.com/yourusername/IE60P.git
   cd IE60P
   ```

2. **Open with PlatformIO:**
   ```bash
   platformio run --target upload
   ```

   **Or with Arduino IDE:**
   - Open `src/main.cpp`
   - Select Board: Arduino UNO
   - Select Port: COM port where UNO is connected
   - Click Upload

3. **Monitor serial output (115200 baud):**
   ```bash
   platformio device monitor
   ```

### ⚡ First Run

On first power-up, the system:
1. Initializes all modules
2. Loads settings from EEPROM (or sets defaults if uninitialized)
3. Sets the OFF button LED to indicate current mode (Manual = ON, Auto = OFF)
4. Begins accepting IR and button commands

---

## 🎛️ System Modes

### 🤖 Automatic Mode (Hob2Hood)
- Controlled by compatible cooktop via IR commands
- Responds to speed and power commands
- Features automatic post-exhaust after cooktop turns off
- Can be overridden by manual button presses

### 🖱️ Manual Mode
- Controlled by physical buttons on the unit
- Hob2Hood commands are ignored
- Full control via 5 physical buttons
- Mode toggle via long-press OFF button (3 seconds)

---

## 📚 Documentation

This documentation is organized as follows:

| Document | Purpose |
|----------|---------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | System design, module relationships, data flow |
| [MODULES.md](MODULES.md) | Detailed documentation of each module |
| [API_REFERENCE.md](API_REFERENCE.md) | Complete API reference for all classes |
| [HARDWARE_SETUP.md](HARDWARE_SETUP.md) | Wiring diagrams and pin configuration |
| [INSTALLATION.md](INSTALLATION.md) | Step-by-step setup and compilation guide |
| [USAGE_GUIDE.md](USAGE_GUIDE.md) | User manual and operation instructions |
| [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Common issues and solutions |

---

## 🏗️ System Architecture

The IE60P is built on a modular architecture with a central state machine that coordinates all subsystems:

```
┌─────────────────────────────────────────┐
│      StateMachine (Central Coordinator) │
└────────────┬────────────────────────────┘
             │
    ┌────────┼────────┬──────────┬─────────────┬──────────┐
    │        │        │          │             │          │
    ▼        ▼        ▼          ▼             ▼          ▼
┌────────┐ ┌──────┐ ┌─────┐  ┌──────┐   ┌─────────┐  ┌─────────┐
│Buttons │ │Motor │ │Light│  │Hob2H │   │Timer    │  │Watchdog │
└────────┘ └──────┘ └─────┘  │oodCmd│   │(PostEx) │  └─────────┘
    │        │        │       └──────┘   └─────────┘
    │        │        │          │            │
    └────────┼────────┼──────────┼────────────┘
             │        │          │
    ┌────────┼────────┼──────────┴──────┐
    │        │        │                 │
    ▼        ▼        ▼                 ▼
  ┌──────┐ ┌─────┐ ┌──────┐      ┌─────────────┐
  │Logger│ │ IR  │ │EEPROM│      │Settings     │
  └──────┘ └─────┘ └──────┘      └─────────────┘
```

### 🔑 Key Concepts

### 🔑 Key Concepts

- **Debouncing**: All button inputs are software-debounced with a 30ms delay to eliminate electrical noise.
- **Mode Switching**: Pressing the OFF button for 3+ seconds toggles between Manual and Automatic modes. The LED OFF blinks 3 times to confirm the transition.
- **Post-Exhaust**: When in Automatic mode and the Hob2Hood sends an OFF command, the motor switches to V1 (low speed) for a configurable duration (default 5 minutes) before fully shutting down.
- **Hob2Hood Protocol**: Uses JVC-like IR protocol with 38 kHz carrier frequency, 526 µs time units, and hash-based command validation.

---

## 🔨 Building & Contributing

### 📦 Build Requirements
- PlatformIO CLI or VS Code PlatformIO extension
- Arduino Board Package (ATmega AVR)

### 🛠️ Build Commands
```bash
# Build
platformio run

# Upload to board
platformio run --target upload

# Monitor serial output
platformio device monitor

# Clean build artifacts
platformio run --target clean
```

### 📁 Project Structure
```
IE60P/
├── platformio.ini       # Build configuration
├── src/
│   ├── main.cpp         # Entry point
│   ├── config.h         # Pin & timing constants
│   ├── StateMachine.*   # Central coordinator
│   ├── Buttons.*        # Button input handler
│   ├── Motor.*          # Motor/relay control
│   ├── Light.*          # LED control
│   ├── Hob2Hood.*       # IR command decoder
│   ├── Timer.*          # Timer management
│   ├── Logger.*         # Serial logging
│   ├── IR.*             # IR receiver handler
│   ├── EEPROMModule.*   # Persistent storage
│   ├── Settings.*       # Settings cache
│   └── Watchdog.*       # Watchdog timer
└── docs/
    ├── README.md
    ├── ARCHITECTURE.md
    ├── MODULES.md
    ├── API_REFERENCE.md
    └── ...
```

---

## 🖥️ Requirements

| Category | Details |
|----------|---------|
| **Operating System** | Windows, macOS, Linux |
| **Development Environment** | Arduino IDE 1.8.x+ or PlatformIO |
| **Compiler** | AVR GCC |
| **Hardware** | Arduino UNO (ATmega328P) |
| **USB Driver** | CH340 or FTDI (depending on board) |
| **Python** | 3.6+ (for PlatformIO) |

---

## 📝 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

You are free to:
- ✅ Use this project for personal or commercial purposes
- ✅ Modify and distribute the code
- ✅ Include in your own projects

Just remember to:
- ⚠️ Include a copy of the license and attribution
- ⚠️ Describe any changes you made

---

## 💬 Support & Contact

For issues, questions, or contributions:

- **🐛 Report Bugs**: [GitHub Issues](../../issues)
- **💡 Feature Requests**: [GitHub Discussions](../../discussions)
- **📧 Email**: [Your contact email]
- **📖 Help**: See the [TROUBLESHOOTING.md](TROUBLESHOOTING.md) guide
- **🚀 Contributing**: See [Contributing Guidelines](#contributing)

### 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

For major changes, please open an issue first to discuss the proposed changes.

---

## 📜 Changelog

### Version 1.0 (Initial Release)
- ✨ Full Hob2Hood IR protocol support
- 🎯 Dual-mode operation (Automatic/Manual)
- 🖱️ Physical button interface with debouncing
- 💾 EEPROM-backed settings persistence
- 🛡️ Watchdog safety protection
- 📊 Comprehensive logging system
- ⚙️ Motor speed control with interlock protection
- 💡 Adaptive LED blinking for timer feedback

---

## 📚 Glossary

| Term | Definition |
|------|-----------|
| **Hob2Hood** | Smart cooktop-to-ventilation integration system |
| **JVC Protocol** | IR protocol variant used by Hob2Hood |
| **Debounce** | Technique to eliminate electrical noise from button presses |
| **Post-Exhaust** | Automatic low-speed ventilation after cooking |
| **Watchdog** | Hardware protection against system lockups |
| **EEPROM** | Non-volatile memory for persistent settings storage |

---

