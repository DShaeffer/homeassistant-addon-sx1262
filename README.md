# SX1262 LoRa Gateway for Home Assistant

[![Add repository to Home Assistant][repository-badge]][repository-url]

[repository-badge]: https://img.shields.io/badge/Add%20repository%20to%20my-Home%20Assistant-41BDF5?logo=home-assistant&style=for-the-badge
[repository-url]: https://my.home-assistant.io/redirect/supervisor_add_addon_repository/?repository_url=https%3A%2F%2Fgithub.com%2FDShaeffer%2Fhomeassistant-addon-sx1262

A complete LoRa to MQTT gateway system for Home Assistant. This repository includes both the Raspberry Pi gateway add-on and a production-ready ESP32 water sensor bridge application demonstrating deep sleep power management, button wake, and seamless Home Assistant integration.

## 🌟 What's Included

### 1. Home Assistant Gateway Add-on
A universal LoRa receiver that runs on your Raspberry Pi with a Waveshare SX1262 HAT. Receives LoRa packets from any ESP32/Arduino device and publishes them to MQTT with automatic JSON parsing.

**📂 Location:** [`sx1262_lora_gateway/`](sx1262_lora_gateway/)

### 2. ESP32 Water Sensor Bridge (Production Example)
A complete, production-ready application for reading Tuya ME201W water sensors via serial and transmitting via LoRa. Features advanced power management with deep sleep, button wake for display, and complete Home Assistant integration.

**📂 Location:** [`esp32_tuya_me201ws_serial_reader/`](esp32_tuya_me201ws_serial_reader/)

### 3. ESP32 Test Sketches
Simple test applications for validating LoRa communication during initial setup.

**📂 Location:** [`esp32_tests/`](esp32_tests/)

## ✨ Key Features

- 🎯 **Universal Compatibility**: Works with any SX126x LoRa device (SX1261, SX1262, SX1268)
- 📡 **Flexible Configuration**: Full control over LoRa parameters (frequency, SF, BW, CR, sync word)
- 🔄 **Auto JSON Parsing**: Automatically creates MQTT topics from nested JSON structures
- 📊 **Signal Monitoring**: RSSI and SNR reporting for every packet
- 🏠 **Home Assistant Native**: Seamless MQTT integration with auto-discovery support
- � **Low Power**: ESP32 deep sleep with wake-on-serial and button wake
- 🔧 **Production Ready**: Complete working example with the ME201W water sensor

## 🚀 Quick Start

### Hardware Requirements

**Gateway (Receiver):**
- Raspberry Pi (3/4/5 or Zero 2 W)
- [Waveshare SX1262 LoRaWAN HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm) (915MHz for US)
- Home Assistant OS or Supervised installation

**Transmitter (Example):**
- Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- Tuya ME201W water sensor (optional, for full water sensor bridge)



**Transmitter (Example):**
- Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- Tuya ME201W water sensor (optional, for full water sensor bridge)

### Installation Steps

1. **Install the Gateway Add-on**

   Click the button at the top of this page or manually add this repository:
   - Go to **Settings** → **Add-ons** → **Add-on Store** (three dots menu) → **Repositories**
   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`
   - Find **SX1262 LoRa Gateway** and click **Install**

2. **Configure LoRa Parameters**

   Set your region-specific frequency and LoRa parameters (must match ESP32 settings):
   ```yaml
   lora_frequency: 915.0           # US: 915, EU: 868
   lora_spreading_factor: 7        # SF7-SF12
   lora_bandwidth: 125000          # 125kHz
   lora_sync_word: 0x12           # Must match ESP32
   ```

3. **Start the Gateway**

   Click **Start** and check the logs for "LoRa receiver initialized"

4. **Program Your ESP32**

   - For testing: Use [Heltec_Gateway_Test](esp32_tests/Heltec_Gateway_Test/)
   - For production: Use [ME201W Water Sensor Bridge](esp32_tuya_me201ws_serial_reader/)

5. **Add Sensors to Home Assistant**

   The gateway auto-creates MQTT topics. Add them to your `configuration.yaml` (see examples below)

## 📡 MQTT Topic Structure

The gateway automatically creates MQTT topics from JSON payloads. 

**Example:** ESP32 sends this JSON:
```json
{
  "water": {
    "level": 85.2,
    "percent": 76,
    "state": 0
  },
  "batt": {
    "voltage": 4.15,
    "unit": 95
  }
}
```

**Gateway creates these topics:**
- `lora/gateway/water/level` → `85.2`
- `lora/gateway/water/percent` → `76`
- `lora/gateway/water/state` → `0`
- `lora/gateway/batt/voltage` → `4.15`
- `lora/gateway/batt/unit` → `95`
- `lora/gateway/rssi` → `-35.2` (signal strength)
- `lora/gateway/snr` → `9.75` (signal quality)

## 🏡 Home Assistant Integration

Add these sensors to your `configuration.yaml`:

```yaml
mqtt:
  sensor:
    # Water Level Sensors
    - name: "Water Tank Level"
      state_topic: "lora/gateway/water/level"
      unit_of_measurement: "cm"
      device_class: distance
      icon: mdi:water-well
    
    - name: "Water Tank Percent"
      state_topic: "lora/gateway/water/percent"
      unit_of_measurement: "%"
      icon: mdi:water-percent
    
    # Battery Sensors
    - name: "Water Sensor Battery"
      state_topic: "lora/gateway/batt/voltage"
      unit_of_measurement: "V"
      device_class: voltage
    
    - name: "Water Sensor Battery Level"
      state_topic: "lora/gateway/batt/unit"
      unit_of_measurement: "%"
      device_class: battery
    
    # Signal Quality
    - name: "Water Sensor RSSI"
      state_topic: "lora/gateway/rssi"
      unit_of_measurement: "dBm"
      device_class: signal_strength
    
    - name: "Water Sensor SNR"
      state_topic: "lora/gateway/snr"
      unit_of_measurement: "dB"
      icon: mdi:signal-variant
  
  binary_sensor:
    # Water Level Alarm
    - name: "Water Tank Alarm"
      state_topic: "lora/gateway/water/state"
      payload_off: "0"
      payload_on: "1"
      device_class: problem
