/**
 * ESP32 LoRa Water Sensor Bridge - Serial Direct Read
 * 
 * Hardware: Heltec WiFi LoRa 32 V3
 * Purpose: Read ME201W water sensor via direct serial connection, transmit via 915MHz LoRa
 * 
 * Features:
 * - Direct UART connection to ME201W sensor (115200 baud)
 * - Parse sensor debug output into structured JSON
 * - 915MHz LoRa transmission to Raspberry Pi
 * - OLED display with sensor data
 * - Power management for battery operation (ME201W solar panel with 18650 batteries)
 * - Button controls and statistics tracking
 * 
 * Connections (IMPORTANT - READ THIS):
 * 
 * FOR TESTING WITH USB (Serial Monitor):
 *   - ME201W GND   → ESP32 GND  (MUST connect GND!)
 *   - ME201W TX    → ESP32 GPIO 5 (RX)
 *   - ME201W 5V    → DISCONNECTED (do not connect power when USB is plugged in!)
 *   - ESP32 USB    → Computer (provides power)
 * 
 * FOR DEPLOYMENT (Solar/Battery):
 *   - ME201W GND   → ESP32 GND
 *   - ME201W TX    → ESP32 GPIO 5 (RX)
 *   - ME201W 5V    → ESP32 5V pin (both powered by ME201W solar panel system)
 *   - ESP32 USB    → DISCONNECTED
 * 
 * BEFORE UPLOADING:
 *   Get your Heltec license from: https://resource.heltec.cn/search
 *   Update the license[4] array below with your board-specific values
 * 
 * Author: Hillsville Cabin Project
 * Date: November 2025
 */

// Board-specific defines for Heltec WiFi LoRa 32 V3
#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include <SPI.h>
#include <Wire.h>
#include "HT_SSD1306Wire.h"  // Heltec OLED library
#include <Preferences.h>
#include "LoRaWan_APP.h"      // Provides Vext power control and board initialization
#include <ArduinoJson.h>      // For JSON creation

// ============================================================================
// HELTEC LICENSE - REQUIRED FOR V3 BOARDS
// ============================================================================
// Get your license from: https://resource.heltec.cn/search
// Enter your chip ID and it will generate these 4 values
uint32_t license[4] = {0x3521755D, 0xB0401FFE, 0xA7307FF6, 0xDDFE8E4E};
// ============================================================================
// CONFIGURATION
// ============================================================================

// Power Management - Deep Sleep Configuration
// IMPORTANT: For solar/battery operation, keep DEEP_SLEEP_ENABLED true!
// The ME201W solar panel with 18650 batteries can only sustain this if ESP32 sleeps most of the time.
// NOTE: ME201W wakes every ~45 seconds to transmit data
// Strategy: Wake on UART activity (most power efficient!) or timer as backup
#define DEEP_SLEEP_ENABLED true          // Enable deep sleep for battery operation
#define UART_WAKE_ENABLED true           // Attempt wake on serial start bit (requires RTC IO pin & EXT0)
#define SLEEP_INTERVAL_SECONDS 0         // 0 = no timer wake; only wake on serial activity
#define SENSOR_READ_TIMEOUT_MS 10000     // Read for 10 seconds after wake (sensor transmits for ~5s)
#define BUTTON_WAKE_ENABLED false        // Disable button wake to save power (no display polling)

// Serial connection to ME201W sensor
// IMPORTANT: For deep sleep wake on serial, RX must be an RTC IO capable pin.
// Rewire ME201W TX to GPIO 5 (RTC IO). Idle HIGH, start bit drives LOW -> EXT0 wake.
#define SENSOR_SERIAL_RX 5        // Using GPIO5 to allow EXT0 deep sleep wake on start bit
#define SENSOR_SERIAL_TX -1       // TX not needed (read-only), use -1
#define SENSOR_BAUD_RATE 115200

// LoRa (915MHz) - Using Heltec Radio API for SX1262
#define ENABLE_LORA 1                 // Set to 1 to enable LoRa transmission
#define LORA_FREQUENCY 915000000      // 915 MHz in Hz
#define LORA_TX_POWER 20              // dBm (max 22 for SX1262)
#define LORA_SPREADING_FACTOR 7       // SF7..SF12
#define LORA_BANDWIDTH 0              // 0: 125 kHz, 1: 250 kHz, 2: 500 kHz
#define LORA_CODINGRATE 1             // 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8
#define LORA_PREAMBLE_LENGTH 8        // Preamble length
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

// OLED pins for WiFi LoRa 32 V3
#ifndef SDA_OLED
#define SDA_OLED 17
#endif
#ifndef SCL_OLED
#define SCL_OLED 18
#endif
#ifndef RST_OLED
#define RST_OLED 21
#endif

