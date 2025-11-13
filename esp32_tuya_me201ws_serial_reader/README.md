# ME201W Water Sensor LoRa Bridge

A production-ready ESP32 application that reads Tuya ME201W ultrasonic water sensors via serial and transmits data to Home Assistant via LoRa. Features advanced power management with deep sleep, button wake for diagnostics, and robust error handling.

## 🌟 Features

### Core Functionality
- ✅ **Serial Communication** - Reads Tuya ME201W sensor data via UART
- ✅ **LoRa Transmission** - Sends data to Raspberry Pi gateway at 915MHz
- ✅ **Deep Sleep Power Management** - Ultra-low power consumption between readings
- ✅ **Wake-on-Serial** - Automatically wakes when sensor transmits data
- ✅ **Button Wake** - Press PRG button to view stats on OLED display
- ✅ **Complete Data Validation** - Only transmits when full dataset received
- ✅ **JSON Payload** - Structured data for easy Home Assistant integration
- ✅ **Signal Quality Reporting** - Includes RSSI and SNR in transmissions

### Power Management
- **Deep Sleep**: ~2-5mA between sensor readings
- **Active Transmission**: ~100-120mA for <1 second
- **Display Active**: ~50-80mA for 30 seconds (button wake only)
- **Solar Compatible**: Works indefinitely with 18650 battery + solar panel

### Reliability Features
- **Reboot-to-Sleep Strategy** - Eliminates deep sleep crashes on ESP32-S3
- **Data Validation** - Tracks 4 critical fields per wake cycle
- **No Stale Data** - Never transmits cached or incomplete readings
- **Crash-Free** - Stable operation tested over extended periods

## 📋 Hardware Requirements

### Required Components
- **ESP32 Board**: Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- **Sensor**: Tuya ME201W ultrasonic water level sensor
- **Power**: 18650 battery (3.7V) with solar charging (recommended)
- **Gateway**: Raspberry Pi with [SX1262 LoRa Gateway](../sx1262_lora_gateway/)

### Wiring Connections

**ME201W Sensor → ESP32:**
- ME201W TX → ESP32 GPIO 5 (RX, configured for wake-on-serial)
- ME201W RX → ESP32 GPIO 4 (TX, optional for two-way communication)
- ME201W VCC → ESP32 3.3V or 5V (sensor operates on both)
- ME201W GND → ESP32 GND

**Power:**
- Battery: Connect to ESP32 battery terminals
- Solar Panel: Connect to charging circuit (if equipped)

**Display:**
- Built-in OLED on Heltec V3 (no additional wiring)

**Button:**
- Built-in PRG button on GPIO 0 (no additional wiring)

## 🚀 Installation

### 1. Install Arduino IDE & Dependencies

1. **Install Arduino IDE** (1.8.19 or 2.x)

2. **Add ESP32 Board Support:**
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Board Manager**
   - Search "esp32" and install "esp32 by Espressif Systems"

3. **Install Required Libraries** (Tools → Manage Libraries):
   - `Heltec ESP32 Dev-Boards` (for Heltec V3 support)
   - `RadioLib` (for LoRa SX1262 radio)
   - `ArduinoJson` (for JSON payload creation)

### 2. Configure Arduino IDE

Select these settings in **Tools** menu:
- **Board**: "WiFi LoRa 32(V3)" (under Heltec ESP32 Arduino)
- **Upload Speed**: 921600
- **CPU Frequency**: 240MHz
- **Flash Frequency**: 80MHz
- **Flash Mode**: QIO
- **Flash Size**: 8MB
- **Partition Scheme**: "Default 4MB with spiffs"
- **PSRAM**: Enabled
- **Arduino Runs On**: Core 1
- **Events Run On**: Core 1
- **Port**: Select your USB serial port

### 3. Program the ESP32

1. Open `esp32_tuya_me201ws_serial_reader.ino` in Arduino IDE
2. Review the configuration section (lines 50-100) and adjust if needed:
   ```cpp
   #define LORA_FREQUENCY 915.0       // US: 915, EU: 868
   #define LORA_SPREADING_FACTOR 7    // SF7-SF12
   #define LORA_BANDWIDTH 125.0       // 125kHz
   #define LORA_SYNC_WORD 0x12        // Must match gateway
   ```
