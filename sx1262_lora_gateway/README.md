# SX1262 LoRa Gateway for Home Assistant

A universal LoRa to MQTT gateway add-on for Home Assistant. Receives LoRa packets from ESP32/Arduino devices with SX126x radios and publishes them to MQTT with automatic JSON parsing for seamless Home Assistant integration.

See the complete documentation and examples at: https://github.com/DShaeffer/homeassistant-addon-sx1262

## ✨ Features

- 🎯 **Universal Compatibility** - Works with any LoRa device using SX126x radios
- 📡 **Flexible Configuration** - Full control over LoRa parameters
- 🔄 **Auto JSON Parsing** - Automatically flattens nested JSON and creates individual MQTT topics
- 📊 **Signal Quality Monitoring** - Reports RSSI and SNR for each packet
- 🏠 **Home Assistant Native** - Seamless MQTT integration
- 🚰 **Production Example** - Includes complete ME201W water sensor bridge application

## 🔌 Hardware Requirements

- Raspberry Pi (3/4/5 or Zero 2 W)
- [Waveshare SX1262 LoRa HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm) (915MHz for US, 868MHz for EU)
- Home Assistant OS or Supervised
- MQTT Broker (Mosquitto add-on recommended)

## 📥 Quick Start

1. **Enable SPI** on your Raspberry Pi (required for HAT communication)
2. **Add repository** to Home Assistant: `https://github.com/DShaeffer/homeassistant-addon-sx1262`
3. **Install** SX1262 LoRa Gateway add-on
4. **Configure** LoRa parameters (must match your ESP32 settings)
5. **Start** the add-on and check logs

For detailed installation and configuration, see the full documentation link above.
