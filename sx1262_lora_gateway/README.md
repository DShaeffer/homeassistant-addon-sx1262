# SX1262 LoRa Gateway for Home Assistant# SX1262 LoRa Gateway for Home Assistant# SX1262 LoRa Gateway Add-on for Home Assistant



A general-purpose LoRa to MQTT gateway add-on for Home Assistant. Receives LoRa packets from ESP32/Arduino devices with SX126x radios and publishes them to MQTT for easy integration with Home Assistant.



## FeaturesA general-purpose LoRa to MQTT gateway add-on for Home Assistant. Receives LoRa packets from ESP32/Arduino devices with SX126x radios and publishes them to MQTT for easy integration with Home Assistant.This add-on enables your Raspberry Pi with a Waveshare SX1262 LoRaWAN HAT to receive LoRa messages from 



- 🎯 **Universal Compatibility**: Works with any LoRa device using SX126x radios (SX1261, SX1262, SX1268)remote sensors (such as the ESP32 water sensor bridge) and integrate them into Home Assistant via MQTT. 

- 📡 **Flexible Configuration**: Full control over LoRa parameters (frequency, spreading factor, bandwidth, coding rate, sync word)

- 🔄 **Auto JSON Parsing**: Automatically publishes all JSON fields as individual MQTT topics## Features## Hardware Requirements

- 📊 **Signal Quality Monitoring**: Reports RSSI and SNR for each received packet

- 🏠 **Home Assistant Native**: Designed for seamless integration with Home Assistant

- 🔧 **Advanced Sync Word Options**: Support for precise sync word control for challenging compatibility scenarios

- 🎯 **Universal Compatibility**: Works with any LoRa device using SX126x radios (SX1261, SX1262, SX1268)- Raspberry Pi (3/4/5 or Zero 2 W)

## Hardware Requirements