3. Connect ESP32 via USB
4. Click **Upload** (or Ctrl+U)
5. Open **Serial Monitor** (115200 baud) to view operation

## 📡 Operation

### Normal Operation Cycle

1. **Deep Sleep** - ESP32 sleeps, consuming ~2-5mA
2. **Wake on UART** - ME201W sensor transmits data → ESP32 wakes via GPIO 5
3. **Read Sensor** - Parses serial data from ME201W (water level, battery, etc.)
4. **Validate Data** - Ensures complete dataset received (4 critical fields)
5. **Transmit LoRa** - Sends JSON payload to gateway
6. **Reboot to Sleep** - Cleans up and returns to deep sleep

### Button Wake (Diagnostics)

1. **Press PRG Button** - ESP32 wakes from deep sleep
2. **Display Activates** - OLED shows:
   - Water level (cm and %)
   - Battery voltage and level
   - Alarm state
   - Sensor statistics
3. **Auto Sleep** - Display turns off after 30 seconds, returns to deep sleep

### Initial Boot Behavior

On first power-on, the ESP32:
1. Shows boot screen briefly
2. Immediately enters deep sleep
3. Only wakes on UART activity or button press

This ensures no unnecessary power consumption.

## 📊 Data Format

### JSON Payload Structure

The ESP32 transmits this JSON structure via LoRa:

```json
{
  "water": {
    "level": 85.2,           // Water level in cm
    "percent": 76,           // Water level as percentage
    "raw_distance": 114.8,   // Raw ultrasonic distance
    "state": 0               // Alarm state (0=normal, 1=low, 2=high)
  },
  "batt": {
    "voltage": 4.15,         // Battery voltage
    "unit": 95               // Battery percentage
  },
  "thr": {
    "high": 200,             // High threshold in cm
    "low": 20                // Low threshold in cm
  },
  "stat": {
    "instHeight": 200,       // Installation height
    "sensorState": "Normal"  // Sensor status text
  }
}
```

### ME201W Serial Protocol

The ME201W sensor transmits data via serial at 9600 baud:

```
Water_Level:85.2
Percent_Level:76
Raw_Distance:114.8
Voltage:4.15
Battery_Unit:95
High_Threshold:200
Low_Threshold:20
Installation_Height:200
State:0
```

The ESP32 parses these lines and builds the JSON payload.

## 🏡 Home Assistant Integration

### MQTT Topics Created

The gateway automatically creates these MQTT topics from the JSON:

- `lora/gateway/water/level` → Water level in cm
- `lora/gateway/water/percent` → Water level percentage
- `lora/gateway/water/raw_distance` → Raw sensor distance
- `lora/gateway/water/state` → Alarm state
- `lora/gateway/batt/voltage` → Battery voltage
- `lora/gateway/batt/unit` → Battery percentage
- `lora/gateway/thr/high` → High threshold
- `lora/gateway/thr/low` → Low threshold
- `lora/gateway/stat/instHeight` → Installation height
- `lora/gateway/stat/sensorState` → Sensor status
- `lora/gateway/rssi` → Signal strength
- `lora/gateway/snr` → Signal quality

### Home Assistant Sensors

Add to your `configuration.yaml`:

```yaml
mqtt:
  sensor:
    # Water Level Monitoring
    - name: "Water Tank Level"
      state_topic: "lora/gateway/water/level"
      unit_of_measurement: "cm"
      device_class: distance
      icon: mdi:water-well
      state_class: measurement
    
    - name: "Water Tank Percent"
      state_topic: "lora/gateway/water/percent"
      unit_of_measurement: "%"
      icon: mdi:water-percent
      state_class: measurement
    
    - name: "Water Tank Raw Distance"
      state_topic: "lora/gateway/water/raw_distance"
      unit_of_measurement: "cm"
      device_class: distance
      state_class: measurement
    
    # Battery Monitoring
    - name: "Water Sensor Battery Voltage"
      state_topic: "lora/gateway/batt/voltage"
      unit_of_measurement: "V"
      device_class: voltage
      state_class: measurement
    
    - name: "Water Sensor Battery Level"
      state_topic: "lora/gateway/batt/unit"
      unit_of_measurement: "%"
      device_class: battery
      state_class: measurement
    
    # Thresholds
    - name: "Water Tank High Threshold"
      state_topic: "lora/gateway/thr/high"
      unit_of_measurement: "cm"
      icon: mdi:arrow-up-bold
    
    - name: "Water Tank Low Threshold"
      state_topic: "lora/gateway/thr/low"
      unit_of_measurement: "cm"
      icon: mdi:arrow-down-bold
    
    # Configuration
    - name: "Water Sensor Install Height"
      state_topic: "lora/gateway/stat/instHeight"
      unit_of_measurement: "cm"
      icon: mdi:ruler
    
    - name: "Water Sensor Status"
      state_topic: "lora/gateway/stat/sensorState"
      icon: mdi:information-outline
    
    # Signal Quality
    - name: "Water Sensor RSSI"
      state_topic: "lora/gateway/rssi"
      unit_of_measurement: "dBm"
      device_class: signal_strength
      state_class: measurement
    
    - name: "Water Sensor SNR"
      state_topic: "lora/gateway/snr"
      unit_of_measurement: "dB"
      icon: mdi:signal-variant
      state_class: measurement
  
  binary_sensor:
    # Alarm States
    - name: "Water Tank Low Alarm"
      state_topic: "lora/gateway/water/state"
      payload_off: "0"
      payload_on: "1"
      device_class: problem
    
    - name: "Water Tank High Alarm"
      state_topic: "lora/gateway/water/state"
      value_template: "{{ '2' if value == '2' else '0' }}"
      payload_off: "0"
      payload_on: "2"
      device_class: problem
```

After restarting Home Assistant, all sensors will be available for your dashboard and automations.

## 🔧 Configuration Options

### Key Constants (edit in code)

```cpp
// LoRa Configuration
#define LORA_FREQUENCY 915.0          // 915 for US, 868 for EU
#define LORA_SPREADING_FACTOR 7       // SF7-12 (7=fast, 12=long range)
#define LORA_BANDWIDTH 125.0          // 125kHz standard
#define LORA_CODING_RATE 5            // 4/5 standard
#define LORA_SYNC_WORD 0x12          // Must match gateway

// Power Management
#define DEEP_SLEEP_ENABLED true       // Enable/disable deep sleep
#define BUTTON_WAKE_ENABLED true      // Enable button wake
#define UART_WAKE_ENABLED true        // Enable wake-on-serial
#define DISPLAY_TIMEOUT_MS 30000      // Display timeout (30 seconds)

// Sensor Configuration
#define SENSOR_BAUD_RATE 9600         // ME201W baud rate
#define SENSOR_SERIAL_RX 5            // RX pin (GPIO 5 for wake)
#define SENSOR_SERIAL_TX 4            // TX pin (optional)
#define SENSOR_READ_TIMEOUT_MS 60000  // Wait up to 60s for sensor
```

## 🐛 Troubleshooting

### No LoRa Transmissions

**Check Serial Monitor:**
- Should see "LoRa initialized successfully"
- If "LoRa initialization failed" → check radio connections

**Verify Configuration:**
- Frequency must match gateway (915 for US)
- Sync word must match gateway (`0x12` default)
- Check antenna is connected

### Not Waking from Sleep

**UART Wake Issues:**
- Verify ME201W TX → ESP32 GPIO 5
- Sensor must be transmitting data periodically
- Check sensor has power

**Button Wake Issues:**
- PRG button is on GPIO 0 (built-in)
- Press and hold for 1-2 seconds
- Check `BUTTON_WAKE_ENABLED` is `true`

### Display Not Showing

**After Button Wake:**
- Display should activate for 30 seconds
- Check OLED connections (built-in on Heltec V3)
- Look for "displayActive = true" in serial monitor

**On First Boot:**
- Normal - goes straight to sleep on power-on
- Press button to view stats

### Incomplete or Old Data

**This should not happen** - the code validates complete data:
- Tracks 4 critical fields per wake cycle
- Only transmits when all fields received
- Never caches or reuses old values

If you see repeated values:
- Check sensor is responding (serial monitor)
- Verify sensor wiring
- Ensure sensor has power

### High Power Consumption

**Check These:**
- Display auto-sleeps after 30 seconds
- Deep sleep enabled (`DEEP_SLEEP_ENABLED true`)
- No serial monitor connected (keeps USB active)
- WiFi disabled (not used in this sketch)

### Gateway Not Receiving