// Button pin
#define BUTTON_PIN 0

// Display settings
#define DISPLAY_TIMEOUT_MS 30000         // Turn off display after 30 seconds
#define DISPLAY_PAGE_DURATION_MS 5000    // Show each page for 5 seconds

// ============================================================================
// SENSOR DATA STRUCTURE
// ============================================================================

struct ME201WData {
    // Raw sensor readings
    float rawDistance_cm;         // R_L: Distance from sensor to liquid surface
    float batteryVoltage;         // Bat_V: Battery voltage
    int batteryUnit;              // S_Battery: Battery level (0-100 scale?)
    float temperature;            // Temp: Temperature sensor (often unused/0)
    
    // Cached computed values
    int levelCache_cm;            // Level_Cache: Inst_H - R_L (actual liquid height)
    int percentCache;             // Percent_Cache: Computed % based on thresholds
    int stateCache;               // State_Cache: 0=normal, 1=low alarm, 2=high alarm
    
    // Installation configuration (set via app)
    int instHeight_cm;            // Inst_H: Sensor to bottom distance
    int depthMax_cm;              // Dep_Max: Max depth for % calculations (usually = Inst_H)
    int maxThreshold;             // Max: High alarm threshold %
    int minThreshold;             // Mini: Low alarm threshold %
    
    // Real-time reported state (mirrors cached values)
    int sensorState;              // S_State: Current alarm state
    int sensorLevel_cm;           // S_Level: Current liquid height
    int sensorPercent;            // S_Percent: Current % full
    
    // Network & timing
    int wifiState;                // W_NET: 0=disconnected, 1=connected to cloud
    float powerTime_s;            // PWR_T: Uptime since last reset (seconds)
    
    // Status flags
    bool lowPowerMode;            // ENT_LPWR detected
    bool rebootDetected;          // REBOOT detected
    uint32_t lastUpdate;          // Timestamp of last valid data
    bool valid;                   // Data validity flag
    
    // Statistics
    uint32_t updateCount;
    uint32_t rebootCount;
    uint32_t lowPowerCount;
};

// ============================================================================
// GLOBALS
// ============================================================================

SSD1306Wire oled(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
Preferences preferences;
HardwareSerial sensorSerial(1);  // Use UART1 for sensor communication

ME201WData sensorData;
String serialBuffer = "";
uint32_t bootTime = 0;
uint32_t displayActivatedTime = 0;
bool displayActive = false;
int displayPage = 0;              // 0=data, 1=config, 2=stats
uint32_t lastPageChange = 0;

// Wake reason tracking
RTC_DATA_ATTR uint32_t bootCount = 0;
RTC_DATA_ATTR uint32_t totalTransmissions = 0;
RTC_DATA_ATTR uint32_t buttonWakeCount = 0;
RTC_DATA_ATTR uint32_t timerWakeCount = 0;
RTC_DATA_ATTR uint32_t uartWakeCount = 0;

// Sleep state
bool readyForSleep = false;
bool sensorDataReceived = false;
uint32_t wakeTime = 0;

// LoRa state
bool loraInitialized = false;
static RadioEvents_t RadioEvents;

// ============================================================================
// VEXT POWER CONTROL (OLED)
// ============================================================================

void VextON(void) {
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);  // LOW = ON
}

void VextOFF(void) {
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH); // HIGH = OFF
}

// ============================================================================
// OLED DISPLAY FUNCTIONS
// ============================================================================

void showBootScreen() {
    oled.clear();
    oled.setTextAlignment(TEXT_ALIGN_CENTER);
    oled.setFont(ArialMT_Plain_16);
    oled.drawString(64, 0, "ME201W Bridge");
    oled.setFont(ArialMT_Plain_10);
    oled.drawString(64, 20, "Serial Reader");
    oled.drawString(64, 35, "v2.0 Deep Sleep");
    
    // Show wake reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    String reason = "Power On";
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        reason = "Timer Wake";
    } else if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        reason = "Serial Wake";
    }
    oled.drawString(64, 50, reason);
    oled.display();
}