- 📡 **Flexible Configuration**: Full control over LoRa parameters (frequency, spreading factor, bandwidth, coding rate, sync word)- [Waveshare SX1262 LoRaWAN HAT](https://www.waveshare.com/sx1262-lorawan-hat.htm)

- Raspberry Pi (any model with 40-pin GPIO header)

- **Waveshare SX1262 LoRa HAT** (868MHz or 915MHz depending on your region)- 🔄 **Auto JSON Parsing**: Automatically publishes all JSON fields as individual MQTT topics- Properly installed HAT on GPIO pins

- Home Assistant OS or Supervised installation

- 📊 **Signal Quality Monitoring**: Reports RSSI and SNR for each received packet

## Installation

- 🏠 **Home Assistant Native**: Designed for seamless integration with Home Assistant## Features

1. Add this repository to your Home Assistant Add-on Store:

   - Navigate to **Settings** → **Add-ons** → **Add-on Store** (three dots menu, top right) → **Repositories**- 🔧 **Advanced Sync Word Options**: Support for precise sync word control for challenging compatibility scenarios

   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`

- Receives LoRa messages on 915MHz (configurable)

2. Install the **SX1262 LoRa Gateway** add-on

## Hardware Requirements- Parses JSON sensor data from ESP32 bridge

3. Configure the add-on (see Configuration section below)

- Publishes to MQTT for Home Assistant integration

4. Start the add-on and check the logs

- Raspberry Pi (any model with 40-pin GPIO header)- Configurable LoRa parameters (SF, BW, CR, sync word)

## Configuration

- **Waveshare SX1262 LoRa HAT** (868MHz or 915MHz depending on your region)- Signal quality monitoring (RSSI, SNR)

### Basic Configuration

- Home Assistant OS or Supervised installation- Gateway statistics tracking

```yaml

lora_frequency: 915.0              # Your region's LoRa frequency (MHz)- Auto-discovery compatible MQTT topics

lora_spreading_factor: 7           # SF7-SF12 (higher = longer range, slower)

lora_bandwidth: 125000             # 125kHz typical for long range## Installation

lora_coding_rate: 5                # 4/5 coding rate

lora_sync_word: 52                 # 52 = 0x34 (private network)## Installation

lora_tx_power: 20                  # Gateway TX power (not used for RX-only)

mqtt_host: core-mosquitto          # Your MQTT broker1. Add this repository to your Home Assistant Add-on Store:

mqtt_port: 1883

mqtt_username: ""                  # Optional MQTT auth   - Navigate to **Settings** → **Add-ons** → **Add-on Store** (three dots menu, top right) → **Repositories**1. **Enable SPI on your Raspberry Pi:**

mqtt_password: ""

mqtt_topic_prefix: "lora/gateway"  # MQTT topic prefix   - Add: `https://github.com/DShaeffer/homeassistant-addon-sx1262`   - Go to Supervisor â System â Host â Hardware

log_level: "info"                  # debug, info, warning, error

```   - Click the three dots menu â "Import from USB"



### Frequency Bands by Region2. Install the **SX1262 LoRa Gateway** add-on   - Or via SSH: `raspi-config` â Interfacing Options â SPI â Enable



- **North America (US/Canada)**: 902-928 MHz (typically 915.0 MHz)

- **Europe**: 863-870 MHz (typically 868.0 MHz)

- **Asia**: 920-925 MHz (varies by country)3. Configure the add-on (see Configuration section below)2. **Add the repository to Home Assistant:**

- **Australia**: 915-928 MHz (typically 915.0 MHz)

   - Go to Supervisor â Add-on Store

**⚠️ Important**: Ensure you comply with your local regulations regarding LoRa frequencies and power limits.

4. Start the add-on and check the logs   - Click the three dots menu (top right) â Repositories

### Advanced Sync Word Configuration

   - Add this repository URL: `file:///config/addons/homeassistant-addon-sx1262`

The gateway supports multiple sync word configuration methods for compatibility with various LoRa devices:

## Configuration   - Or copy the addon folder to `/addons/` directory

**Method 1: Standard (Recommended)**

```yaml

lora_sync_word: 52  # Decimal (52 = 0x34 hex)

```### Basic Configuration3. **Install the add-on:**

This uses the standard LoRa private network sync word. The driver will transform this to register values automatically.

   - Find "SX1262 LoRa Gateway" in the add-on store

**Method 2: Force 16-bit Value**

```yaml```yaml   - Click on it and press "INSTALL"

lora_sync_word_force: "0x3424"  # Hex string with full 16-bit value

```lora_frequency: 915.0              # Your region's LoRa frequency (MHz)

Writes the exact 16-bit value directly to SX126x registers. Use this if you know the exact register values needed.

lora_spreading_factor: 7           # SF7-SF12 (higher = longer range, slower)4. **Configure the add-on:**

**Method 3: Raw Register Bytes**

```yamllora_bandwidth: 125000             # 125kHz typical for long range   - Go to the Configuration tab

lora_sync_word_msb: "0x34"  # High byte

lora_sync_word_lsb: "0x24"  # Low bytelora_coding_rate: 5                # 4/5 coding rate   - Set your LoRa parameters (must match ESP32 settings!)

```

Provides individual control over MSB and LSB register bytes.lora_sync_word: 52                 # 52 = 0x34 (private network)   - Configure MQTT broker (default: core-mosquitto)



**Why Multiple Methods?**lora_tx_power: 20                  # Gateway TX power (not used for RX-only)   - Click "SAVE"

Some LoRa libraries (like Heltec's) transform sync word values in unexpected ways. For example, `SetSyncWord(0x34)` might write `MSB=0x34, LSB=0x24` instead of `0x34, 0x34`. These advanced options let you match any transmitter precisely.

mqtt_host: core-mosquitto          # Your MQTT broker

**Troubleshooting Sync Word Issues:**

- If you're not receiving packets, try `lora_sync_word_force: "0x3424"`mqtt_port: 18835. **Start the add-on:**

- Monitor gateway logs for "Sync Word Registers: MSB=... LSB=..." to see actual values

- Match these values to your transmitter's sync word registers (0x0740/0x0741)mqtt_username: ""                  # Optional MQTT auth   - Go to the Info tab



## MQTT Topicsmqtt_password: ""   - Toggle "Start on boot" if desired



The gateway publishes received data to MQTT with automatic topic generation:mqtt_topic_prefix: "lora/gateway"  # MQTT topic prefix   - Click "START"



### Main Topicslog_level: "info"                  # debug, info, warning, error

- `{prefix}/data` - Complete JSON payload

- `{prefix}/rssi` - Packet RSSI (signal strength in dBm)```## Configuration

- `{prefix}/snr` - Packet SNR (signal-to-noise ratio in dB)

- `{prefix}/last_seen` - Timestamp of last received packet

- `{prefix}/status` - Gateway status (online/offline)

### Frequency Bands by Region### LoRa Settings

### Auto-Generated Topics

All JSON fields are automatically published as individual topics. For example, if your device sends:

```json

{- **North America (US/Canada)**: 902-928 MHz (typically 915.0 MHz)```yaml

  "temperature": 23.5,

  "humidity": 65,- **Europe**: 863-870 MHz (typically 868.0 MHz)lora_frequency: 915.0            # Frequency in MHz (902-928 for US)

  "battery": {

    "voltage": 3.95,- **Asia**: 920-925 MHz (varies by country)lora_spreading_factor: 7         # SF7-SF12 (7=fastest, 12=longest range)

    "percent": 87

  }- **Australia**: 915-928 MHz (typically 915.0 MHz)lora_bandwidth: 125000           # Bandwidth in Hz (125kHz default)

}

