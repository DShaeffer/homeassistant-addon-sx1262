# Home Assistant SX1262 LoRa Gateway# Home Assistant Add-ons for SX1262 LoRa# SX1262 LoRa Gateway Add-on for Home Assistant



[![Add repository to Home Assistant][repository-badge]][repository-url]



A general-purpose LoRa to MQTT gateway add-on for Home Assistant. Works with any ESP32/Arduino device using SX126x radios (SX1261, SX1262, SX1268).This repository contains Home Assistant add-ons for working with SX1262 LoRa devices.This add-on enables your Raspberry Pi with a Waveshare SX1262 LoRaWAN HAT to receive LoRa messages from 



[repository-badge]: https://img.shields.io/badge/Add%20repository%20to%20my-Home%20Assistant-41BDF5?logo=home-assistant&style=for-the-badgeremote sensors (such as the ESP32 water sensor bridge) and integrate them into Home Assistant via MQTT. 

[repository-url]: https://my.home-assistant.io/redirect/supervisor_add_addon_repository/?repository_url=https%3A%2F%2Fgithub.com%2FDShaeffer%2Fhomeassistant-addon-sx1262

## Add-ons## Hardware Requirements

## Features



- 🎯 **Universal Compatibility**: Works with any SX126x LoRa device

- 📡 **Flexible Configuration**: Full control over LoRa parameters### SX1262 LoRa Gateway- Raspberry Pi (3/4/5 or Zero 2 W)

- 🔄 **Auto JSON Parsing**: Automatically creates MQTT topics from any JSON structure

- 📊 **Signal Monitoring**: RSSI and SNR reporting for each packet- [Waveshare SX1262 LoRaWAN HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm)

- 🏠 **Home Assistant Native**: Seamless MQTT integration

- 🔧 **Advanced Sync Word Options**: Multiple configuration methods for device compatibilityA LoRa receiver add-on for Waveshare SX1262 HAT that receives data from remote sensors and publishes to MQTT.- Properly installed HAT on GPIO pins



## Quick Start



### 1. Install the Add-on[![Add repository to Home Assistant][repository-badge]][repository-url]## Features



Click the button above or manually add this repository:



1. Go to **Settings** → **Add-ons** → **Add-on Store** (three dots menu, top right) → **Repositories**## Installation- Receives LoRa messages on 915MHz (configurable)

2. Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`

3. Find **SX1262 LoRa Gateway** and click **Install**- Parses JSON sensor data from ESP32 bridge



### 2. Hardware Required1. Click the button above or manually add this repository to your Home Assistant instance:- Publishes to MQTT for Home Assistant integration



- **Gateway**: Raspberry Pi with Waveshare SX1262 LoRa HAT   - Go to **Settings** → **Add-ons** → **Add-on Store** - Configurable LoRa parameters (SF, BW, CR, sync word)

- **Transmitter**: ESP32/Arduino with SX126x radio (Heltec, TTGO, etc.)

   - Click the three dots menu (⋮) in the top right- Signal quality monitoring (RSSI, SNR)

### 3. Configure & Start

   - Select **Repositories**- Gateway statistics tracking

1. Configure LoRa parameters (frequency, spreading factor, bandwidth)

2. Set MQTT broker settings (default: `core-mosquitto`)   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`- Auto-discovery compatible MQTT topics

3. Start the add-on

   - Click **Add**

### 4. Test with ESP32

## Installation

See the [Heltec Example](esp32_tests/Heltec_Gateway_Test/) for a ready-to-use Arduino sketch.

2. The add-ons from this repository will now be available in your add-on store.

## Documentation

1. **Enable SPI on your Raspberry Pi:**

- **Gateway Documentation**: [sx1262_lora_gateway/README.md](sx1262_lora_gateway/README.md)

- **Heltec ESP32 Example**: [esp32_tests/Heltec_Gateway_Test/README.md](esp32_tests/Heltec_Gateway_Test/README.md)## Support   - Go to Supervisor â System â Host â Hardware

- **Changelog**: [sx1262_lora_gateway/CHANGELOG.md](sx1262_lora_gateway/CHANGELOG.md)

   - Click the three dots menu â "Import from USB"

## Basic Configuration

