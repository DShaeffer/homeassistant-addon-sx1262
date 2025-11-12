# SX1262 LoRa Gateway for Home Assistant# SX1262 LoRa Gateway Add-on for Home Assistant



A general-purpose LoRa to MQTT gateway add-on for Home Assistant. Receives LoRa packets from ESP32/Arduino devices with SX126x radios and publishes them to MQTT for easy integration with Home Assistant.This add-on enables your Raspberry Pi with a Waveshare SX1262 LoRaWAN HAT to receive LoRa messages from 

remote sensors (such as the ESP32 water sensor bridge) and integrate them into Home Assistant via MQTT. 

## Features## Hardware Requirements



- 🎯 **Universal Compatibility**: Works with any LoRa device using SX126x radios (SX1261, SX1262, SX1268)- Raspberry Pi (3/4/5 or Zero 2 W)

- 📡 **Flexible Configuration**: Full control over LoRa parameters (frequency, spreading factor, bandwidth, coding rate, sync word)- [Waveshare SX1262 LoRaWAN HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm)

- 🔄 **Auto JSON Parsing**: Automatically publishes all JSON fields as individual MQTT topics- Properly installed HAT on GPIO pins

- 📊 **Signal Quality Monitoring**: Reports RSSI and SNR for each received packet

- 🏠 **Home Assistant Native**: Designed for seamless integration with Home Assistant## Features

- 🔧 **Advanced Sync Word Options**: Support for precise sync word control for challenging compatibility scenarios

- Receives LoRa messages on 915MHz (configurable)

## Hardware Requirements- Parses JSON sensor data from ESP32 bridge

- Publishes to MQTT for Home Assistant integration

- Raspberry Pi (any model with 40-pin GPIO header)- Configurable LoRa parameters (SF, BW, CR, sync word)

- **Waveshare SX1262 LoRa HAT** (868MHz or 915MHz depending on your region)- Signal quality monitoring (RSSI, SNR)

- Home Assistant OS or Supervised installation- Gateway statistics tracking

- Auto-discovery compatible MQTT topics

## Installation

## Installation

1. Add this repository to your Home Assistant Add-on Store:

   - Navigate to **Settings** → **Add-ons** → **Add-on Store** (three dots menu, top right) → **Repositories**1. **Enable SPI on your Raspberry Pi:**

   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`   - Go to Supervisor â System â Host â Hardware

   - Click the three dots menu â "Import from USB"

2. Install the **SX1262 LoRa Gateway** add-on   - Or via SSH: `raspi-config` â Interfacing Options â SPI â Enable



3. Configure the add-on (see Configuration section below)2. **Add the repository to Home Assistant:**

   - Go to Supervisor â Add-on Store

4. Start the add-on and check the logs   - Click the three dots menu (top right) â Repositories

   - Add this repository URL: `file:///config/addons/homeassistant-addon-sx1262`

## Configuration   - Or copy the addon folder to `/addons/` directory



### Basic Configuration3. **Install the add-on:**

   - Find "SX1262 LoRa Gateway" in the add-on store

```yaml   - Click on it and press "INSTALL"

lora_frequency: 915.0              # Your region's LoRa frequency (MHz)

lora_spreading_factor: 7           # SF7-SF12 (higher = longer range, slower)4. **Configure the add-on:**

lora_bandwidth: 125000             # 125kHz typical for long range   - Go to the Configuration tab

lora_coding_rate: 5                # 4/5 coding rate   - Set your LoRa parameters (must match ESP32 settings!)

lora_sync_word: 52                 # 52 = 0x34 (private network)   - Configure MQTT broker (default: core-mosquitto)

lora_tx_power: 20                  # Gateway TX power (not used for RX-only)   - Click "SAVE"

mqtt_host: core-mosquitto          # Your MQTT broker

mqtt_port: 18835. **Start the add-on:**

mqtt_username: ""                  # Optional MQTT auth   - Go to the Info tab

mqtt_password: ""   - Toggle "Start on boot" if desired

mqtt_topic_prefix: "lora/gateway"  # MQTT topic prefix   - Click "START"

log_level: "info"                  # debug, info, warning, error

```## Configuration



### Frequency Bands by Region### LoRa Settings



