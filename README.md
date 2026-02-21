# STM32 DHT11 UART BareMetal Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)
![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)


## Video Demonstrations

**Hardware Demo**  

https://github.com/user-attachments/assets/4d8c22fa-11c4-4e12-b30f-000ceb8ef572

**UART Output**  

https://github.com/user-attachments/assets/48de6ca8-5985-420c-825a-51685881ee41

*Real hardware test - hand on sensor increases temperature/humidity*<br>
*Serial monitor showing readings at 115200 baud*

## Project Overview

This project demonstrates **complete bare-metal programming** on the STM32F103C8 "Blue Pill" to read temperature and humidity from a **DHT11 sensor** and output the data via **UART**. All code is written using direct register access, no HAL, no LL, no CubeMX. The system runs at **72 MHz** using HSE external crystal and PLL.

For a **HAL-based version** of this project with LCD 16x2 display:<br>
🔗 [STM32_DHT11_LCD16x2](https://github.com/rubin-khadka/STM32_DHT11_LCD16x2)

### Hardware Components

| Component | Description | Notes |
|-----------|-------------|-------|
| **STM32F103C8T6** | "Blue Pill" board | 72MHz Cortex-M3 MCU |
| **DHT11 Sensor** | Temperature & Humidity | 3.3V-5V, 1-wire protocol |
| **USB-to-Serial Converter** | CP2102 / CH340 / FTDI | For UART communication |

### Pin Configuration

| Peripheral | Pin | Connection | Notes |
|------------|-----|------------|-------|
| **DHT11 Sensor** | PB0 | DATA | Built-in 1kΩ pull-up |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **UART** | PA9 | TX to USB-to-Serial RX | 115200 baud |
| | PA10 | RX to USB-to-Serial TX | Optional |
| | GND | GND to USB-to-Serial GND | Common ground |
| **LED** | PC13 | Onboard LED | Active low |

**Note:** The DHT11 sensor module already has a built-in 1kΩ pull-up resistor so no external resistor needed. The LED on PC13 is just for testing/status indication.

### Timer Configuration

| Timer | Resolution | Purpose |
|-------|------------|---------|
| **TIMER1** | 1µs | DHT11 timing (microsecond precision) |
| **TIMER2** | 0.1ms | longer delays between readings (1 sec) |

### UART Communication

UART uses **interrupts** with circular buffers to transmit sensor data without blocking the main loop.

### DHT11 Sensor Driver

The DHT11 uses a **single-wire protocol** with precise timing:

| Phase | Duration | Description |
|-------|----------|-------------|
| **Start Signal** | 18ms LOW + 20µs HIGH | MCU wakes sensor |
| **Sensor Response** | 80µs LOW + 80µs HIGH | Sensor acknowledges |
| **Bit "0"** | 50µs LOW + 26-28µs HIGH | Logic 0 |
| **Bit "1"** | 50µs LOW + 70µs HIGH | Logic 1 |
| **Data Frame** | 40 bits | 5 bytes (humidity ×2 + temp ×2 + checksum) |

Instead of measuring pulse width, I used a **simpler approach** looking at datasheet:

For each bit:
1. Wait for line to go HIGH
2. Delay exactly 40µs
3. If line still HIGH → logic 1 <br>
   If line is LOW → logic 0

To ensure the timing is not interrupted, **interrupts are disabled** while communicating with the sensor. The checksum provided by the sensor is used to verify data integrity.

🔗 [View DHT11 Driver Source Code](https://github.com/rubin-khadka/STM32_DHT11_UART_BareMetal/blob/main/Src/dht11.c)

### Lightweight String Formatting

Instead of using sprintf() which is computational heavy and increases code size, I created custom lightweight functions:
- itoa_16()     - Converts 16-bit integers to strings
- itoa_8()      - Converts 8-bit integers to strings
- format_value() - Formats temperature/humidity with 1 decimal place
- format_reading() - Builds complete output line: "[count] Temp: XX.XC, Hum: XX.X%"

🔗 [View Utility Functions Source Code](https://github.com/rubin-khadka/STM32_DHT11_UART_BareMetal/blob/main/Src/utils.c)

## Project Schematic

<img width="1100" height="478" alt="STM32_DHT11_UART_Schematic" src="https://github.com/user-attachments/assets/020f7b81-2233-4aad-8f89-412f657e50ee" />

*Schematic designed using EasyEDA*

The schematic shows the simple STM32F103C8T6 implementation including:
- **8MHz HSE clock circuit** (for 72MHz operation)
- **DHT11 sensor connection** (PB0 for data)
- **UART interface** (PA9/PA10 for USB-to-Serial)

## Getting Started

### Prerequisites
| Components | Details |
|------|---------|
| **MCU** | STM32F103C8T6 "Blue Pill" |
| **DHT11 Sensor** | Temperature & Humidity | 
| **USB-to-Serial Converter** | For UART communication |
| **Programmer** | ST-Link V2 |
| **IDE** | STM32cube |

### Installation

1. Clone the repository
```bash
git clone https://github.com/rubin-khadka/STM32_DHT11_UART_BareMetal.git
```
2. Open in STM32CubeIDE
    - `File` → `Import...`
    - `General` → `Existing Projects into Workspace` → `Next`
    - Select the project directory
    - Click `Finish`    

3. Verify Project Settings
    - `Project` → `Properties` → `C/C++ Build` → `Settings`
    - `MCU GCC Compiler` → `Preprocessor`
    - Ensure `STM32F103xB` is defined

4. Build & Flash
    - Build: `Ctrl+B`
    - Debug: `F11`
    - Run: `F8` (Resume)

## Expected Output
```bash
=============================
DHT11 SENSOR READER 
=============================
[1] Temp: 24.0C, Hum: 45.0%
[2] Temp: 24.0C, Hum: 45.0%
[3] Temp: 24.5C, Hum: 46.0%
[4] Temp: 24.5C, Hum: 46.0%
[5] Temp: 25.0C, Hum: 47.0%
```

## Resources
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DHT11 Sensor Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)

## Project Status
- **Status**: Complete
- **Version**: v1.0
- **Last Updated**: February 2026

## Contact
**Rubin Khadka Chhetri**  
📧 rubin.robotic@gmail.com  
🐙 GitHub: https://github.com/rubin-khadka

## Related Project
- [STM32_DHT11_LCD16x2](https://github.com/rubin-khadka/STM32_DHT11_LCD16x2)
- [STM32_Digital_Thermometer_BareMetal](https://github.com/rubin-khadka/STM32_Digital_Thermometer_BareMetal)