For issues, questions, or contributions, please visit the [GitHub repository](https://github.com/DShaeffer/homeassistant-addon-sx1262).   - Or via SSH: `raspi-config` â Interfacing Options â SPI â Enable

```yaml

lora_frequency: 915.0              # Your region (US: 915, EU: 868)

lora_spreading_factor: 7           # SF7-SF12

lora_bandwidth: 125000             # 125kHz for long range[repository-badge]: https://img.shields.io/badge/Add%20repository%20to%20my-Home%20Assistant-41BDF5?logo=home-assistant&style=for-the-badge2. **Add the repository to Home Assistant:**

lora_coding_rate: 5                # 4/5 coding rate

lora_sync_word: 52                 # Standard private network[repository-url]: https://my.home-assistant.io/redirect/supervisor_add_addon_repository/?repository_url=https%3A%2F%2Fgithub.com%2FDShaeffer%2Fhomeassistant-addon-sx1262   - Go to Supervisor â Add-on Store

mqtt_host: core-mosquitto

mqtt_topic_prefix: "lora/gateway"  # Customize for your use   - Click the three dots menu (top right) â Repositories

```   - Add this repository URL: `file:///config/addons/homeassistant-addon-sx1262`

   - Or copy the addon folder to `/addons/` directory

## MQTT Topic Structure

3. **Install the add-on:**

The gateway automatically creates MQTT topics from your JSON payloads:   - Find "SX1262 LoRa Gateway" in the add-on store

   - Click on it and press "INSTALL"

**Example:** Sending this JSON:

```json4. **Configure the add-on:**

{   - Go to the Configuration tab

  "temperature": 23.5,   - Set your LoRa parameters (must match ESP32 settings!)

  "humidity": 65,   - Configure MQTT broker (default: core-mosquitto)

  "battery": {"voltage": 3.95}   - Click "SAVE"

}

```5. **Start the add-on:**

   - Go to the Info tab

**Creates these topics:**   - Toggle "Start on boot" if desired

- `lora/gateway/temperature` → `23.5`   - Click "START"

- `lora/gateway/humidity` → `65`

- `lora/gateway/battery/voltage` → `3.95`## Configuration

- `lora/gateway/rssi` → `-45.0` (signal strength)

- `lora/gateway/snr` → `9.5` (signal quality)### LoRa Settings



## Example Home Assistant Sensors```yaml

lora_frequency: 915.0          # Frequency in MHz (902-928 for US)

```yamllora_spreading_factor: 7       # SF7-SF12 (7=fastest, 12=longest range)

mqtt:lora_bandwidth: 125000         # Bandwidth in Hz (125kHz default)

  sensor:lora_coding_rate: 5            # 4/5, 4/6, 4/7, 4/8

    - name: "LoRa Temperature"lora_sync_word: 0x12          # Must match ESP32 setting

      state_topic: "lora/gateway/temperature"lora_tx_power: 20             # TX power in dBm (2-22)

      unit_of_measurement: "°C"```

      device_class: temperature

      ### MQTT Settings

    - name: "LoRa Signal Strength"

      state_topic: "lora/gateway/rssi"```yaml

      unit_of_measurement: "dBm"mqtt_host: core-mosquitto      # MQTT broker hostname

      device_class: signal_strengthmqtt_port: 1883               # MQTT broker port

```mqtt_username: ""             # Optional MQTT username

mqtt_password: ""             # Optional MQTT password

## Supported Hardwaremqtt_topic_prefix: "lora/water_sensor"  # Topic prefix for all messages

```

### Gateway (Receiver)

- Waveshare SX1262 LoRa HAT (868MHz or 915MHz)### Logging

- Raspberry Pi (any model with 40-pin GPIO)

```yaml

### Transmitters (Tested)log_level: info               # debug, info, warning, error

- ✅ Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)```

- ✅ Heltec WiFi LoRa 32 V2

- ✅ TTGO LoRa boards## MQTT Topics

- ✅ Generic ESP32 + SX126x combinations

The add-on publishes to the following topics:

## Project Structure

### Main Data Topics

```- `lora/water_sensor/data` - Complete JSON payload

homeassistant-addon-sx1262/- `lora/water_sensor/status` - Gateway online/offline status

├── sx1262_lora_gateway/          # Home Assistant add-on- `lora/water_sensor/last_seen` - Last message timestamp

│   ├── README.md                 # Complete documentation

│   ├── CHANGELOG.md              # Version history### Water Sensor Topics

│   ├── config.yaml               # Add-on configuration- `lora/water_sensor/water/level` - Water level in cm

│   └── lora_gateway.py           # Gateway application- `lora/water_sensor/water/percent` - Water level percentage

│- `lora/water_sensor/water/raw_distance` - Raw ultrasonic distance

└── esp32_tests/- `lora/water_sensor/water/state` - Alarm state (0=normal, 1=low, 2=high)

    ├── Heltec_Gateway_Test/      # Ready-to-use example

    │   ├── README.md             # Upload instructions### Battery Topics

    │   └── Heltec_Gateway_Test.ino- `lora/water_sensor/battery/voltage` - Battery voltage

    │- `lora/water_sensor/battery/unit` - Battery level (0-100)

    └── LoRa_TX_Test_Simple/      # Development/diagnostic version