**Verify LoRa Parameters Match:**
```cpp
// ESP32 (this sketch)
#define LORA_FREQUENCY 915.0
#define LORA_SPREADING_FACTOR 7
#define LORA_SYNC_WORD 0x12

// Gateway config.yaml
lora_frequency: 915.0
lora_spreading_factor: 7
lora_sync_word: 0x12
```

**Check Signal Quality:**
- Start with devices < 10 meters apart
- RSSI should be > -100 dBm for reliable reception
- Check antennas on both devices

## 📝 Serial Monitor Output

### Normal Operation
```
🌅 ========================================
🌅 Wake Event #5
🌅 Wake Reason: SERIAL (start bit)

🌅 Total: UART=5, Timer=0, Button=2
🌅 ========================================

📡 Initializing sensor serial on RX=5 @ 9600 baud
⏳ Waiting for sensor data...
📊 Sensor Data: Water_Level:85.2
📊 Sensor Data: Percent_Level:76
📊 Sensor Data: Battery_Unit:95
📊 Sensor Data: Voltage:4.15
✅ Complete data received this wake cycle
📡 Transmitting to gateway...
✅ LoRa packet sent successfully
✅ Transmission #12 complete (UART wake #5)
🔄 Rebooting to sleep...
```

### Button Wake
```
🌅 ========================================
🌅 Wake Event #6
🌅 Wake Reason: BUTTON PRESS
🌅 Total: UART=5, Timer=0, Button=3
🌅 ========================================

[Display activates, shows stats for 30 seconds]

💤 Display timeout - turning off
🔄 Rebooting to sleep...
```

## 🔋 Power Consumption Details

Measured with Heltec WiFi LoRa 32 V3:

| State | Current | Duration | Notes |
|-------|---------|----------|-------|
| Deep Sleep | 2-5 mA | 99% of time | Between sensor readings |
| Wake & Read | 80-100 mA | 1-5 seconds | Reading sensor data |
| LoRa TX | 100-120 mA | <1 second | Transmitting packet |
| Display Active | 50-80 mA | 30 seconds | Button wake only |

**Battery Life Estimate:**
- 3000mAh 18650 battery
- Sensor transmits every 30 minutes
- Without solar: ~2-3 months
- With 6V 1W solar panel: Indefinite

## 📂 File Structure

```
esp32_tuya_me201ws_serial_reader/
├── README.md                                  # This file
└── esp32_tuya_me201ws_serial_reader.ino      # Main application (~1100 lines)
```

The sketch is fully self-contained in a single .ino file for easy Arduino IDE compatibility.

## 🎯 Code Architecture

### Key Functions

- `setup()` - Initialization and wake reason detection
- `loop()` - Main sensor reading and transmission logic
- `checkWakeReason()` - Determines why ESP32 woke up
- `parseSensorLine()` - Parses ME201W serial protocol
- `transmitLoRa()` - Sends JSON payload via LoRa
- `createJSON()` - Builds JSON payload from sensor data
- `updateDisplay()` - Updates OLED with current stats
- `handleButton()` - Manages button input for display toggle

### Data Validation Strategy

The sketch tracks which fields were received during the current wake cycle:

```cpp
bool receivedThisWake_RawDistance = false;
bool receivedThisWake_Level = false;
bool receivedThisWake_Battery = false;
bool receivedThisWake_InstHeight = false;
```

Only when all 4 critical fields are received will it transmit. This eliminates the possibility of sending stale or incomplete data.

### Reboot-to-Sleep Pattern

Instead of traditional `esp_deep_sleep_start()` after full initialization (which caused LoadProhibited crashes), we use:

1. Set RTC memory magic flag: `rebootToSleepMagic = 0xDEED5EEDul`
2. Call `ESP.restart()`
3. On next boot, detect magic flag
4. Configure wake sources without initializing peripherals
5. Enter deep sleep with clean state

This eliminates all peripheral teardown crashes on ESP32-S3.

## 🤝 Contributing

This is a working production example. Contributions welcome for:
- Support for additional water sensor models
- Power consumption optimizations
- Additional wake sources
- Enhanced error handling

## 📄 License

MIT License - Free to use and modify

## 🙏 Credits

- Built for Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- Designed for Tuya ME201W ultrasonic water sensors
- Works with [SX1262 LoRa Gateway](../sx1262_lora_gateway/) for Home Assistant
- Uses RadioLib for LoRa communication
- Part of the [homeassistant-addon-sx1262](../) project