void showDataPage() {
    oled.clear();
    oled.setTextAlignment(TEXT_ALIGN_LEFT);
    oled.setFont(ArialMT_Plain_10);
    
    if (sensorData.valid) {
        // Line 0: Title + page indicator
        oled.drawString(0, 0, "Water Data [1/3]");
        
        // Line 1: Water level
        String levelStr = "Level: " + String(sensorData.sensorLevel_cm) + "cm (" + 
                         String(sensorData.sensorPercent) + "%)";
        oled.drawString(0, 12, levelStr);
        
        // Line 2: Battery
        String battStr = "Bat: " + String(sensorData.batteryVoltage, 2) + "V";
        if (sensorData.batteryUnit > 0) {
            battStr += " (" + String(sensorData.batteryUnit) + ")";
        }
        oled.drawString(0, 24, battStr);
        
        // Line 3: Raw distance reading
        String rawStr = "Raw: " + String(sensorData.rawDistance_cm, 1) + "cm";
        oled.drawString(0, 36, rawStr);
        
        // Line 4: Status
        String status = sensorData.wifiState ? "WiFi:ON" : "WiFi:OFF";
        if (sensorData.lowPowerMode) status += " LP";
        if (sensorData.stateCache == 1) status = "LOW ALARM!";
        if (sensorData.stateCache == 2) status = "HIGH ALARM!";
        oled.drawString(0, 48, status);
        
    } else {
        oled.drawString(0, 0, "Water Data [1/3]");
        oled.drawString(0, 20, "Waiting for sensor...");
        oled.drawString(0, 35, String((millis() - wakeTime) / 1000) + "s elapsed");
    }
    
    oled.display();
}

void showConfigPage() {
    oled.clear();
    oled.setTextAlignment(TEXT_ALIGN_LEFT);
    oled.setFont(ArialMT_Plain_10);
    
    oled.drawString(0, 0, "Configuration [2/3]");
    
    if (sensorData.valid && sensorData.instHeight_cm > 0) {
        String instStr = "Inst_H: " + String(sensorData.instHeight_cm) + " cm";
        oled.drawString(0, 12, instStr);
        
        String depthStr = "Dep_Max: " + String(sensorData.depthMax_cm) + " cm";
        oled.drawString(0, 24, depthStr);
        
        String maxStr = "Max Thr: " + String(sensorData.maxThreshold) + "%";
        oled.drawString(0, 36, maxStr);
        
        String minStr = "Min Thr: " + String(sensorData.minThreshold) + "%";
        oled.drawString(0, 48, minStr);
    } else {
        oled.drawString(0, 20, "No config data yet");
        oled.drawString(0, 35, "Waiting for sensor...");
    }
    
    oled.display();
}

void showStatsPage() {
    oled.clear();
    oled.setTextAlignment(TEXT_ALIGN_LEFT);
    oled.setFont(ArialMT_Plain_10);
    
    oled.drawString(0, 0, "Statistics [3/3]");
    
    String bootStr = "Boot #" + String(bootCount);
    oled.drawString(0, 12, bootStr);
    
    String txStr = "TX: " + String(totalTransmissions);
    oled.drawString(0, 24, txStr);
    
    String wakeStr = "Wake: U" + String(uartWakeCount) + " T" + String(timerWakeCount);
    oled.drawString(0, 36, wakeStr);
    
    String sleepStr = UART_WAKE_ENABLED ? "UART Wake: ON" : "Timer: " + String(SLEEP_INTERVAL_SECONDS) + "s";
    oled.drawString(0, 48, sleepStr);
    
    oled.display();
}

void updateDisplay() {
    if (!displayActive) return;
    
    // Auto-advance pages every DISPLAY_PAGE_DURATION_MS
    if (millis() - lastPageChange > DISPLAY_PAGE_DURATION_MS) {
        displayPage = (displayPage + 1) % 3;
        lastPageChange = millis();
    }
    
    switch (displayPage) {
        case 0:
            showDataPage();
            break;
        case 1:
            showConfigPage();
            break;
        case 2:
            showStatsPage();
            break;
    }
}

// ============================================================================
// SERIAL PARSING FUNCTIONS
// ============================================================================

float parseFloat(String line, const char* key) {
    int idx = line.indexOf(key);
    if (idx == -1) return 0.0;
    String value = line.substring(idx + strlen(key));
    value.trim();
    // Remove any trailing units or text
    int spaceIdx = value.indexOf(' ');
    if (spaceIdx != -1) {
        value = value.substring(0, spaceIdx);
    }
    return value.toFloat();
}

int parseInt(String line, const char* key) {
    int idx = line.indexOf(key);
    if (idx == -1) return 0;
    String value = line.substring(idx + strlen(key));
    value.trim();
    // Remove any trailing units or text
    int spaceIdx = value.indexOf(' ');
    if (spaceIdx != -1) {
        value = value.substring(0, spaceIdx);
    }
    return value.toInt();
}