- **North America (US/Canada)**: 902-928 MHz (typically 915.0 MHz)```yaml

- **Europe**: 863-870 MHz (typically 868.0 MHz)lora_frequency: 915.0            # Frequency in MHz (902-928 for US)

- **Asia**: 920-925 MHz (varies by country)lora_spreading_factor: 7         # SF7-SF12 (7=fastest, 12=longest range)

- **Australia**: 915-928 MHz (typically 915.0 MHz)lora_bandwidth: 125000           # Bandwidth in Hz (125kHz default)

lora_coding_rate: 5              # 4/5, 4/6, 4/7, 4/8

**⚠️ Important**: Ensure you comply with your local regulations regarding LoRa frequencies and power limits.lora_sync_word: 0x34             # Single-byte legacy request (will be transformed by Heltec / driver)

# Optional advanced overrides (add only if needed):

### Advanced Sync Word Configuration# lora_sync_word_force: 0x3424   # Force a full 16-bit sync word directly

# lora_sync_word_msb: 0x34       # Raw MSB byte

The gateway supports multiple sync word configuration methods for compatibility with various LoRa devices:# lora_sync_word_lsb: 0x24       # Raw LSB byte

lora_tx_power: 20                # TX power in dBm (2-22)

**Method 1: Standard (Recommended)**```

```yaml

lora_sync_word: 52  # Decimal (52 = 0x34 hex)> Sync Word Note: Heltec's SX1262 ESP32 library may transform a single-byte sync request (e.g. 0x34) into two register bytes (observed MSB=0x34, LSB=0x24). If your ESP32 test sketch reads back these registers, configure the gateway to match using `lora_sync_word_force` or the raw MSB/LSB fields.

```

This uses the standard LoRa private network sync word. The driver will transform this to register values automatically.### MQTT Settings



**Method 2: Force 16-bit Value**```yaml

```yamlmqtt_host: core-mosquitto      # MQTT broker hostname

lora_sync_word_force: "0x3424"  # Hex string with full 16-bit valuemqtt_port: 1883               # MQTT broker port

```mqtt_username: ""             # Optional MQTT username

Writes the exact 16-bit value directly to SX126x registers. Use this if you know the exact register values needed.mqtt_password: ""             # Optional MQTT password

mqtt_topic_prefix: "lora/water_sensor"  # Topic prefix for all messages

**Method 3: Raw Register Bytes**```

```yaml

lora_sync_word_msb: "0x34"  # High byte### Logging

lora_sync_word_lsb: "0x24"  # Low byte

``````yaml

Provides individual control over MSB and LSB register bytes.log_level: info               # debug, info, warning, error

```

**Why Multiple Methods?**

