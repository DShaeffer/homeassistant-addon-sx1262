/**
 * Simple LoRa TX Test for Heltec WiFi LoRa 32 V3
 * 
 * Purpose: Minimal test to verify LoRa transmission is working
 * Transmits a simple message every 5 seconds
 * 
 * Hardware: Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
 * Target: Raspberry Pi with Waveshare SX1262 HAT
 * 
 * Author: Hillsville Cabin Project
 * Date: November 2025
 */

#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include "LoRaWan_APP.h"

// ============================================================================
// CONFIGURATION - MUST MATCH GATEWAY!
// ============================================================================

#define LORA_FREQUENCY 915000000      // 915 MHz (match gateway)
#define LORA_TX_POWER 20              // 20 dBm (match gateway)
#define LORA_SPREADING_FACTOR 7       // SF7 (match gateway)
#define LORA_BANDWIDTH 0              // 0 = 125 kHz (match gateway)
#define LORA_CODINGRATE 1             // 1 = 4/5 (match gateway)
#define LORA_PREAMBLE_LENGTH 8        // 8 symbols (match gateway)
#define LORA_SYNC_WORD 0x34           // 0x34 (match gateway 0x3434)
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

#define TX_INTERVAL_MS 5000           // Transmit every 5 seconds

// ============================================================================
// GLOBALS
// ============================================================================

bool loraInitialized = false;
static RadioEvents_t RadioEvents;
uint32_t packetCount = 0;
uint32_t lastTxTime = 0;
bool txInProgress = false;
uint32_t txStartTime = 0; // when current TX began

// ============================================================================
// LORA CALLBACKS
// ============================================================================

void OnTxDone(void) {
    Serial.println("✅ TX Done");
    txInProgress = false;
}

void OnTxTimeout(void) {
    Serial.println("⚠️  TX Timeout");
    txInProgress = false;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    // Not used (TX only)
}

void OnRxTimeout(void) {
    // Not used (TX only)
}

void OnRxError(void) {
    // Not used (TX only)
}

// ============================================================================
// LORA INITIALIZATION
// ============================================================================

void initLoRa() {
    Serial.println("\n========================================");
    Serial.println("📡 Initializing LoRa Radio (SX1262)");
    Serial.println("========================================");
    
    // Set up callbacks
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    
    // Initialize radio hardware
    Radio.Init(&RadioEvents);
    Serial.println("✅ Radio.Init() complete");
    
    // Set sync word FIRST (before SetTxConfig)
    Radio.SetSyncWord(LORA_SYNC_WORD);
    Serial.printf("✅ Sync Word: 0x%02X\n", LORA_SYNC_WORD);
    
    // Configure TX parameters
    Radio.SetTxConfig(
        MODEM_LORA,                     // Modem type
        LORA_TX_POWER,                  // Power in dBm
        0,                              // Frequency deviation (FSK only)
        LORA_BANDWIDTH,                 // Bandwidth (0=125kHz, 1=250kHz, 2=500kHz)
        LORA_SPREADING_FACTOR,          // Spreading factor (7-12)
        LORA_CODINGRATE,                // Coding rate (1=4/5, 2=4/6, 3=4/7, 4=4/8)
        LORA_PREAMBLE_LENGTH,           // Preamble length
        LORA_FIX_LENGTH_PAYLOAD_ON,     // Fixed length payload
        true,                           // CRC enabled
        0,                              // Frequency hopping disabled
        0,                              // Hop period
        LORA_IQ_INVERSION_ON,           // IQ inversion (false = normal)
        3000                            // TX timeout (ms)
    );
    Serial.println("✅ Radio.SetTxConfig() complete");
    
    // Set frequency
    Radio.SetChannel(LORA_FREQUENCY);
    Serial.printf("✅ Frequency: %d Hz\n", LORA_FREQUENCY);
    
    loraInitialized = true;
    
    Serial.println("\n📡 LoRa Configuration Summary:");
    Serial.printf("   Frequency:    %d MHz\n", LORA_FREQUENCY / 1000000);
    Serial.printf("   TX Power:     %d dBm\n", LORA_TX_POWER);
    Serial.printf("   SF:           %d\n", LORA_SPREADING_FACTOR);
    Serial.printf("   Bandwidth:    %s\n", 
                  LORA_BANDWIDTH == 0 ? "125 kHz" : 
                  LORA_BANDWIDTH == 1 ? "250 kHz" : "500 kHz");
    Serial.printf("   Coding Rate:  4/%d\n", LORA_CODINGRATE + 4);
    Serial.printf("   Preamble:     %d symbols\n", LORA_PREAMBLE_LENGTH);
    Serial.printf("   Sync Word:    0x%02X\n", LORA_SYNC_WORD);
    Serial.printf("   CRC:          Enabled\n");
    Serial.printf("   IQ Inversion: %s\n", LORA_IQ_INVERSION_ON ? "Yes" : "No");
    Serial.println("========================================\n");
}

// ============================================================================
// TRANSMIT TEST MESSAGE
// ============================================================================

void transmitTestMessage() {
    if (txInProgress) {
        Serial.println("⏳ TX still in progress, skipping...");
        return;
    }
    
    packetCount++;
    
    // Create simple test message
    char message[128];
    snprintf(message, sizeof(message), 
             "{\"test\":\"esp32\",\"packet\":%lu,\"uptime\":%lu,\"rssi\":0}", 
             packetCount, millis() / 1000);
    
    uint8_t txBuffer[128];
    int len = strlen(message);
    memcpy(txBuffer, message, len);
    
    Serial.println("\n📡 ========================================");
    Serial.printf("📡 TX Packet #%lu\n", packetCount);
    Serial.println("📡 ========================================");
    Serial.printf("Message: %s\n", message);
    Serial.printf("Length:  %d bytes\n", len);
    Serial.println("Transmitting...");
    
    txInProgress = true;
    txStartTime = millis();
    Radio.Send(txBuffer, len);
    
    // Wait a bit for transmission to start
    delay(50);
    
    // Note: OnTxDone() callback will be called by Radio.IrqProcess() in loop()
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  ESP32 LoRa TX Test - Simple");
    Serial.println("  Heltec WiFi LoRa 32 V3");
    Serial.println("========================================");
    Serial.println();
    
    // Turn off LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    
    // Initialize LoRa
    initLoRa();
    
    Serial.println("✅ Setup complete!");
    Serial.printf("Will transmit every %d seconds\n\n", TX_INTERVAL_MS / 1000);
    
    lastTxTime = millis();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // CRITICAL: Process radio events
    // The Heltec library requires calling Radio.IrqProcess() to service callbacks
    Radio.IrqProcess();

    // If TX seems stuck beyond 3500 ms (slightly > configured 3000 ms timeout), recover
    if (txInProgress) {
        uint32_t elapsed = millis() - txStartTime;
        if (elapsed > 500 && (elapsed % 500) < 15) { // periodic status every ~500ms
            Serial.printf("⏳ Waiting for TX complete... %lu ms\n", elapsed);
        }
        if (elapsed > 3500) {
            Serial.println("❌ TX did not complete within expected window. Forcing standby and clearing flag.");
            Radio.Standby();
            txInProgress = false;
        }
    }
    
    // Transmit at regular intervals
    if (!txInProgress && millis() - lastTxTime >= TX_INTERVAL_MS) {
        transmitTestMessage();
        lastTxTime = millis();
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}