void parseSensorLine(String line) {
    line.trim();
    
    // Skip empty lines
    if (line.length() == 0) return;
    
    // Event detection
    if (line.indexOf("REBOOT!") != -1) {
        Serial.println("� Sensor rebooted");
        sensorData.rebootDetected = true;
        sensorData.rebootCount++;
        return;
    }
    
    if (line.indexOf("ENT_LPWR") != -1) {
        Serial.println("💤 Entering low power mode");
        sensorData.lowPowerMode = true;
        sensorData.lowPowerCount++;
        return;
    }
    
    // Parse data fields
    
    // Raw distance from sensor to liquid surface
    if (line.indexOf("R_L =") != -1) {
        sensorData.rawDistance_cm = parseFloat(line, "R_L =");
        sensorData.valid = true;
        sensorData.lastUpdate = millis();
        Serial.printf("� Raw Distance: %.1f cm (sensor to surface)\n", sensorData.rawDistance_cm);
    }
    
    // Cached computed liquid height (Inst_H - R_L)
    if (line.indexOf("Level_Cache =") != -1) {
        sensorData.levelCache_cm = parseInt(line, "Level_Cache =");
        Serial.printf("💧 Level Cache: %d cm (computed height)\n", sensorData.levelCache_cm);
    }
    
    // Cached percent full
    if (line.indexOf("Percent_Cache =") != -1) {
        sensorData.percentCache = parseInt(line, "Percent_Cache =");
        Serial.printf("📊 Percent Cache: %d%%\n", sensorData.percentCache);
    }
    
    // Cached alarm state (0=normal, 1=low, 2=high)
    if (line.indexOf("State_Cache =") != -1) {
        sensorData.stateCache = parseInt(line, "State_Cache =");
        const char* stateStr = (sensorData.stateCache == 0) ? "Normal" :
                               (sensorData.stateCache == 1) ? "LOW ALARM" : "HIGH ALARM";
        Serial.printf("🚨 State Cache: %d (%s)\n", sensorData.stateCache, stateStr);
    }
    
    // Real-time sensor level (same as Level_Cache, updated live)
    if (line.indexOf("S_Level =") != -1) {
        sensorData.sensorLevel_cm = parseInt(line, "S_Level =");
        Serial.printf("💧 Sensor Level: %d cm\n", sensorData.sensorLevel_cm);
    }
    
    // Real-time percent (same as Percent_Cache, updated live)
    if (line.indexOf("S_Percent =") != -1) {
        sensorData.sensorPercent = parseInt(line, "S_Percent =");
        Serial.printf("📊 Sensor Percent: %d%%\n", sensorData.sensorPercent);
    }
    
    // Real-time state (mirrors State_Cache)
    if (line.indexOf("S_State =") != -1) {
        sensorData.sensorState = parseInt(line, "S_State =");
    }
    
    // Battery voltage
    if (line.indexOf("Bat_V =") != -1) {
        sensorData.batteryVoltage = parseFloat(line, "Bat_V =");
        Serial.printf("🔋 Battery: %.3f V\n", sensorData.batteryVoltage);
    }
    
    // Battery level unit (0-100 scale, needs calibration)
    if (line.indexOf("S_Battery =") != -1) {
        sensorData.batteryUnit = parseInt(line, "S_Battery =");
        Serial.printf("🔋 Battery Unit: %d\n", sensorData.batteryUnit);
    }
    
    // Temperature (often unused, reads 0)
    if (line.indexOf("Temp =") != -1) {
        sensorData.temperature = parseFloat(line, "Temp =");
        if (sensorData.temperature != 0) {
            Serial.printf("🌡️  Temperature: %.1f'C\n", sensorData.temperature);
        }
    }
    
    // WiFi/network state (0=not connected, 1=connected to cloud)
    if (line.indexOf("W_NET =") != -1) {
        sensorData.wifiState = parseInt(line, "W_NET =");
        Serial.printf("📶 WiFi: %s\n", sensorData.wifiState ? "Connected" : "Disconnected");
    }
    
    // Power-on time (uptime in seconds since last reset)
    if (line.indexOf("PWR_T =") != -1) {
        sensorData.powerTime_s = parseFloat(line, "PWR_T =");
        Serial.printf("⏱️  Uptime: %.1f s\n", sensorData.powerTime_s);
    }
    
    // Installation height (sensor to tank bottom)
    if (line.indexOf("Inst_H =") != -1) {
        sensorData.instHeight_cm = parseInt(line, "Inst_H =");
        Serial.printf("📐 Install Height: %d cm\n", sensorData.instHeight_cm);
    }
    
    // Maximum depth for calculations
    if (line.indexOf("Dep_Max =") != -1) {
        sensorData.depthMax_cm = parseInt(line, "Dep_Max =");
    }
    
    // High alarm threshold percentage
    if (line.indexOf("Max =") != -1) {
        sensorData.maxThreshold = parseInt(line, "Max =");
        Serial.printf("⬆️  Max Threshold: %d%%\n", sensorData.maxThreshold);
    }
    
    // Low alarm threshold percentage
    if (line.indexOf("Mini =") != -1) {
        sensorData.minThreshold = parseInt(line, "Mini =");
        Serial.printf("⬇️  Min Threshold: %d%%\n", sensorData.minThreshold);
    }
}

