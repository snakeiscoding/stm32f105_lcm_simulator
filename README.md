# STM32F105 LCB Simulator

Firmware for a hardware simulator of the **LCB (Load Control Board)** used in Johnson Health Tech (JHT) exercise equipment. When connected to a UCB (Upper Control Board / exercise console) via RS-485, this device should behave and respond as a real LCB would — allowing console development, testing, and calibration without physical exercise equipment present.

---

## Hardware

| Item | Detail |
|---|---|
| MCU | STM32F105VC (ARM Cortex-M3, 48 MHz, 256 KB flash, 64 KB RAM) |
| EEPROM | Atmel 93C86 (SPI bit-bang) |
| ADC module | ADS1100 (16-bit, I2C) |
| Display | HD44780-compatible LCD (8-bit mode on V2 board, 4-bit on others) |
| Communication | RS-485/422 via USART2 |

---

## Building

Both IAR and GCC/CMake toolchains are supported.

### IAR Embedded Workbench

**Tools:** [IAR EWARM 9.20+](https://www.iar.com/)

1. Open `NewLCBSimulator.eww`.
2. Select the **Debug** configuration and build (`F7`).

Build output: `Debug/Exe/` (excluded from git). Linker script: `Link/JHT_stm32f105V1.2_IAR9.icf`.

### GCC + CMake + Ninja

**Tools:** [GCC ARM](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [SRecord](https://srecord.sourceforge.net/) (`winget install sourceforge.srecord`), [SEGGER J-Link](https://www.segger.com/downloads/jlink/), [VS Code](https://code.visualstudio.com/) (extensions: CMake Tools, C/C++, Cortex-Debug)

1. Add all tool locations to your `PATH`, including the J-Link installation directory (e.g. `C:\Program Files\SEGGER\JLink`).
2. Open the project folder in VS Code — CMake Tools will detect it automatically.
3. Press the build button. Outputs land in `build/`.

Alternatively, from the command line:
```
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
cmake --build build
```

**Do not change the `-O1` optimization level** — the firmware does not run correctly at other levels.

#### Build artifacts

| Artifact | Description |
|---|---|
| `build/white_simulator.elf/.bin/.hex` | Application only |
| `build/BootAPI/white_simulator_bootloader.elf/.bin/.hex` | Bootloader only |
| `build/white_simulator_with_bootloader.bin` | Combined image ready to flash |

The bootloader must be present on the device — it jumps to the application. Flash the combined binary for a full install.

#### Memory map

| Region | Address |
|---|---|
| Bootloader | `0x08000000` |
| Application | `0x0800D000` |

### Build configuration switches (`Inc/PinDefine.h`)

| Define | Effect |
|---|---|
| `_PRODUCT_BOARD` | Targets production PCB (vs. dev/KL board) |
| `_PRODUCT_V2` | V2 PCB pin layout; also enables Phoenix console and serial number commands |
| `_V3` | RIS protocol version 3 (current) |
| `_V3_Update` | Additional V3 protocol extensions |
| `_SupportPhoenixConsole` | Enabled automatically when `_PRODUCT_BOARD` + `_PRODUCT_V2` are both defined |

---

## Firmware Update

The `BootAPI/` module supports USB-based firmware updates:
1. Place the new firmware image on a USB mass storage device.
2. The bootloader (`Boot.c`) detects the file, erases flash, and reprograms via `Bootflash.c`.
3. Boot-time status is shown on the LCD via `BootLCM.c`.

---

## Architecture

### Software layers

```
┌─────────────────────────────────────────────────────┐
│  SimulatorMain.c  — top-level state machine          │
│  LCBSimulator.c   — mode management, status, EEPROM  │
├─────────────────────────────────────────────────────┤
│  JHTLCBComm.c / JHTLCBCommxxx.c — RS-485 protocol   │
├──────────────────────┬──────────────────────────────┤
│  Screen / LCMDisplay │  RowerWatts / PowerTable      │
│  LCM (LCD driver)    │  Sintable, machine.h          │
├──────────────────────┴──────────────────────────────┤
│  ADC  ADS1100  Encoder  EE93CXX  stm32f10x_it        │
├─────────────────────────────────────────────────────┤
│  CMSIS + STM32F10x StdPeriph Library + FAT/USB       │
└─────────────────────────────────────────────────────┘
```

**`SimulatorMain.c`** — Entry point and 1 ms SysTick handler. Drives the top-level mode state machine:

| Mode constant | Value | Meaning |
|---|---|---|
| `_RunMode` | 0 | Normal operation |
| `_ErPActionMode` | 1 | Energy-related protocol (ErP) action |
| `_SelectLCBType` | 2 | User is selecting machine type via encoder |
| `_RISBatteryOFF` | 3 | RIS battery-off state |

**`LCBSimulator.c`** — Core simulation. Owns `LCBParameter` (the master state struct `LCBSystemControlDataStatus`) and `RISLCBParameter`. Handles EEPROM reads/writes for machine type and serial numbers, battery/power state machines, and ErP logic.

**`JHTLCBComm.c` / `JHTLCBCommxxx.c`** — Interrupt-driven RS-485 engine. Parses incoming frames, dispatches to per-command handlers, and builds response frames. All 100+ command codes are in `Inc/JHTCOMMAND.H`. `JHTLCBCommxxx.c` extends this for machine types that require non-standard responses.

**`Screen.c` / `LCMDisplay.c` / `LCM.c`** — Layered display stack. `LCM.c` is a raw HD44780 driver. `LCMDisplay.c` formats data into strings. `Screen.c` provides machine-specific screen templates for 25+ parameter types (watts, RPM, SPM, incline %, current, voltage, NM, brake pressure, etc.).

**`RowerWatts.c`** — Rower physics engine. Processes generator pulses (20 edges/rotation from a 4-pole, 5-pole-pair alternator), detects Drive/Recovery stroke phases, calculates drag factor and power output. Physical constants (flywheel inertia = 60 517 kgf·mm², belt ratios, torque curves) live in `Inc/machine.h`.

### RS-485 Protocol

```
UCB → LCB:  [0x00][0xFF][Command][Length][Data...][CRC8]
LCB → UCB:  [0x01][Status bytes][Command][Length][Data...][CRC8]

Timeout:          150 ms
Rx/Tx switch:     5 ms delay
```

All command codes defined in `Inc/JHTCOMMAND.H`. Command categories: system init/status/version, motor control (RPM, PWM, watts), incline (position, calibration), serial numbers, and machine-specific commands.

---

## Hardware Pin Mapping (V2 production board)

| Peripheral | Signal | GPIO |
|---|---|---|
| RS-485 (USART2) | TxD | PA2 |
| RS-485 (USART2) | RxD | PA3 |
| RS-485 (USART2) | Direction ctrl | PA1 |
| Console power switch | 12V SW | PA5 |
| Encoder | A / B / Button | PB6 / PB7 / PB5 |
| Safety key (E-STOP) | Input | PB1 |
| EEPROM 93C86 | CS / SK / DI / DO | PD8 / PB15 / PB14 / PB13 |
| ADS1100 I2C ADC | SCL / SDA | PB10 / PB11 |
| LCD backlight | BL | PA8 |
| LCD data bus (D0–D7) | D0 | PC6 |
| LCD data bus (D0–D7) | D1–D3 | PD15–PD13 |
| LCD data bus (D0–D7) | D4–D7 | PD12–PD9 |
| 12 V analog input | ADC Ch4 | PA4 |

---

## EEPROM Layout (93C86)

| Address (decimal) | Contents |
|---|---|
| 50 (`_EEMachineType`) | Selected machine type code |
| 51 | Climbmill setting |
| 52–53 | Position min (16-bit) |
| 54–55 | Position max (16-bit) |
| 56 | Powered status |
| 100 | Serial number length |
| 101–350 | Serial number data (max 250 bytes) |

---

## Supported Machine Types

Machine type is selected at runtime via the rotary encoder, displayed on the LCD, and persisted to EEPROM address 50.

| Code | Constant | Equipment |
|---|---|---|
| 0x07 | `_LCB1_` | LCB1 Rower |
| 0x0A | `_LCB2_` | LCB2 Rower |
| 0x08 | `_LCB3_` | LCB3 |
| 0x14 | `_LCB1x_` | JIS 1x |
| 0x13 | `_LCBA_` | LCBA Bike |
| 0x18 | `_LCBA_M_` | LCBA-M Bike |
| 0x1B | `_Athena` | Athena Indoor Cycle |
| 0x1D | `_JISICR70LCB` | JIS Indoor Cycle 70 series |
| 0x0B | `_Climbmill_` | Climbmill |
| 0x17 | `_ClimbmillEN_` | Climbmill EN2017 |
| 0x19 | `_ClimbmillR_` | JIS Retail Climbmill C50 |
| 0x1E | `_IMPULSE_LCB` | C700E Climbmill |
| 0x00 | `_Rower` | Generic Rower |
| 0x01 | `_IndoorCycle` | Indoor Cycle |
| 0x02 | `_SDrive` | S-Drive (Rxp/SxP) |
| 0x03 | `_UBCM` | Upper Body Cycle M |
| 0x04 | `_UBCP` | Upper Body Cycle Phoenix |
| 0x05 | `_Universal` | Universal LCB |
| 0x06 | `_GTM` | GTM |
| 0x16 | `_ITC` | Smart Strength Load Cell |
| 0xC0 | `_DCI_` | DCI daughter board |
| 0xC1 | `_TopTek_` | TopTek MCB (T70/75) |
| 0xC2–0xC8 | `_Delta_I/H/I1–I4` | Delta drive board variants |
| 0xC7 | `_Liteon` | Liteon MCB |
| 0xF0 | `_LiteonJHT` | Liteon (JHT manufactured) |
| 0xA1–0xA3 | `_VAVE_A1/A2/A3` | VAVE drive (T-PS, T600, T-LS) |
| 0xF1 | `_VAVE_F1` | VAVE T-PP |
| 0xC9 / 0xF2 | `_PS_PMSM` / `_PP_PMSM` | PMSM motor variants (2026) |
| 0xE0/0xE1 | `_JISACTM110/220` | JIS AC Treadmill T90 |
| 0xD0/0xD1 | `_JISGMCB110/220` | JIS Global DC MCB |
| 0x0C | `_JISEM` | JIS EM (U/R/SE50I) |
| 0x0E | `_JISDCTM` | JIS DC Treadmill T30/50 |
| 0x12 | `_JISECB` | JIS ECB (U/R/SE30) |
| 0xFF | `_JISECB_I` | JIS ECB-I (A30) |
| 0xFE | `_JISDCLCB` | JIS DC LCB (responds as type 0x04) |
| 0xFD | `_CxpUpdate` | Cxp Update (responds as type 0x03) |
