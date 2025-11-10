# ESP32 Water Sensor Bridge - Main Application

This folder contains the full water sensor bridge application that reads from the ME201W sensor and transmits via LoRa.

## Files
- `esp32_serial_reader.ino` - Complete water sensor bridge with:
  - Serial communication with ME201W sensor
  - LoRa transmission to Raspberry Pi gateway
  - OLED display
  - Deep sleep power management
  - Statistics tracking

## Usage
Use this once you've verified basic LoRa communication works with the test sketches in `../esp32_tests/`.

## Opening in Arduino IDE
Open `esp32_serial_reader.ino` in Arduino IDE. Make sure NO other .ino files are in this folder to avoid compilation conflicts.