void readSensorSerial() {
    static uint32_t lastDebug = 0;
    static uint32_t bytesReceived = 0;
    static uint32_t linesReceived = 0;
    
    while (sensorSerial.available()) {
        char c = sensorSerial.read();
        bytesReceived++;
        
        // Echo to debug console (so you can see raw data)
        Serial.print(c);
        
        // Buffer line
        if (c == '\n' || c == '\r') {
            if (serialBuffer.length() > 0) {
                linesReceived++;
                // Show the line we're about to parse
                Serial.printf("\n[LINE %d] Parsing: '%s'\n", linesReceived, serialBuffer.c_str());
                parseSensorLine(serialBuffer);
                serialBuffer = "";
                sensorData.updateCount++;
            }
        } else {
            serialBuffer += c;
        }
        
        // Reset low power flag on new data
        sensorData.lowPowerMode = false;
        sensorData.rebootDetected = false;
    }
    
    // Debug output every 5 seconds
    if (millis() - lastDebug > 5000) {
        lastDebug = millis();
        if (bytesReceived == 0) {
            Serial.println("⚠️  No serial data received from sensor!");
            Serial.println("   Check wiring:");
            Serial.println("   - ME201W TX → ESP32 GPIO 5 (RX)");
            Serial.println("   - ME201W GND → ESP32 GND");
            Serial.printf("   - Current config: GPIO %d @ %d baud\n", SENSOR_SERIAL_RX, SENSOR_BAUD_RATE);
        } else {
            Serial.printf("📊 Received %d bytes, %d lines, parsed=%d, valid=%s\n", 
                         bytesReceived, linesReceived, sensorData.updateCount, 
                         sensorData.valid ? "YES" : "NO");
            if (sensorData.valid) {
                Serial.printf("   Level: %d cm, Percent: %d%%, Battery: %.2fV\n",
                             sensorData.sensorLevel_cm, sensorData.sensorPercent, sensorData.batteryVoltage);
            }
        }
        bytesReceived = 0;
        linesReceived = 0;
    }
}

// ============================================================================
// JSON CREATION
// ============================================================================

String createJSON() {
    StaticJsonDocument<512> doc;
    
    doc["dev"] = "ME201W";
    doc["ts"] = millis();
    doc["up"] = sensorData.powerTime_s;
    
    // Water data - use S_Level (computed actual height)
    JsonObject water = doc.createNestedObject("water");
    water["lvl"] = sensorData.sensorLevel_cm;              // Actual liquid height (Inst_H - R_L)
    water["pct"] = sensorData.sensorPercent;               // % full based on thresholds
    water["raw"] = round(sensorData.rawDistance_cm * 10) / 10.0;  // Sensor to surface
    water["inst"] = sensorData.instHeight_cm;              // Sensor to bottom
    water["st"] = sensorData.sensorState;                  // 0=normal, 1=low, 2=high
    
    // Thresholds
    JsonObject thr = doc.createNestedObject("thr");
    thr["max"] = sensorData.maxThreshold;
    thr["min"] = sensorData.minThreshold;
    
    // Battery
    JsonObject batt = doc.createNestedObject("batt");
    batt["v"] = round(sensorData.batteryVoltage * 100) / 100.0;
    batt["u"] = sensorData.batteryUnit;                    // 0-100 scale
    
    // Environment
    if (sensorData.temperature != 0) {
        doc["temp"] = round(sensorData.temperature * 10) / 10.0;
    }
    
    // Status (minimal - only include what's needed)
    JsonObject stat = doc.createNestedObject("stat");
    stat["wifi"] = sensorData.wifiState;
    stat["lp"] = sensorData.lowPowerMode;
    
    String output;
    serializeJson(doc, output);
    return output;
}

// ============================================================================
// LORA FUNCTIONS
// ============================================================================

// Radio event callbacks (required for Heltec Radio API)
void OnTxDone(void) {
    Serial.println("📡 LoRa TX Done");
}

void OnTxTimeout(void) {
    Serial.println("⚠️  LoRa TX Timeout");
    // COMMENTED OUT - Causes crash on some Heltec boards
    // Radio.Sleep();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    // Not used in this application (TX only)
}

void OnRxTimeout(void) {
    // Not used in this application (TX only)
}

void OnRxError(void) {
    // Not used in this application (TX only)
}