```lora_coding_rate: 5              # 4/5, 4/6, 4/7, 4/8



The gateway will create these topics:**⚠️ Important**: Ensure you comply with your local regulations regarding LoRa frequencies and power limits.lora_sync_word: 0x34             # Single-byte legacy request (will be transformed by Heltec / driver)

- `{prefix}/temperature` → `23.5`

- `{prefix}/humidity` → `65`# Optional advanced overrides (add only if needed):

- `{prefix}/battery/voltage` → `3.95`

- `{prefix}/battery/percent` → `87`### Advanced Sync Word Configuration# lora_sync_word_force: 0x3424   # Force a full 16-bit sync word directly



This makes it trivial to create Home Assistant sensors for any data structure!# lora_sync_word_msb: 0x34       # Raw MSB byte



## ESP32 Quickstart (Heltec Example)The gateway supports multiple sync word configuration methods for compatibility with various LoRa devices:# lora_sync_word_lsb: 0x24       # Raw LSB byte



This example works with **Heltec WiFi LoRa 32 V3** (ESP32-S3 + SX1262) and similar boards.lora_tx_power: 20                # TX power in dBm (2-22)



### Prerequisites**Method 1: Standard (Recommended)**```

1. Install [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)

2. Install **Heltec ESP32 board support** and **Heltec LoRaWan_APP** library```yaml

3. Get your Heltec license from: https://resource.heltec.cn/search

lora_sync_word: 52  # Decimal (52 = 0x34 hex)> Sync Word Note: Heltec's SX1262 ESP32 library may transform a single-byte sync request (e.g. 0x34) into two register bytes (observed MSB=0x34, LSB=0x24). If your ESP32 test sketch reads back these registers, configure the gateway to match using `lora_sync_word_force` or the raw MSB/LSB fields.

### Arduino Sketch

```

```cpp

#include "LoRaWan_APP.h"This uses the standard LoRa private network sync word. The driver will transform this to register values automatically.### MQTT Settings

#include "Arduino.h"



// ===== HELTEC LICENSE (V3 boards require this) =====

// Get your license from: https://resource.heltec.cn/search**Method 2: Force 16-bit Value**```yaml

