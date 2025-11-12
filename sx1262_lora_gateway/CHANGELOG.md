# Changelog

All notable changes to this add-on will be documented in this file.

## [1.4.0] - 2025-11-11

### Added
- Flexible sync word configuration (force 16-bit, raw MSB/LSB, legacy single byte)
- Sync word register readback logging for alignment with Heltec ESP32 transformations
- Heartbeat now includes instantaneous RSSI sampling
- Expanded README with sync word transformation note and advanced overrides

### Fixed
- Mismatch assumption of ESP32 sync word (0x3434) vs actual transformed bytes (observed 0x3424)
- Improved troubleshooting guidance when no packets are received

### Changed
- Default documentation sync word example updated to 0x34 with explanation of transformation

## [1.3.4] - 2025-11-10

### Added
- Initial release of SX1262 LoRa Gateway add-on
- Support for Waveshare SX1262 LoRaWAN HAT
- LoRa message reception on 915MHz
- JSON sensor data parsing
- MQTT integration with Home Assistant
- Configurable LoRa parameters (frequency, SF, BW, CR, sync word)
- Signal quality monitoring (RSSI, SNR)
- Gateway statistics tracking
- Individual MQTT topics for each sensor value
- Auto-reconnect for MQTT connection
- Detailed logging with configurable log levels

### Features
- Receives data from ESP32 water sensor bridge
- Publishes to MQTT for Home Assistant integration
- Compatible with ME201W ultrasonic water level sensor data
- Battery monitoring
- Connection quality metrics
- Gateway uptime and message statistics

### Configuration Options
- LoRa frequency (902-928 MHz)
- Spreading factor (SF6-SF12)
- Bandwidth (7.8kHz - 500kHz)
- Coding rate (4/5 - 4/8)
- Sync word (0x00-0xFF)
- TX power (2-22 dBm)
- MQTT broker settings
- Topic prefix customization
- Log level control