void initLoRa() {
    if (loraInitialized) return;
    
    Serial.println("\n📡 Initializing LoRa (SX1262)...");
    
    // CRITICAL: Set license BEFORE Mcu.begin() for Heltec V3 boards
    Serial.println("Setting Heltec license...");
    Mcu.setlicense(license, HELTEC_BOARD);
    
    Serial.println("Initializing MCU...");
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
    
    // Set up callbacks
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    
    // Initialize radio
    Radio.Init(&RadioEvents);
    
    // Set sync word to match gateway (0x34 = standard LoRa private network)
    Radio.SetSyncWord(0x34);
    
    // Configure TX parameters
    Radio.SetTxConfig(
        MODEM_LORA,                     // Modem type
        LORA_TX_POWER,                  // Power in dBm
        0,                              // Frequency deviation (FSK only)
        LORA_BANDWIDTH,                 // Bandwidth
        LORA_SPREADING_FACTOR,          // Spreading factor
        LORA_CODINGRATE,                // Coding rate
        LORA_PREAMBLE_LENGTH,           // Preamble length
        LORA_FIX_LENGTH_PAYLOAD_ON,     // Fixed length payload
        true,                           // CRC enabled
        0,                              // Frequency hopping (disabled)
        0,                              // Hop period
        LORA_IQ_INVERSION_ON,           // IQ inversion
        3000                            // TX timeout (ms)
    );
    
    // Set frequency
    Radio.SetChannel(LORA_FREQUENCY);
    
    loraInitialized = true;
    Serial.printf("✅ LoRa initialized: %d MHz, SF%d, BW%d, TX Power %d dBm\n", 
                  LORA_FREQUENCY / 1000000, LORA_SPREADING_FACTOR, 
                  (LORA_BANDWIDTH == 0) ? 125 : (LORA_BANDWIDTH == 1) ? 250 : 500,
                  LORA_TX_POWER);
}

void transmitLoRa() {
    if (!ENABLE_LORA) {
        Serial.println("\n📡 LoRa transmission (disabled):");
        String json = createJSON();
        Serial.println(json);
        Serial.println("---");
        return;
    }
    
    // Initialize LoRa if not already done
    if (!loraInitialized) {
        initLoRa();
    }
    
    // Create JSON payload
    String json = createJSON();
    Serial.println("\n📡 ========================================");
    Serial.println("📡 Transmitting via LoRa");
    Serial.println("📡 ========================================");
    Serial.println(json);
    
    // Send packet
    uint8_t txBuffer[256];
    int len = json.length();
    if (len > 255) {
        Serial.printf("⚠️  WARNING: Payload too large (%d bytes), truncating to 255\n", len);
        len = 255;  // SX1262 max payload is 256, but we use 255 to be safe
    }
    
    json.getBytes(txBuffer, len + 1);
    
    Serial.printf("📡 Packet size: %d bytes\n", len);
    Radio.Send(txBuffer, len);
    
    // Wait for TX to complete (blocking)
    delay(100);  // Brief delay for transmission
    
    Serial.println("📡 ========================================\n");
}

// ============================================================================
// DEEP SLEEP FUNCTIONS
// ============================================================================

