/**
 * Heltec ESP32 LoRa Gateway Test Example
 * 
 * A minimal example for testing the SX1262 LoRa Gateway Home Assistant add-on
 * Works with Heltec WiFi LoRa 32 V3 and similar boards
 * 
 * BEFORE UPLOADING:
 * 1. Get your Heltec license from: https://resource.heltec.cn/search
 * 2. Update the license[4] array below with your values
 * 3. Ensure LoRa parameters match your gateway configuration
 * 
 * Author: Home Assistant SX1262 Gateway Project
 * License: MIT
 * Repository: https://github.com/DShaeffer/homeassistant-addon-sx1262
 */

#include "LoRaWan_APP.h"
#include "Arduino.h"

// ============================================================================
// HELTEC LICENSE - REQUIRED FOR V3 BOARDS
// ============================================================================
// Get your license from: https://resource.heltec.cn/search
// Enter your chip ID and it will generate these 4 values
uint32_t license[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

// ============================================================================
// LORA CONFIGURATION - MUST MATCH YOUR GATEWAY!
// ============================================================================
#define RF_FREQUENCY                915000000  // Hz (915.0 MHz)
                                               // US: 915 MHz, EU: 868 MHz
#define TX_OUTPUT_POWER             14         // dBm (2-20)
                                               // Start with 14 for testing
#define LORA_BANDWIDTH              0          // 0=125kHz, 1=250kHz, 2=500kHz
#define LORA_SPREADING_FACTOR       7          // 7-12 (higher=longer range)
#define LORA_CODINGRATE             1          // 1=4/5, 2=4/6, 3=4/7, 4=4/8
#define LORA_PREAMBLE_LENGTH        8          // Symbols (typically 8)
#define LORA_FIX_LENGTH_PAYLOAD_ON  false      // false = variable length
#define LORA_IQ_INVERSION_ON        false      // false = normal IQ

// ============================================================================
// APPLICATION SETTINGS
// ============================================================================
#define TX_INTERVAL_MS              10000      // Send packet every 10 seconds
#define BUFFER_SIZE                 256        // TX buffer size

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
char txpacket[BUFFER_SIZE];
uint32_t packetCount = 0;
bool lora_idle = true;

static RadioEvents_t RadioEvents;

// ============================================================================
// LORA CALLBACKS
// ============================================================================

void OnTxDone(void) {
    Serial.println("✅ TX Done");
    lora_idle = true;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    Serial.println("⚠️  TX Timeout");
    lora_idle = true;
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n========================================");
    Serial.println("  Heltec ESP32 LoRa Gateway Test");
    Serial.println("  WiFi LoRa 32 V3 (ESP32-S3 + SX1262)");
    Serial.println("========================================\n");
    
    // CRITICAL: Set license BEFORE Mcu.begin()
    Serial.println("Setting Heltec license...");
    Mcu.setlicense(license, HELTEC_BOARD);
    
    Serial.println("Initializing MCU...");
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
    
    // Setup radio callbacks
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    // Initialize radio
    Serial.println("Initializing SX1262 radio...");
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
    
    Serial.println("\n📡 LoRa Configuration:");
    Serial.printf("   Frequency:     %d MHz\n", RF_FREQUENCY / 1000000);
    Serial.printf("   TX Power:      %d dBm\n", TX_OUTPUT_POWER);
    Serial.printf("   Spreading Factor: SF%d\n", LORA_SPREADING_FACTOR);
    Serial.printf("   Bandwidth:     %s\n", 
                  LORA_BANDWIDTH == 0 ? "125 kHz" : 
                  LORA_BANDWIDTH == 1 ? "250 kHz" : "500 kHz");
    Serial.printf("   Coding Rate:   4/%d\n", LORA_CODINGRATE + 4);
    Serial.printf("   Preamble:      %d symbols\n", LORA_PREAMBLE_LENGTH);
    Serial.println("========================================\n");
    
    Serial.println("✅ Setup complete!");
    Serial.printf("Transmitting test packets every %d seconds\n\n", TX_INTERVAL_MS / 1000);
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    if (lora_idle) {
        delay(TX_INTERVAL_MS);
        packetCount++;
        
        // Create a simple JSON payload
        // Customize this for your application!
        float temp = 22.5 + (rand() % 30) / 10.0;  // Random temp 22.5-25.4°C
        float humidity = 45.0 + (rand() % 20);     // Random humidity 45-64%
        
        snprintf(txpacket, sizeof(txpacket),
                 "{\"device\":\"heltec_test\","
                 "\"packet\":%lu,"
                 "\"uptime\":%lu,"
                 "\"temperature\":%.1f,"
                 "\"humidity\":%.0f,"
                 "\"rssi\":0}",
                 packetCount, 
                 millis() / 1000,
                 temp,
                 humidity);
        
        Serial.println("\n📡 ========================================");
        Serial.printf("📡 TX Packet #%lu\n", packetCount);
        Serial.println("📡 ========================================");
        Serial.printf("Payload: %s\n", txpacket);
        Serial.printf("Length:  %d bytes\n", strlen(txpacket));
        Serial.println("Transmitting...");
        
        Radio.Send((uint8_t *)txpacket, strlen(txpacket));
        lora_idle = false;
    }
    
    // CRITICAL: Must call this regularly to process radio interrupts
    Radio.IrqProcess();
}
