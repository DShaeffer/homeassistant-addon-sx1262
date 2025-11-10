# SX1262 LoRa Gateway Add-on for Home Assistant

This add-on enables your Raspberry Pi with a Waveshare SX1262 LoRaWAN HAT to receive LoRa messages from 
remote sensors (such as the ESP32 water sensor bridge) and integrate them into Home Assistant via MQTT. 
## Hardware Requirements

- Raspberry Pi (3/4/5 or Zero 2 W)
- [Waveshare SX1262 LoRaWAN HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm)
- Properly installed HAT on GPIO pins

## Features

- Receives LoRa messages on 915MHz (configurable)
- Parses JSON sensor data from ESP32 bridge
- Publishes to MQTT for Home Assistant integration
- Configurable LoRa parameters (SF, BW, CR, sync word)
- Signal quality monitoring (RSSI, SNR)
- Gateway statistics tracking
- Auto-discovery compatible MQTT topics

## Installation

1. **Enable SPI on your Raspberry Pi:**
   - Go to Supervisor â System â Host â Hardware
   - Click the three dots menu â "Import from USB"
   - Or via SSH: `raspi-config` â Interfacing Options â SPI â Enable

2. **Add the repository to Home Assistant:**
   - Go to Supervisor â Add-on Store
   - Click the three dots menu (top right) â Repositories
   - Add this repository URL: `file:///config/addons/homeassistant-addon-sx1262`
   - Or copy the addon folder to `/addons/` directory

3. **Install the add-on:**
   - Find "SX1262 LoRa Gateway" in the add-on store
   - Click on it and press "INSTALL"

4. **Configure the add-on:**
   - Go to the Configuration tab
   - Set your LoRa parameters (must match ESP32 settings!)
   - Configure MQTT broker (default: core-mosquitto)
   - Click "SAVE"

5. **Start the add-on:**
   - Go to the Info tab
   - Toggle "Start on boot" if desired
   - Click "START"

## Configuration

### LoRa Settings

```yaml
lora_frequency: 915.0          # Frequency in MHz (902-928 for US)
lora_spreading_factor: 7       # SF7-SF12 (7=fastest, 12=longest range)
lora_bandwidth: 125000         # Bandwidth in Hz (125kHz default)
lora_coding_rate: 5            # 4/5, 4/6, 4/7, 4/8
lora_sync_word: 0x12          # Must match ESP32 setting
lora_tx_power: 20             # TX power in dBm (2-22)
```

### MQTT Settings

```yaml
mqtt_host: core-mosquitto      # MQTT broker hostname
mqtt_port: 1883               # MQTT broker port
mqtt_username: ""             # Optional MQTT username
mqtt_password: ""             # Optional MQTT password
mqtt_topic_prefix: "lora/water_sensor"  # Topic prefix for all messages
```

### Logging

```yaml
log_level: info               # debug, info, warning, error
```

## MQTT Topics

The add-on publishes to the following topics:

### Main Data Topics
- `lora/water_sensor/data` - Complete JSON payload
- `lora/water_sensor/status` - Gateway online/offline status
- `lora/water_sensor/last_seen` - Last message timestamp

### Water Sensor Topics
- `lora/water_sensor/water/level` - Water level in cm
- `lora/water_sensor/water/percent` - Water level percentage
- `lora/water_sensor/water/raw_distance` - Raw ultrasonic distance
- `lora/water_sensor/water/state` - Alarm state (0=normal, 1=low, 2=high)

### Battery Topics
- `lora/water_sensor/battery/voltage` - Battery voltage
- `lora/water_sensor/battery/unit` - Battery level (0-100)

### Signal Quality Topics
- `lora/water_sensor/rssi` - Received Signal Strength Indicator (dBm)
- `lora/water_sensor/snr` - Signal-to-Noise Ratio (dB)

### Gateway Statistics
- `lora/water_sensor/gateway/stats` - Gateway statistics (JSON)

## Home Assistant Integration

After the add-on is running, add these sensors to your `configuration.yaml`:

```yaml
mqtt:
  sensor:
    # Water Level
    - name: "Water Tank Level"
      state_topic: "lora/water_sensor/water/level"
      unit_of_measurement: "cm"
      icon: mdi:water-well

    - name: "Water Tank Percent"
      state_topic: "lora/water_sensor/water/percent"
      unit_of_measurement: "%"
      icon: mdi:water-percent

    # Battery
    - name: "Water Sensor Battery"
      state_topic: "lora/water_sensor/battery/voltage"
      unit_of_measurement: "V"
      device_class: voltage
      icon: mdi:battery

    # Signal Quality
    - name: "Water Sensor RSSI"
      state_topic: "lora/water_sensor/rssi"
      unit_of_measurement: "dBm"
      icon: mdi:signal

    - name: "Water Sensor SNR"
      state_topic: "lora/water_sensor/snr"
      unit_of_measurement: "dB"
      icon: mdi:signal-variant

    # Last Seen
    - name: "Water Sensor Last Seen"
      state_topic: "lora/water_sensor/last_seen"
      icon: mdi:clock-outline

  binary_sensor:
    # Gateway Status
    - name: "LoRa Gateway"
      state_topic: "lora/water_sensor/status"
      payload_on: "online"
      payload_off: "offline"
      device_class: connectivity
```

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