void enterDeepSleep() {
    Serial.println("\n💤 ========================================");
    Serial.println("💤 Entering Deep Sleep");
    if (SLEEP_INTERVAL_SECONDS > 0) {
        Serial.printf("💤 Sleep Duration: %d seconds (timer backup)\n", SLEEP_INTERVAL_SECONDS);
    } else {
        Serial.println("💤 Sleep Duration: until serial activity (no timer backup)");
    }
    
    if (UART_WAKE_ENABLED) {
        if (SLEEP_INTERVAL_SECONDS > 0) {
            Serial.println("💤 Serial Wake: ENABLED (EXT0 on start bit LOW)");
            Serial.printf("💤 Wake sources: Serial RX (GPIO %d LOW) or Timer (%ds)\n", SENSOR_SERIAL_RX, SLEEP_INTERVAL_SECONDS);
        } else {
            Serial.println("💤 Wake sources: Serial RX only (EXT0 on start bit LOW)");
        }
    } else if (SLEEP_INTERVAL_SECONDS > 0) {
        Serial.printf("💤 Wake sources: Timer (%ds) only\n", SLEEP_INTERVAL_SECONDS);
    }
    
    Serial.println("💤 ========================================\n");
    
    // Save data to preferences
    preferences.putUInt("bootCount", bootCount);
    preferences.putUInt("totalTx", totalTransmissions);
    preferences.putUInt("btnWake", buttonWakeCount);
    preferences.putUInt("tmrWake", timerWakeCount);
    preferences.putUInt("uartWake", uartWakeCount);
    
    // Put radio to sleep - COMMENTED OUT - Causes LoadProhibited crash
    // if (loraInitialized) {
    //     Radio.Sleep();
    // }
    
    // Turn off display
    oled.clear();
    oled.display();
    VextOFF();
    
    // Flush serial
    Serial.flush();
    delay(100);
    
    // Configure wake sources
    if (SLEEP_INTERVAL_SECONDS > 0) {
        uint64_t sleepTime = SLEEP_INTERVAL_SECONDS * 1000000ULL; // Convert to microseconds
        esp_sleep_enable_timer_wakeup(sleepTime);
    }
    
    // EXT0 wake on RX LOW (UART start bit). Pin must be RTC IO; using GPIO5.
    if (UART_WAKE_ENABLED) {
        esp_sleep_enable_ext0_wakeup((gpio_num_t)SENSOR_SERIAL_RX, 0); // Wake when line goes LOW
    }
    
    if (BUTTON_WAKE_ENABLED) {
        // Configure button (GPIO 0) as wake source (active LOW)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
    }
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void checkWakeReason() {
    bootCount++;
    
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    Serial.println("\n🌅 ========================================");
    Serial.printf("🌅 Wake Event #%d\n", bootCount);
    
    switch(wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("🌅 Wake Reason: SERIAL (start bit)\n");
            uartWakeCount++;  // Treat EXT0 as serial wake
            displayActive = false;  // Keep display off for serial wake
            break;
            
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("🌅 Wake Reason: TIMER (backup)");
            timerWakeCount++;
            displayActive = false;  // Keep display off for timer wake
            break;
            
        case ESP_SLEEP_WAKEUP_UART:
            Serial.println("🌅 Wake Reason: UART - Sensor transmitting!");
            uartWakeCount++;
            displayActive = false;  // Keep display off for UART wake
            break;
            
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            Serial.println("🌅 Wake Reason: POWER ON / RESET");
            displayActive = true;  // Show display on first boot
            displayActivatedTime = millis();
            break;
    }
    
    Serial.printf("🌅 Total: UART=%d, Timer=%d, Button=%d\n", uartWakeCount, timerWakeCount, buttonWakeCount);
    Serial.println("🌅 ========================================\n");
}

// ============================================================================
// BUTTON HANDLING
// ============================================================================

void handleButton() {
    static uint32_t lastPress = 0;
    static bool wasPressed = false;
    
    bool pressed = (digitalRead(BUTTON_PIN) == LOW);
    
    if (pressed && !wasPressed && (millis() - lastPress > 500)) {
        lastPress = millis();
        
        if (!displayActive) {
            // Wake display
            displayActive = true;
            displayActivatedTime = millis();
            displayPage = 0;
            lastPageChange = millis();
            VextON();
            delay(100);
            oled.init();
            Serial.println("🔆 Display activated by button");
        } else {
            // Advance to next page
            displayPage = (displayPage + 1) % 3;
            lastPageChange = millis();
            displayActivatedTime = millis();  // Reset timeout
            Serial.printf("📄 Page changed to %d\n", displayPage);
        }
        
        updateDisplay();
    }
    
    wasPressed = pressed;
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(100);
    
    wakeTime = millis();
    bootTime = millis();
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  ME201W Serial Bridge v2.0");
    Serial.println("  Deep Sleep Mode");
    Serial.println("========================================");
    Serial.println();
    
    // Turn off LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    preferences.begin("water-sensor", false);
    
    // Load persistent counters from RTC memory or preferences
    if (bootCount == 0) {
        // First boot after power loss - load from preferences
        bootCount = preferences.getUInt("bootCount", 0);
        totalTransmissions = preferences.getUInt("totalTx", 0);
        buttonWakeCount = preferences.getUInt("btnWake", 0);
        timerWakeCount = preferences.getUInt("tmrWake", 0);
        uartWakeCount = preferences.getUInt("uartWake", 0);
    }
    
    // Check why we woke up
    checkWakeReason();
    
    // Initialize display (only if active)
    if (displayActive) {
        VextON();
        delay(100);
        oled.init();
        oled.clear();
        oled.display();
        delay(100);
        showBootScreen();
        delay(2000);
    } else {
        // Keep display off for power savings
        VextOFF();
        Serial.println("💤 Display sleeping (timer wake)");
    }
    
    // Initialize serial connection to sensor
    Serial.printf("📡 Initializing sensor serial on RX=%d @ %d baud\n", 
                  SENSOR_SERIAL_RX, SENSOR_BAUD_RATE);
    sensorSerial.begin(SENSOR_BAUD_RATE, SERIAL_8N1, SENSOR_SERIAL_RX, SENSOR_SERIAL_TX);
    
    // Initialize sensor data structure
    memset(&sensorData, 0, sizeof(sensorData));
    sensorDataReceived = false;
    
    Serial.printf("⏰ Sensor read timeout: %d seconds\n", SENSOR_READ_TIMEOUT_MS / 1000);
    
    if (UART_WAKE_ENABLED) {
        Serial.println("💤 Power Mode: Deep Sleep + EXT0 Serial Wake");
        Serial.printf("💤 RX pin: GPIO%d (idle HIGH, wake on LOW start bit)\n", SENSOR_SERIAL_RX);
        Serial.printf("💤 Backup timer: %d seconds\n", SLEEP_INTERVAL_SECONDS);
        Serial.println("⚠️  First byte of transmission may be lost (wake latency) - sensor sends multiple lines so OK.");
    } else {
        Serial.printf("💤 Power Mode: Timer Wake every %d seconds\n", SLEEP_INTERVAL_SECONDS);
    }
    
    // Initialize LoRa if enabled
    if (ENABLE_LORA) {
        initLoRa();
    }
    
    Serial.println("✅ Ready to receive sensor data\n");
    
    if (displayActive) {
        lastPageChange = millis();
        updateDisplay();
    }
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // CRITICAL: Process radio events to service LoRa callbacks
    // Without this, OnTxDone/OnTxTimeout will never fire!
    if (loraInitialized && ENABLE_LORA) {
        Radio.IrqProcess();
    }
    
    // Read sensor serial data
    readSensorSerial();
    
    // Check if we got valid sensor data (valid flag is set when R_L is received)
    // IMPROVED: Wait for multiple key fields to ensure complete data packet
    if (sensorData.valid && !sensorDataReceived) {
        // Check for complete data: battery voltage AND at least one level reading
        bool hasComplete = (sensorData.batteryVoltage > 0) && 
                          (sensorData.sensorLevel_cm > 0 || sensorData.rawDistance_cm > 0);
        
        if (hasComplete) {
            sensorDataReceived = true;
            Serial.println("✅ Complete sensor data received!");
            Serial.printf("   Level: %d cm, Percent: %d%%, Battery: %.2fV\n",
                         sensorData.sensorLevel_cm, sensorData.sensorPercent, sensorData.batteryVoltage);
            
            // Transmit via LoRa
            transmitLoRa();
            totalTransmissions++;
            
            // Go back to sleep immediately after transmission (unless display is active)
            if (!displayActive && DEEP_SLEEP_ENABLED) {
                Serial.println("💤 Data transmitted - going back to sleep");
                delay(100);  // Brief delay to ensure serial output completes
                enterDeepSleep();
            } else if (!DEEP_SLEEP_ENABLED) {
                // If deep sleep is disabled (testing mode), reset flags after transmission
                // so we can transmit again when new sensor data arrives
                Serial.println("🔄 Deep sleep disabled - will transmit on next sensor update");
                delay(2000);  // Brief delay before allowing next transmission
                sensorDataReceived = false;
                sensorData.valid = false;  // Reset valid flag to wait for fresh data
            }
        }
    }
    
    // Update display if active
    if (displayActive) {
        static uint32_t lastDisplayUpdate = 0;
        if (millis() - lastDisplayUpdate > 500) {  // Update every 500ms when active
            lastDisplayUpdate = millis();
            updateDisplay();
        }
        
        // Check display timeout
        if (millis() - displayActivatedTime > DISPLAY_TIMEOUT_MS) {
            Serial.println("💤 Display timeout - turning off");
            displayActive = false;
            oled.clear();
            oled.display();
            VextOFF();
            
            // If we already have data, go to sleep
            if (sensorDataReceived && DEEP_SLEEP_ENABLED) {
                readyForSleep = true;
            }
        }
    }
    
    // Handle button press
    handleButton();
    
    // Check if we should enter deep sleep
    if (DEEP_SLEEP_ENABLED) {
        // Sleep conditions:
        // 1. We have sensor data (or timeout waiting for it)
        // 2. Display is off
        // 3. Enough time has passed
        
        uint32_t awakeTime = millis() - wakeTime;
        bool timedOut = (awakeTime > SENSOR_READ_TIMEOUT_MS);
        bool hasData = sensorDataReceived;
        
        if (readyForSleep || (timedOut && !displayActive)) {
            if (timedOut && !hasData) {
                Serial.println("⚠️  Sensor read timeout - sleeping anyway");
            }
            enterDeepSleep();
        }
    } else {
        // Deep sleep disabled - just keep running
        delay(10);
    }
    
    delay(10);  // Small delay to prevent watchdog issues
}