uint32_t license[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

```yamlmqtt_host: core-mosquitto      # MQTT broker hostname

// ===== LORA CONFIGURATION (MUST MATCH GATEWAY!) =====

#define RF_FREQUENCY                915000000  // Hz (915 MHz for US)lora_sync_word_force: "0x3424"  # Hex string with full 16-bit valuemqtt_port: 1883               # MQTT broker port

#define TX_OUTPUT_POWER             14         // dBm (14 recommended for testing)

#define LORA_BANDWIDTH              0          // 0=125kHz, 1=250kHz, 2=500kHz```mqtt_username: ""             # Optional MQTT username

#define LORA_SPREADING_FACTOR       7          // SF7-SF12

#define LORA_CODINGRATE             1          // 1=4/5, 2=4/6, 3=4/7, 4=4/8Writes the exact 16-bit value directly to SX126x registers. Use this if you know the exact register values needed.mqtt_password: ""             # Optional MQTT password

#define LORA_PREAMBLE_LENGTH        8          // Symbols

#define LORA_FIX_LENGTH_PAYLOAD_ON  false      // Variable lengthmqtt_topic_prefix: "lora/water_sensor"  # Topic prefix for all messages

#define LORA_IQ_INVERSION_ON        false      // Normal IQ

**Method 3: Raw Register Bytes**```

#define TX_INTERVAL_MS              10000      // Send every 10 seconds

```yaml

// ===== GLOBALS =====

char txpacket[256];lora_sync_word_msb: "0x34"  # High byte### Logging

uint32_t packetCount = 0;

bool lora_idle = true;lora_sync_word_lsb: "0x24"  # Low byte

static RadioEvents_t RadioEvents;

``````yaml

void OnTxDone(void) {

    Serial.println("✅ TX Done");Provides individual control over MSB and LSB register bytes.log_level: info               # debug, info, warning, error

    lora_idle = true;

}```



void OnTxTimeout(void) {**Why Multiple Methods?**

    Radio.Sleep();

    Serial.println("⚠️ TX Timeout");Some LoRa libraries (like Heltec's) transform sync word values in unexpected ways. For example, `SetSyncWord(0x34)` might write `MSB=0x34, LSB=0x24` instead of `0x34, 0x34`. These advanced options let you match any transmitter precisely.## MQTT Topics

    lora_idle = true;

}



void setup() {**Troubleshooting Sync Word Issues:**The add-on publishes to the following topics:

    Serial.begin(115200);

    delay(1000);- If you're not receiving packets, try `lora_sync_word_force: "0x3424"`

    

    Serial.println("\n=== LoRa Gateway Test ===");- Monitor gateway logs for "Sync Word Registers: MSB=... LSB=..." to see actual values### Main Data Topics

    

    // IMPORTANT: Set license BEFORE Mcu.begin()- Match these values to your transmitter's sync word registers (0x0740/0x0741)- `lora/water_sensor/data` - Complete JSON payload

    Mcu.setlicense(license, HELTEC_BOARD);

    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);- `lora/water_sensor/status` - Gateway online/offline status

    

    RadioEvents.TxDone = OnTxDone;## MQTT Topics- `lora/water_sensor/last_seen` - Last message timestamp

    RadioEvents.TxTimeout = OnTxTimeout;

    

    Radio.Init(&RadioEvents);

    Radio.SetChannel(RF_FREQUENCY);The gateway publishes received data to MQTT with automatic topic generation:### Water Sensor Topics

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,

                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,- `lora/water_sensor/water/level` - Water level in cm

                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,

                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);### Main Topics- `lora/water_sensor/water/percent` - Water level percentage

    

    Serial.println("✅ Ready to transmit!");- `{prefix}/data` - Complete JSON payload- `lora/water_sensor/water/raw_distance` - Raw ultrasonic distance

}

- `{prefix}/rssi` - Packet RSSI (signal strength in dBm)- `lora/water_sensor/water/state` - Alarm state (0=normal, 1=low, 2=high)

void loop() {

    if (lora_idle) {- `{prefix}/snr` - Packet SNR (signal-to-noise ratio in dB)

        delay(TX_INTERVAL_MS);

        packetCount++;- `{prefix}/last_seen` - Timestamp of last received packet### Battery Topics

        

        // Create JSON payload (customize for your application)- `{prefix}/status` - Gateway status (online/offline)- `lora/water_sensor/battery/voltage` - Battery voltage

        snprintf(txpacket, sizeof(txpacket),

                 "{\"device\":\"heltec_test\",\"packet\":%lu,\"uptime\":%lu,\"temperature\":%.1f}",- `lora/water_sensor/battery/unit` - Battery level (0-100)

                 packetCount, millis() / 1000, 22.5 + (rand() % 30) / 10.0);

        ### Auto-Generated Topics

        Serial.printf("\n📡 TX #%lu: %s\n", packetCount, txpacket);

        All JSON fields are automatically published as individual topics. For example, if your device sends:### Signal Quality Topics

        Radio.Send((uint8_t *)txpacket, strlen(txpacket));

        lora_idle = false;```json- `lora/water_sensor/rssi` - Received Signal Strength Indicator (dBm)

    }

    {- `lora/water_sensor/snr` - Signal-to-Noise Ratio (dB)

    Radio.IrqProcess();

}  "temperature": 23.5,

