# Heltec ESP32 LoRa Gateway Test Example

A minimal, ready-to-use example sketch for testing the SX1262 LoRa Gateway Home Assistant add-on.

## Compatible Hardware

- Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- Heltec WiFi LoRa 32 V2
- Heltec Wireless Stick series
- Similar ESP32 + SX126x boards

## Quick Start

### 1. Get Your Heltec License

Heltec V3 boards require a license for LoRa functionality:

1. Visit: https://resource.heltec.cn/search
2. Enter your board's Chip ID (shown in Arduino Serial Monitor on first boot without license)
3. Copy the four 32-bit hex values provided
4. Update the `license[4]` array in the sketch:

```cpp
uint32_t license[4] = {0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788};
```

### 2. Install Arduino Environment

**Arduino IDE:**
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add Heltec board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/x.x.x/package_heltec_esp32_index.json`
   - (Check [Heltec's official docs](https://heltec-automation-docs.readthedocs.io/) for latest URL)
3. Tools → Board → Boards Manager → Search "Heltec ESP32" → Install
4. Install library: Sketch → Include Library → Manage Libraries → Search "Heltec ESP32" → Install "Heltec ESP32 Dev-Boards"

**PlatformIO:**
Add to `platformio.ini`:
```ini
[env:heltec_wifi_lora_32_V3]
platform = espressif32
board = heltec_wifi_lora_32_V3
framework = arduino
lib_deps = 
    heltecautomation/Heltec ESP32 Dev-Boards
```

### 3. Configure LoRa Parameters

**IMPORTANT**: LoRa parameters must match your gateway configuration exactly!

Open `Heltec_Gateway_Test.ino` and set these values to match your gateway:

```cpp
#define RF_FREQUENCY           915000000  // 915 MHz for US, 868 MHz for EU
#define TX_OUTPUT_POWER        14         // Start with 14 dBm
#define LORA_BANDWIDTH         0          // 0 = 125 kHz
#define LORA_SPREADING_FACTOR  7          // SF7 (fastest)
#define LORA_CODINGRATE        1          // 4/5 (least overhead)
```

### 4. Upload and Test

1. Connect your Heltec board via USB
2. Select correct board: Tools → Board → Heltec WiFi LoRa 32 (V3)
3. Select port: Tools → Port → (your COM/tty port)
4. Click Upload
5. Open Serial Monitor (115200 baud)

### 5. Expected Output

**Serial Monitor:**
```
========================================
  Heltec ESP32 LoRa Gateway Test
  WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
========================================

Setting Heltec license...
Initializing MCU...
Initializing SX1262 radio...

📡 LoRa Configuration:
   Frequency:     915 MHz
   TX Power:      14 dBm
   Spreading Factor: SF7
   Bandwidth:     125 kHz
   Coding Rate:   4/5
   Preamble:      8 symbols
========================================

✅ Setup complete!
Transmitting test packets every 10 seconds

📡 ========================================
📡 TX Packet #1
📡 ========================================
Payload: {"device":"heltec_test","packet":1,"uptime":10,"temperature":23.4,"humidity":52,"rssi":0}
Length:  91 bytes
Transmitting...
✅ TX Done
```

**Gateway Logs (Home Assistant):**
```
INFO - ✅ LoRa RX: 91 bytes, RSSI=-42.0dBm, SNR=10.5dB
INFO - Published data with keys: ['device', 'packet', 'uptime', 'temperature', 'humidity']
```

**MQTT Topics Created:**
- `lora/gateway/device` → `heltec_test`
- `lora/gateway/packet` → `1`
- `lora/gateway/uptime` → `10`
- `lora/gateway/temperature` → `23.4`
- `lora/gateway/humidity` → `52`
- `lora/gateway/rssi` → `-42.0`
- `lora/gateway/snr` → `10.5`

## Customizing the Payload

Edit the `snprintf()` call in `loop()` to send your own data:

```cpp
// Example: Add a battery voltage reading
float batteryVoltage = analogRead(ADC_PIN) * 3.3 / 4095.0;