```

After restarting Home Assistant, your sensors will appear and can be added to your dashboard.

## 📚 Documentation

- **[Gateway Add-on Documentation](sx1262_lora_gateway/README.md)** - Complete gateway configuration and troubleshooting
- **[ME201W Water Sensor Bridge](esp32_tuya_me201ws_serial_reader/README.md)** - Production ESP32 application with deep sleep
- **[ESP32 Test Sketches](esp32_tests/README.md)** - Simple examples for testing LoRa communication
- **[Changelog](sx1262_lora_gateway/CHANGELOG.md)** - Version history

## 🔧 Troubleshooting

### No Packets Received?

1. **Check Frequency**: US=915MHz, EU=868MHz - must match exactly between gateway and ESP32
2. **Verify Sync Word**: Must be identical on both devices (default: `0x12`)
3. **Check Distance**: Start with devices < 10 meters apart for initial testing
4. **Monitor Logs**: Gateway logs show RSSI even when packets fail to decode

### Weak Signal (RSSI < -100 dBm)?

1. **Check Antennas**: Ensure properly connected on both devices
2. **Increase TX Power**: Try 22 dBm on ESP32 (within legal limits)
3. **Increase Spreading Factor**: SF9 or SF10 for longer range (slower data rate)
4. **Line of Sight**: Obstacles significantly reduce range

### ESP32 Not Transmitting?

1. **Check Serial Monitor**: Verify LoRa initialization succeeded
2. **Verify Pin Configuration**: Heltec V3 uses specific pins (see code)
3. **Check Power**: Low battery can prevent LoRa transmission

See the [complete troubleshooting guide](sx1262_lora_gateway/README.md#troubleshooting) for more details.

## 💡 Use Cases

This system is perfect for:

- 🚰 **Water Tank Monitoring** - Track levels remotely with low power consumption
- 🌡️ **Environmental Sensors** - Temperature, humidity, soil moisture
- 🔋 **Battery-Powered IoT** - Deep sleep enables months of battery life
- 🏡 **Smart Home Sensors** - Door/window sensors, motion detectors
- 📊 **Agricultural Monitoring** - Soil conditions, weather stations
- 🔔 **Alarm Systems** - Remote notifications without WiFi/cellular

Anything that needs wireless sensing beyond WiFi range with low power consumption!

## 🎯 Project Structure

```
homeassistant-addon-sx1262/
├── README.md                                  # This file
├── repository.json                            # HA add-on repository config
│
├── sx1262_lora_gateway/                       # Raspberry Pi Gateway Add-on
│   ├── README.md                              # Complete gateway documentation
│   ├── CHANGELOG.md                           # Version history
│   ├── config.yaml                            # Add-on configuration schema
│   ├── Dockerfile                             # Container build
│   ├── lora_gateway.py                        # Main gateway application
│   └── LoRaRF/                               # LoRa radio library
│
├── esp32_tuya_me201ws_serial_reader/         # Production Water Sensor Bridge
│   ├── README.md                              # Complete bridge documentation
│   └── esp32_tuya_me201ws_serial_reader.ino  # Main application (~1100 lines)
│
└── esp32_tests/                               # Test & Development Sketches
    ├── README.md                              # Testing guide
    ├── Heltec_Gateway_Test/                  # Simple LoRa test (recommended)
    ├── LoRa_TX_Test_Simple/                  # Basic transmission test
    └── [other test sketches]/                # Various diagnostic tools
```

## 🌟 Key Features of the ME201W Water Sensor Bridge

The included ESP32 application demonstrates production-level code:

- ✅ **Deep Sleep Power Management** - Wake on UART activity from sensor
- ✅ **Button Wake** - Press PRG button to view stats on OLED display
- ✅ **Reboot-to-Sleep Strategy** - Eliminates deep sleep crashes
- ✅ **Complete Data Validation** - Only transmits when full sensor data received
- ✅ **LoRa Transmission** - Reliable 915MHz SF7 communication
- ✅ **OLED Display** - Shows sensor data and statistics on demand
- ✅ **JSON Payload** - Structured data for easy Home Assistant integration
- ✅ **Signal Reporting** - RSSI and SNR visible in Home Assistant

Typical power consumption: 
- Deep sleep: ~2-5mA
- Active transmission: ~100-120mA for <1 second
- Display active: ~50-80mA for 30 seconds

With solar charging and 18650 battery, this system can run indefinitely!

## 🤝 Contributing

Contributions welcome! Please open an issue or pull request.

## 💬 Support

- **Issues**: [GitHub Issues](https://github.com/DShaeffer/homeassistant-addon-sx1262/issues)
- **Discussions**: [GitHub Discussions](https://github.com/DShaeffer/homeassistant-addon-sx1262/discussions)

## 📄 License

MIT License - Free to use and modify

## 🙏 Credits

- Built with [LoRaRF Python Library](https://github.com/chandrawi/LoRaRF-Python)
- Designed for [Waveshare SX1262 LoRa HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm)
- Compatible with Heltec, TTGO, and generic SX126x boards
- Tested with Tuya ME201W ultrasonic water sensors[![Add repository to Home Assistant][repository-badge]][repository-url]## Features