```

  "humidity": 65,### Gateway Statistics

### Upload Instructions

  "battery": {- `lora/water_sensor/gateway/stats` - Gateway statistics (JSON)

1. **Update License**: Replace the `license[4]` array with your actual Heltec license

2. **Match Gateway Config**: Ensure `RF_FREQUENCY`, `LORA_SPREADING_FACTOR`, `LORA_BANDWIDTH`, etc. match your gateway configuration    "voltage": 3.95,

3. **Select Board**: Arduino IDE → Tools → Board → Heltec WiFi LoRa 32 (V3)

4. **Select Port**: Tools → Port → (your COM/tty port)    "percent": 87## Home Assistant Integration

5. **Upload**: Click Upload button

6. **Monitor**: Tools → Serial Monitor (115200 baud)  }



### Expected Output}After the add-on is running, add these sensors to your `configuration.yaml`:



**ESP32 Serial Monitor:**```

```

=== LoRa Gateway Test ===```yaml

✅ Ready to transmit!

The gateway will create these topics:mqtt:

📡 TX #1: {"device":"heltec_test","packet":1,"uptime":10,"temperature":23.4}

✅ TX Done- `{prefix}/temperature` → `23.5`  sensor:



📡 TX #2: {"device":"heltec_test","packet":2,"uptime":20,"temperature":24.1}- `{prefix}/humidity` → `65`    # Water Level

✅ TX Done

```- `{prefix}/battery/voltage` → `3.95`    - name: "Water Tank Level"



**Gateway Logs:**- `{prefix}/battery/percent` → `87`      state_topic: "lora/water_sensor/water/level"

```

INFO - ✅ LoRa RX: 78 bytes, RSSI=-45.0dBm, SNR=9.5dB      unit_of_measurement: "cm"

INFO - Published data with keys: ['device', 'packet', 'uptime', 'temperature']

```This makes it trivial to create Home Assistant sensors for any data structure!      icon: mdi:water-well



**MQTT Topics Created:**

- `lora/gateway/device` → `heltec_test`

- `lora/gateway/packet` → `1`## ESP32 Quickstart (Heltec Example)    - name: "Water Tank Percent"

- `lora/gateway/uptime` → `10`

- `lora/gateway/temperature` → `23.4`      state_topic: "lora/water_sensor/water/percent"

- `lora/gateway/rssi` → `-45.0`

- `lora/gateway/snr` → `9.5`This example works with **Heltec WiFi LoRa 32 V3** (ESP32-S3 + SX1262) and similar boards.      unit_of_measurement: "%"



## Creating Home Assistant Sensors      icon: mdi:water-percent



Once data is flowing to MQTT, create sensors in Home Assistant:### Prerequisites



### Method 1: YAML Configuration1. Install [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)    # Battery



Add to `configuration.yaml`:2. Install **Heltec ESP32 board support** and **Heltec LoRaWan_APP** library    - name: "Water Sensor Battery"

```yaml

