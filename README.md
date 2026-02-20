# STM32 DHT11 UART BareMetal Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)
![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)


## Video Demonstrations

https://github.com/user-attachments/assets/4d8c22fa-11c4-4e12-b30f-000ceb8ef572


https://github.com/user-attachments/assets/48de6ca8-5985-420c-825a-51685881ee41

*Real hardware test - hand on sensor increases temperature/humidity*<br>
*Serial monitor showing readings at 115200 baud*

## Project Overview

This project demonstrates **complete bare-metal programming** on the STM32F103C8 "Blue Pill" to read temperature and humidity from a **DHT11 sensor** and output the data via **UART**. All code is written using direct register access, no HAL, no LL, no CubeMX.

### Timer Configuration

| Timer | Resolution | Purpose |
|-------|------------|---------|
| **TIMER1** | 1µs | DHT11 timing (microsecond precision) |
| **TIMER2** | 1ms | longer delays between readings |

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
3. If line still HIGH → logic 1
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