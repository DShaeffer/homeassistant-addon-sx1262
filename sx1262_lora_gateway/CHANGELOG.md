# Changelog

All notable changes to this add-on will be documented in this file.

## [1.0.0] - 2025-11-11

### 🎉 First Production Release

This is the first official production release of the SX1262 LoRa Gateway for Home Assistant. The add-on has been completely generalized for universal compatibility with any LoRa device using SX126x radios.

### Major Changes
- **Generalized Gateway**: No longer specific to water sensors - works with any JSON payload structure
- **Universal JSON Parsing**: Automatically creates MQTT topics for all nested JSON fields
- **Comprehensive Documentation**: Complete rewrite with Heltec ESP32 example code
- **Production-Ready**: Stable sync word handling, tested with Heltec WiFi LoRa 32 V3

### Added
- Recursive JSON parser that auto-publishes all nested fields as individual MQTT topics
- Complete Heltec ESP32 example sketch (`Heltec_Gateway_Test.ino`)
- Comprehensive README with quickstart guide, troubleshooting, and Home Assistant integration
- Support for any SX126x-based LoRa device (not just specific sensors)
- Default MQTT prefix changed to `lora/gateway` (more generic)
- Multiple sync word configuration methods for compatibility with various LoRa libraries

### Changed
- **Breaking**: MQTT topic structure now reflects actual JSON structure (auto-generated from payload)
- **Breaking**: Default MQTT prefix changed from `lora/water_sensor` to `lora/gateway`
- Log level default changed from `debug` to `info` for cleaner production logs
- Function renamed from `parse_and_publish_sensor_data` to `parse_and_publish_data`
- Updated all documentation to focus on general-purpose gateway functionality

### Fixed
- Sync word handling for Heltec devices (0x3424 transformation)
- SNR method compatibility with LoRaRF library
- JSON parsing now handles any structure, not just predefined sensor fields

### Technical Details
- Python recursive dict/list traversal for automatic MQTT topic generation
- Supports deeply nested JSON structures
- Compatible with Heltec, TTGO, and generic SX126x ESP32 boards
- Tested with Waveshare SX1262 HAT on Raspberry Pi

### Migration Notes

If upgrading from pre-1.0.0 versions:

1. **Update MQTT Topic Prefix**: Default changed to `lora/gateway`
   - Old: `lora/water_sensor/*`
   - New: `lora/gateway/*`
   
2. **Update Home Assistant Sensors**: MQTT topics now match your JSON structure exactly
   - Example: `{"water":{"level":42}}` creates topic `lora/gateway/water/level`

3. **Review Configuration**: Gateway is now application-agnostic - customize `mqtt_topic_prefix` for your use case

### Development History

Previous versions (1.3.4 - 1.4.1) were development releases focused on water sensor integration and sync word compatibility. Version 1.0.0 represents the first stable, general-purpose release suitable for public use.
- Bandwidth (7.8kHz - 500kHz)
- Coding rate (4/5 - 4/8)
- Sync word (0x00-0xFF)
- TX power (2-22 dBm)
- MQTT broker settings
- Topic prefix customization
- Log level control