Some LoRa libraries (like Heltec's) transform sync word values in unexpected ways. For example, `SetSyncWord(0x34)` might write `MSB=0x34, LSB=0x24` instead of `0x34, 0x34`. These advanced options let you match any transmitter precisely.## MQTT Topics



**Troubleshooting Sync Word Issues:**The add-on publishes to the following topics:

- If you're not receiving packets, try `lora_sync_word_force: "0x3424"`

- Monitor gateway logs for "Sync Word Registers: MSB=... LSB=..." to see actual values### Main Data Topics

- Match these values to your transmitter's sync word registers (0x0740/0x0741)- `lora/water_sensor/data` - Complete JSON payload

- `lora/water_sensor/status` - Gateway online/offline status

## MQTT Topics- `lora/water_sensor/last_seen` - Last message timestamp



The gateway publishes received data to MQTT with automatic topic generation:### Water Sensor Topics

- `lora/water_sensor/water/level` - Water level in cm

### Main Topics- `lora/water_sensor/water/percent` - Water level percentage

- `{prefix}/data` - Complete JSON payload- `lora/water_sensor/water/raw_distance` - Raw ultrasonic distance

- `{prefix}/rssi` - Packet RSSI (signal strength in dBm)- `lora/water_sensor/water/state` - Alarm state (0=normal, 1=low, 2=high)

- `{prefix}/snr` - Packet SNR (signal-to-noise ratio in dB)

- `{prefix}/last_seen` - Timestamp of last received packet### Battery Topics

- `{prefix}/status` - Gateway status (online/offline)- `lora/water_sensor/battery/voltage` - Battery voltage

- `lora/water_sensor/battery/unit` - Battery level (0-100)

### Auto-Generated Topics

All JSON fields are automatically published as individual topics. For example, if your device sends:### Signal Quality Topics

```json- `lora/water_sensor/rssi` - Received Signal Strength Indicator (dBm)

{- `lora/water_sensor/snr` - Signal-to-Noise Ratio (dB)

  "temperature": 23.5,

  "humidity": 65,### Gateway Statistics

  "battery": {- `lora/water_sensor/gateway/stats` - Gateway statistics (JSON)

    "voltage": 3.95,

    "percent": 87## Home Assistant Integration

  }

}After the add-on is running, add these sensors to your `configuration.yaml`:

```

```yaml

The gateway will create these topics:mqtt:

- `{prefix}/temperature` → `23.5`  sensor:

- `{prefix}/humidity` → `65`    # Water Level

- `{prefix}/battery/voltage` → `3.95`    - name: "Water Tank Level"

- `{prefix}/battery/percent` → `87`      state_topic: "lora/water_sensor/water/level"

      unit_of_measurement: "cm"

This makes it trivial to create Home Assistant sensors for any data structure!      icon: mdi:water-well



## ESP32 Quickstart (Heltec Example)    - name: "Water Tank Percent"

      state_topic: "lora/water_sensor/water/percent"

This example works with **Heltec WiFi LoRa 32 V3** (ESP32-S3 + SX1262) and similar boards.      unit_of_measurement: "%"

      icon: mdi:water-percent

### Prerequisites

1. Install [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)    # Battery

2. Install **Heltec ESP32 board support** and **Heltec LoRaWan_APP** library    - name: "Water Sensor Battery"

3. Get your Heltec license from: https://resource.heltec.cn/search      state_topic: "lora/water_sensor/battery/voltage"

      unit_of_measurement: "V"

### Arduino Sketch      device_class: voltage

      icon: mdi:battery

```cpp

#include "LoRaWan_APP.h"    # Signal Quality

#include "Arduino.h"    - name: "Water Sensor RSSI"

      state_topic: "lora/water_sensor/rssi"

// ===== HELTEC LICENSE (V3 boards require this) =====      unit_of_measurement: "dBm"

// Get your license from: https://resource.heltec.cn/search      icon: mdi:signal

uint32_t license[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

    - name: "Water Sensor SNR"

// ===== LORA CONFIGURATION (MUST MATCH GATEWAY!) =====      state_topic: "lora/water_sensor/snr"

#define RF_FREQUENCY                915000000  // Hz (915 MHz for US)      unit_of_measurement: "dB"

#define TX_OUTPUT_POWER             14         // dBm (14 recommended for testing)      icon: mdi:signal-variant

#define LORA_BANDWIDTH              0          // 0=125kHz, 1=250kHz, 2=500kHz

#define LORA_SPREADING_FACTOR       7          // SF7-SF12    # Last Seen

#define LORA_CODINGRATE             1          // 1=4/5, 2=4/6, 3=4/7, 4=4/8    - name: "Water Sensor Last Seen"

#define LORA_PREAMBLE_LENGTH        8          // Symbols      state_topic: "lora/water_sensor/last_seen"

#define LORA_FIX_LENGTH_PAYLOAD_ON  false      // Variable length      icon: mdi:clock-outline

#define LORA_IQ_INVERSION_ON        false      // Normal IQ

  binary_sensor:

#define TX_INTERVAL_MS              10000      // Send every 10 seconds    # Gateway Status

    - name: "LoRa Gateway"

// ===== GLOBALS =====      state_topic: "lora/water_sensor/status"

char txpacket[256];      payload_on: "online"

uint32_t packetCount = 0;      payload_off: "offline"

bool lora_idle = true;      device_class: connectivity

static RadioEvents_t RadioEvents;```



void OnTxDone(void) {## Troubleshooting

    Serial.println("✅ TX Done");

    lora_idle = true;### Add-on won't start

}- Check that SPI is enabled on the Raspberry Pi

- Verify the HAT is properly seated on GPIO pins

void OnTxTimeout(void) {- Check logs for specific error messages

    Radio.Sleep();

    Serial.println("⚠️ TX Timeout");### No messages received

    lora_idle = true;- Verify ESP32 is transmitting (check ESP32 Serial Monitor)

}- Ensure frequency, SF, BW, CR match

- Read ESP32 LoRa sync registers (0x0740 & 0x0741) and mirror on gateway if they differ from expected

void setup() {- Check antenna connections on both devices

    Serial.begin(115200);- Verify frequency is correct for your region (915MHz for US)

    delay(1000);- Enable `debug` log level to see RSSI instant values; if RSSI stays near -120 dBm you may be off-frequency or disconnected

    

    Serial.println("\n=== LoRa Gateway Test ===");### MQTT not working

    - Verify Mosquitto broker add-on is installed and running

    // IMPORTANT: Set license BEFORE Mcu.begin()- Check MQTT credentials if authentication is enabled

    Mcu.setlicense(license, HELTEC_BOARD);- Test MQTT connection with MQTT Explorer

    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    ### Poor signal quality

    RadioEvents.TxDone = OnTxDone;- Move devices closer together initially to verify communication

    RadioEvents.TxTimeout = OnTxTimeout;- Check antenna orientation (vertical is best for omni-directional)

    - Increase spreading factor (reduces speed but increases range)

    Radio.Init(&RadioEvents);- Confirm gateway logs show correct sync word register bytes

    Radio.SetChannel(RF_FREQUENCY);- Check for interference sources (WiFi, other 915MHz devices)

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,

                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,## Support

                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,

                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);For issues specific to:

    - **The add-on**: Check add-on logs in Home Assistant

    Serial.println("✅ Ready to transmit!");- **The Waveshare HAT**: Refer to [Waveshare documentation](https://www.waveshare.com/wiki/SX1262_LoRaWA

}N_HAT)

- **Home Assistant**: Visit [Home Assistant forums](https://community.home-assistant.io/)

void loop() {

    if (lora_idle) {## License

        delay(TX_INTERVAL_MS);

        packetCount++;MIT License - feel free to modify and distribute.

        

        // Create JSON payload (customize for your application)## Version History

        snprintf(txpacket, sizeof(txpacket),

                 "{\"device\":\"heltec_test\",\"packet\":%lu,\"uptime\":%lu,\"temperature\":%.1f}",See CHANGELOG.md for version history.

                 packetCount, millis() / 1000, 22.5 + (rand() % 30) / 10.0);
        
        Serial.printf("\n📡 TX #%lu: %s\n", packetCount, txpacket);
        
        Radio.Send((uint8_t *)txpacket, strlen(txpacket));
        lora_idle = false;
    }
    
    Radio.IrqProcess();
}
```

### Upload Instructions

1. **Update License**: Replace the `license[4]` array with your actual Heltec license
2. **Match Gateway Config**: Ensure `RF_FREQUENCY`, `LORA_SPREADING_FACTOR`, `LORA_BANDWIDTH`, etc. match your gateway configuration
3. **Select Board**: Arduino IDE → Tools → Board → Heltec WiFi LoRa 32 (V3)
4. **Select Port**: Tools → Port → (your COM/tty port)
5. **Upload**: Click Upload button
6. **Monitor**: Tools → Serial Monitor (115200 baud)

### Expected Output

**ESP32 Serial Monitor:**
```
=== LoRa Gateway Test ===
✅ Ready to transmit!

📡 TX #1: {"device":"heltec_test","packet":1,"uptime":10,"temperature":23.4}
✅ TX Done

📡 TX #2: {"device":"heltec_test","packet":2,"uptime":20,"temperature":24.1}
✅ TX Done
```

**Gateway Logs:**
```
INFO - ✅ LoRa RX: 78 bytes, RSSI=-45.0dBm, SNR=9.5dB
INFO - Published data with keys: ['device', 'packet', 'uptime', 'temperature']
```

**MQTT Topics Created:**
- `lora/gateway/device` → `heltec_test`
- `lora/gateway/packet` → `1`
- `lora/gateway/uptime` → `10`
- `lora/gateway/temperature` → `23.4`
- `lora/gateway/rssi` → `-45.0`
- `lora/gateway/snr` → `9.5`

## Creating Home Assistant Sensors

Once data is flowing to MQTT, create sensors in Home Assistant:

### Method 1: YAML Configuration

Add to `configuration.yaml`:
```yaml
mqtt:
  sensor:
    - name: "LoRa Temperature"
      state_topic: "lora/gateway/temperature"
      unit_of_measurement: "°C"
      device_class: temperature
      
    - name: "LoRa RSSI"
      state_topic: "lora/gateway/rssi"
      unit_of_measurement: "dBm"
      device_class: signal_strength
      
    - name: "LoRa SNR"
      state_topic: "lora/gateway/snr"
      unit_of_measurement: "dB"
```

### Method 2: UI (MQTT Discovery)

1. Go to **Settings** → **Devices & Services** → **MQTT**
2. Click **Configure**
3. Your topics should appear under "Manually configured MQTT entities"
4. Customize entity IDs and friendly names as desired

## Troubleshooting

### No Packets Received

1. **Check Frequency**: Ensure ESP32 and gateway use the same frequency
2. **Check Sync Word**: Try forcing sync word: `lora_sync_word_force: "0x3424"`
3. **Check Logs**: Gateway logs show RSSI even without valid packets (look for "RSSIinst")
4. **Verify Spreading Factor**: Must match exactly between transmitter and receiver
5. **Check Distance**: Start with devices close together (< 10 meters)

### Low Signal Quality (RSSI < -100 dBm)

1. **Reduce Distance**: Move devices closer
2. **Increase TX Power**: Try 20 dBm (max) on transmitter
3. **Increase SF**: Higher spreading factor = longer range (try SF9 or SF10)
4. **Check Antenna**: Ensure antenna is properly connected

### JSON Parse Errors

1. **Validate JSON**: Use an online JSON validator
2. **Check Buffer Size**: Ensure ESP32 buffer is large enough (256 bytes recommended)
3. **Enable Debug**: Set `log_level: "debug"` to see raw payload
4. **Check Encoding**: Ensure UTF-8 encoding

### Gateway Won't Start

1. **Check SPI Device**: Log should show `/dev/spidev0.0` available
2. **Check Permissions**: Add-on needs `full_access: true`
3. **Check HAT Connection**: Ensure Waveshare HAT is properly seated on GPIO header
4. **Reboot**: Sometimes a full system reboot helps with device initialization

## Technical Details

### LoRa Parameters Explained

- **Spreading Factor (SF)**: 7-12. Higher = longer range but slower data rate
  - SF7: ~5.5 kbps, ~2 km range
  - SF12: ~250 bps, ~15 km range (line of sight)
  
- **Bandwidth (BW)**: 125, 250, or 500 kHz. Narrower = longer range but slower
  - 125 kHz: Best for long range
  - 500 kHz: Faster data rate, shorter range
  
- **Coding Rate (CR)**: 4/5 to 4/8. Higher = more error correction but slower
  - 4/5: Least overhead, fastest
  - 4/8: Most error correction, slowest

### Sync Word Deep Dive

The sync word is a 16-bit value used for network isolation. Devices with different sync words won't receive each other's packets.

**Common Values:**
- `0x3444` (LoRaWAN public network)
- `0x3424` (Common in Heltec devices)
- `0x1424` (LoRaWAN private network)

**Heltec Quirk:** The Heltec library's `SetSyncWord(0x34)` actually writes `0x3424` to the registers, not `0x3434`. This is why the gateway offers multiple sync word configuration methods.

## Performance Tips

1. **Optimize for Your Application**:
   - Short range, high speed: SF7, BW 500kHz
   - Long range, low power: SF10-12, BW 125kHz

2. **Battery Optimization** (ESP32):
   - Reduce TX power (14 dBm instead of 20 dBm if signal is strong)
   - Increase TX interval (send less frequently)
   - Use deep sleep between transmissions

3. **Reliability**:
   - Add packet counters to detect missed packets
   - Include timestamp in payload
   - Monitor RSSI/SNR trends

## Support & Contributing

- **Issues**: https://github.com/DShaeffer/homeassistant-addon-sx1262/issues
- **Discussions**: https://github.com/DShaeffer/homeassistant-addon-sx1262/discussions
- **Pull Requests**: Welcome!

## License

MIT License - see repository for full license text.

## Credits

- Built with [LoRaRF Python Library](https://github.com/chandrawi/LoRaRF-Python)
- Designed for Waveshare SX1262 LoRa HAT
- Compatible with Heltec ESP32 boards and many others

## Version History

See [CHANGELOG.md](CHANGELOG.md) for version history.