mqtt:3. Get your Heltec license from: https://resource.heltec.cn/search      state_topic: "lora/water_sensor/battery/voltage"

  sensor:

    - name: "LoRa Temperature"      unit_of_measurement: "V"

      state_topic: "lora/gateway/temperature"

      unit_of_measurement: "°C"### Arduino Sketch      device_class: voltage

      device_class: temperature

            icon: mdi:battery

    - name: "LoRa RSSI"

      state_topic: "lora/gateway/rssi"```cpp

      unit_of_measurement: "dBm"

      device_class: signal_strength#include "LoRaWan_APP.h"    # Signal Quality

      

    - name: "LoRa SNR"#include "Arduino.h"    - name: "Water Sensor RSSI"

      state_topic: "lora/gateway/snr"

      unit_of_measurement: "dB"      state_topic: "lora/water_sensor/rssi"

```

// ===== HELTEC LICENSE (V3 boards require this) =====      unit_of_measurement: "dBm"

### Method 2: UI (MQTT Discovery)

// Get your license from: https://resource.heltec.cn/search      icon: mdi:signal

1. Go to **Settings** → **Devices & Services** → **MQTT**

2. Click **Configure**uint32_t license[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

3. Your topics should appear under "Manually configured MQTT entities"

4. Customize entity IDs and friendly names as desired    - name: "Water Sensor SNR"



## Troubleshooting// ===== LORA CONFIGURATION (MUST MATCH GATEWAY!) =====      state_topic: "lora/water_sensor/snr"



### No Packets Received#define RF_FREQUENCY                915000000  // Hz (915 MHz for US)      unit_of_measurement: "dB"



1. **Check Frequency**: Ensure ESP32 and gateway use the same frequency#define TX_OUTPUT_POWER             14         // dBm (14 recommended for testing)      icon: mdi:signal-variant

2. **Check Sync Word**: Try forcing sync word: `lora_sync_word_force: "0x3424"`

3. **Check Logs**: Gateway logs show RSSI even without valid packets (look for "RSSIinst")#define LORA_BANDWIDTH              0          // 0=125kHz, 1=250kHz, 2=500kHz

4. **Verify Spreading Factor**: Must match exactly between transmitter and receiver

5. **Check Distance**: Start with devices close together (< 10 meters)#define LORA_SPREADING_FACTOR       7          // SF7-SF12    # Last Seen



### Low Signal Quality (RSSI < -100 dBm)#define LORA_CODINGRATE             1          // 1=4/5, 2=4/6, 3=4/7, 4=4/8    - name: "Water Sensor Last Seen"



1. **Reduce Distance**: Move devices closer#define LORA_PREAMBLE_LENGTH        8          // Symbols      state_topic: "lora/water_sensor/last_seen"

2. **Increase TX Power**: Try 20 dBm (max) on transmitter

3. **Increase SF**: Higher spreading factor = longer range (try SF9 or SF10)#define LORA_FIX_LENGTH_PAYLOAD_ON  false      // Variable length      icon: mdi:clock-outline

4. **Check Antenna**: Ensure antenna is properly connected

#define LORA_IQ_INVERSION_ON        false      // Normal IQ

### JSON Parse Errors

  binary_sensor:

1. **Validate JSON**: Use an online JSON validator

2. **Check Buffer Size**: Ensure ESP32 buffer is large enough (256 bytes recommended)#define TX_INTERVAL_MS              10000      // Send every 10 seconds    # Gateway Status

3. **Enable Debug**: Set `log_level: "debug"` to see raw payload

4. **Check Encoding**: Ensure UTF-8 encoding    - name: "LoRa Gateway"



### Gateway Won't Start// ===== GLOBALS =====      state_topic: "lora/water_sensor/status"



1. **Check SPI Device**: Log should show `/dev/spidev0.0` availablechar txpacket[256];      payload_on: "online"

2. **Check Permissions**: Add-on needs `full_access: true`

3. **Check HAT Connection**: Ensure Waveshare HAT is properly seated on GPIO headeruint32_t packetCount = 0;      payload_off: "offline"

4. **Reboot**: Sometimes a full system reboot helps with device initialization

bool lora_idle = true;      device_class: connectivity

## Technical Details

static RadioEvents_t RadioEvents;```

### LoRa Parameters Explained



- **Spreading Factor (SF)**: 7-12. Higher = longer range but slower data rate

  - SF7: ~5.5 kbps, ~2 km rangevoid OnTxDone(void) {## Troubleshooting

  - SF12: ~250 bps, ~15 km range (line of sight)

      Serial.println("✅ TX Done");

- **Bandwidth (BW)**: 125, 250, or 500 kHz. Narrower = longer range but slower

  - 125 kHz: Best for long range    lora_idle = true;### Add-on won't start

  - 500 kHz: Faster data rate, shorter range

  }- Check that SPI is enabled on the Raspberry Pi

- **Coding Rate (CR)**: 4/5 to 4/8. Higher = more error correction but slower

  - 4/5: Least overhead, fastest- Verify the HAT is properly seated on GPIO pins

  - 4/8: Most error correction, slowest

void OnTxTimeout(void) {- Check logs for specific error messages

### Sync Word Deep Dive

    Radio.Sleep();

The sync word is a 16-bit value used for network isolation. Devices with different sync words won't receive each other's packets.

    Serial.println("⚠️ TX Timeout");### No messages received

**Common Values:**

- `0x3444` (LoRaWAN public network)    lora_idle = true;- Verify ESP32 is transmitting (check ESP32 Serial Monitor)

- `0x3424` (Common in Heltec devices)

- `0x1424` (LoRaWAN private network)}- Ensure frequency, SF, BW, CR match



**Heltec Quirk:** The Heltec library's `SetSyncWord(0x34)` actually writes `0x3424` to the registers, not `0x3434`. This is why the gateway offers multiple sync word configuration methods.- Read ESP32 LoRa sync registers (0x0740 & 0x0741) and mirror on gateway if they differ from expected



## Performance Tipsvoid setup() {- Check antenna connections on both devices



1. **Optimize for Your Application**:    Serial.begin(115200);- Verify frequency is correct for your region (915MHz for US)

   - Short range, high speed: SF7, BW 500kHz

   - Long range, low power: SF10-12, BW 125kHz    delay(1000);- Enable `debug` log level to see RSSI instant values; if RSSI stays near -120 dBm you may be off-frequency or disconnected



2. **Battery Optimization** (ESP32):    

   - Reduce TX power (14 dBm instead of 20 dBm if signal is strong)

   - Increase TX interval (send less frequently)    Serial.println("\n=== LoRa Gateway Test ===");### MQTT not working

   - Use deep sleep between transmissions

    - Verify Mosquitto broker add-on is installed and running

3. **Reliability**:

   - Add packet counters to detect missed packets    // IMPORTANT: Set license BEFORE Mcu.begin()- Check MQTT credentials if authentication is enabled

   - Include timestamp in payload

   - Monitor RSSI/SNR trends    Mcu.setlicense(license, HELTEC_BOARD);- Test MQTT connection with MQTT Explorer



## Support & Contributing    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);



- **Issues**: https://github.com/DShaeffer/homeassistant-addon-sx1262/issues    ### Poor signal quality

- **Discussions**: https://github.com/DShaeffer/homeassistant-addon-sx1262/discussions

- **Pull Requests**: Welcome!    RadioEvents.TxDone = OnTxDone;- Move devices closer together initially to verify communication



## License    RadioEvents.TxTimeout = OnTxTimeout;- Check antenna orientation (vertical is best for omni-directional)



MIT License - see repository for full license text.    - Increase spreading factor (reduces speed but increases range)



## Credits    Radio.Init(&RadioEvents);- Confirm gateway logs show correct sync word register bytes



- Built with [LoRaRF Python Library](https://github.com/chandrawi/LoRaRF-Python)    Radio.SetChannel(RF_FREQUENCY);- Check for interference sources (WiFi, other 915MHz devices)

- Designed for Waveshare SX1262 LoRa HAT

- Compatible with Heltec ESP32 boards and many others    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,



## Version History                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,## Support



See [CHANGELOG.md](CHANGELOG.md) for version history.                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,


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