snprintf(txpacket, sizeof(txpacket),
         "{\"device\":\"my_sensor\","
         "\"temperature\":%.1f,"
         "\"battery\":%.2f}",
         readTemperature(),
         batteryVoltage);
```

The gateway will automatically create MQTT topics for all JSON fields!

## Troubleshooting

### "TX Timeout" Messages

- **Cause**: Radio not initializing properly
- **Fix**: 
  - Check license is correct
  - Verify antenna is connected
  - Try reducing TX power to 10 dBm

### Gateway Not Receiving

1. **Verify Parameters Match**: Check gateway config matches sketch
2. **Check Frequency**: US=915MHz, EU=868MHz
3. **Test Close Range**: Start with devices < 5 meters apart
4. **Check Gateway Logs**: Look for "RSSIinst" in heartbeat messages (shows RF activity even without valid packets)
5. **Try Sync Word Override**: In gateway config, add:
   ```yaml
   lora_sync_word_force: "0x3424"
   ```

### License Errors

- **"license check fail" or similar**: Get new license from Heltec site
- **Old License Format**: V3 boards need the new 4-value format (not the old string format)
- **Board Version Mismatch**: Ensure you selected the correct board variant (V2 vs V3)

### Weak Signal (RSSI < -100 dBm)

1. Check antenna connection
2. Increase TX power to 20 dBm: `#define TX_OUTPUT_POWER 20`
3. Reduce distance between devices
4. Move away from metal objects and walls

### No Serial Output

1. Check baud rate is set to 115200
2. Try pressing the RESET button on the board
3. Ensure USB cable supports data (not just power)
4. Check correct COM port is selected

## Performance Tuning

### For Maximum Range
```cpp
#define TX_OUTPUT_POWER        20    // Maximum power
#define LORA_BANDWIDTH         0     // 125 kHz
#define LORA_SPREADING_FACTOR  10    // SF10 or higher
#define LORA_CODINGRATE        4     // 4/8 (most error correction)
```
Expected: 5-15 km line of sight, slower data rate

### For Fast Data Rate
```cpp
#define TX_OUTPUT_POWER        14    // Moderate power
#define LORA_BANDWIDTH         2     // 500 kHz
#define LORA_SPREADING_FACTOR  7     // SF7
#define LORA_CODINGRATE        1     // 4/5
```
Expected: 1-2 km range, faster transmission

### For Battery Life
```cpp
#define TX_OUTPUT_POWER        10    // Lower power
#define TX_INTERVAL_MS         60000 // Send every 60 seconds
```
Add deep sleep between transmissions for ultra-low power

## Integration with Home Assistant

Once packets are flowing, create sensors in Home Assistant:

### Option 1: YAML Configuration

Add to `configuration.yaml`:
```yaml
mqtt:
  sensor:
    - name: "LoRa Temperature"
      state_topic: "lora/gateway/temperature"
      unit_of_measurement: "°C"
      device_class: temperature
      
    - name: "LoRa Humidity"
      state_topic: "lora/gateway/humidity"
      unit_of_measurement: "%"
      device_class: humidity
      
    - name: "LoRa Signal Strength"
      state_topic: "lora/gateway/rssi"
      unit_of_measurement: "dBm"
      device_class: signal_strength
```

### Option 2: UI Configuration

1. Settings → Devices & Services → MQTT → Configure
2. Find your topics under "Manually configured MQTT entities"
3. Add sensors with custom names and icons

## Next Steps

1. ✅ Confirm packets are being received
2. 🔧 Customize JSON payload for your sensors
3. 📊 Create Home Assistant dashboards
4. ⚡ Add deep sleep for battery operation
5. 📡 Deploy multiple sensor nodes
6. 🎯 Experiment with range testing

## Support

- **Gateway Add-on**: https://github.com/DShaeffer/homeassistant-addon-sx1262
- **Heltec Docs**: https://heltec-automation-docs.readthedocs.io/
- **Issues**: https://github.com/DShaeffer/homeassistant-addon-sx1262/issues

## License

MIT License - Free to use and modify