```### Signal Quality Topics

- `lora/water_sensor/rssi` - Received Signal Strength Indicator (dBm)

## Troubleshooting- `lora/water_sensor/snr` - Signal-to-Noise Ratio (dB)



### No Packets Received?### Gateway Statistics

- `lora/water_sensor/gateway/stats` - Gateway statistics (JSON)

1. **Check Frequency**: US=915MHz, EU=868MHz - must match exactly

2. **Try Sync Word Override**: Add `lora_sync_word_force: "0x3424"` to config## Home Assistant Integration

3. **Verify Distance**: Start with devices < 10 meters apart

4. **Check Logs**: Look for "RSSIinst" showing RF activityAfter the add-on is running, add these sensors to your `configuration.yaml`:



### Weak Signal?```yaml

mqtt:

1. **Increase TX Power**: Try 20 dBm on transmitter  sensor:

2. **Increase Spreading Factor**: SF9 or SF10 for longer range    # Water Level

3. **Check Antenna**: Ensure properly connected    - name: "Water Tank Level"

      state_topic: "lora/water_sensor/water/level"

See the [complete troubleshooting guide](sx1262_lora_gateway/README.md#troubleshooting).      unit_of_measurement: "cm"

      icon: mdi:water-well

## Version History

    - name: "Water Tank Percent"

- **v1.0.0** (2025-11-11): First production release - General-purpose gateway      state_topic: "lora/water_sensor/water/percent"

- Previous versions were development releases      unit_of_measurement: "%"

      icon: mdi:water-percent

See [CHANGELOG.md](sx1262_lora_gateway/CHANGELOG.md) for details.

    # Battery

## Use Cases    - name: "Water Sensor Battery"

      state_topic: "lora/water_sensor/battery/voltage"

This gateway can be used for:      unit_of_measurement: "V"

- 🌡️ Remote temperature/humidity monitoring      device_class: voltage

- 🔋 Battery-powered sensor networks      icon: mdi:battery

- 🚰 Tank level monitoring

- 🌾 Agricultural sensors    # Signal Quality

- 🏡 Smart home IoT devices    - name: "Water Sensor RSSI"

- 📊 Environmental monitoring stations      state_topic: "lora/water_sensor/rssi"

- 🔔 Alarm/notification systems      unit_of_measurement: "dBm"

      icon: mdi:signal

Anything that can send JSON over LoRa!

    - name: "Water Sensor SNR"

## Contributing      state_topic: "lora/water_sensor/snr"

      unit_of_measurement: "dB"

Contributions welcome! Please open an issue or pull request.      icon: mdi:signal-variant



## Support    # Last Seen

    - name: "Water Sensor Last Seen"

- **Issues**: [GitHub Issues](https://github.com/DShaeffer/homeassistant-addon-sx1262/issues)      state_topic: "lora/water_sensor/last_seen"

- **Discussions**: [GitHub Discussions](https://github.com/DShaeffer/homeassistant-addon-sx1262/discussions)      icon: mdi:clock-outline



## License  binary_sensor:

    # Gateway Status

MIT License - Free to use and modify    - name: "LoRa Gateway"

      state_topic: "lora/water_sensor/status"

## Credits      payload_on: "online"

      payload_off: "offline"

- Built with [LoRaRF Python Library](https://github.com/chandrawi/LoRaRF-Python)      device_class: connectivity

- Designed for Waveshare SX1262 LoRa HAT```

- Compatible with Heltec, TTGO, and generic SX126x boards

## Troubleshooting

### Add-on won't start
- Check that SPI is enabled on the Raspberry Pi
- Verify the HAT is properly seated on GPIO pins
- Check logs for specific error messages

### No messages received
- Verify ESP32 is transmitting (check ESP32 Serial Monitor)
- Ensure LoRa parameters match between ESP32 and gateway
- Check antenna connections on both devices
- Verify frequency is correct for your region (915MHz for US)

### MQTT not working
- Verify Mosquitto broker add-on is installed and running
- Check MQTT credentials if authentication is enabled
- Test MQTT connection with MQTT Explorer

### Poor signal quality
- Move devices closer together initially to verify communication
- Check antenna orientation (vertical is best for omni-directional)
- Increase spreading factor (reduces speed but increases range)
- Check for interference sources

## Support

For issues specific to:
- **The add-on**: Check add-on logs in Home Assistant
- **The Waveshare HAT**: Refer to [Waveshare documentation](https://www.waveshare.com/wiki/SX1262_LoRaWA
N_HAT)
- **Home Assistant**: Visit [Home Assistant forums](https://community.home-assistant.io/)

## License

MIT License - feel free to modify and distribute.

## Version History

See CHANGELOG.md for version history.
