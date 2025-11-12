# SX1262 LoRa Gateway for Home Assistant

A general-purpose LoRa to MQTT gateway add-on for Home Assistant. Receives LoRa packets from ESP32/Arduino devices with SX126x radios and publishes them to MQTT for easy integration with Home Assistant.

## Features

-  **Universal Compatibility**: Works with any LoRa device using SX126x radios (SX1261, SX1262, SX1268)
-  **Flexible Configuration**: Full control over LoRa parameters (frequency, spreading factor, bandwidth, coding rate, sync word)
-  **Auto JSON Parsing**: Automatically publishes all JSON fields as individual MQTT topics
-  **Signal Quality Monitoring**: Reports RSSI and SNR for each received packet
-  **Home Assistant Native**: Designed for seamless integration with Home Assistant
-  **Advanced Sync Word Options**: Support for precise sync word control for challenging compatibility scenarios

## Hardware Requirements

- Raspberry Pi (any model with 40-pin GPIO header)
- **Waveshare SX1262 LoRa HAT** (868MHz or 915MHz depending on your region)
- Home Assistant OS or Supervised installation

## Installation

1. Add this repository to your Home Assistant Add-on Store:
   - Navigate to **Settings**  **Add-ons**  **Add-on Store** (three dots menu, top right)  **Repositories**
   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`

2. Install the **SX1262 LoRa Gateway** add-on

3. Configure the add-on (see Configuration section below)

4. Start the add-on and check the logs

See the complete documentation at: https://github.com/DShaeffer/homeassistant-addon-sx1262/blob/main/sx1262_lora_gateway/README.md
