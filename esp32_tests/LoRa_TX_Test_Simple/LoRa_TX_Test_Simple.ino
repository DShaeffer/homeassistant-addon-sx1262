/**
 * Simple LoRa TX Test for Heltec WiFi LoRa 32 V3
 * 
 * Purpose: Minimal test to verify LoRa transmission is working
 * Transmits a simple message every 5 seconds
 * 
 * Hardware: Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
 * Target: Raspberry Pi with Waveshare SX1262 HAT
 * 
 * Based on: Heltec LoRaSender example
 * Author: Hillsville Cabin Project
 * Date: November 2025
 */

#include "LoRaWan_APP.h"
#include "Arduino.h"

// ---------------------------------------------------------------------------
// Low-level SX126x register access helpers using Heltec driver functions
// (prototypes come from driver/sx126x.h included via LoRaWan_APP.h)
// ---------------------------------------------------------------------------
static uint8_t readReg(uint16_t addr) {
    uint8_t v = 0; SX126xReadRegisters(addr, &v, (uint16_t)1); return v;
}
static void writeReg(uint16_t addr, uint8_t v) {
    SX126xWriteRegisters(addr, &v, (uint16_t)1);
}

// ============================================================================
// LICENSE (for Heltec V3 boards)
// ============================================================================
uint32_t license[4] = {0x3521755D, 0xB0401FFE, 0xA7307FF6, 0xDDFE8E4E};

// ============================================================================
// CONFIGURATION - MUST MATCH GATEWAY!
// ============================================================================

#define RF_FREQUENCY                915000000     // 915 MHz (match gateway)
#define TX_OUTPUT_POWER             20            // 20 dBm (match gateway)
#define LORA_BANDWIDTH              0             // 0 = 125 kHz (match gateway)
#define LORA_SPREADING_FACTOR       7             // SF7 (match gateway)
#define LORA_CODINGRATE             1             // 1 = 4/5 (match gateway)
#define LORA_PREAMBLE_LENGTH        8             // 8 symbols (match gateway)
#define LORA_SYMBOL_TIMEOUT         0             // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON  false
#define LORA_IQ_INVERSION_ON        false

#define BUFFER_SIZE                 128           // Buffer size
#define TX_INTERVAL_MS              6000          // Transmit every 6 seconds (slightly longer for gateway scanning)

// ============================================================================
// GLOBALS
// ============================================================================

char txpacket[BUFFER_SIZE];
uint32_t packetCount = 0;
bool lora_idle = true;
uint8_t syncMode = 0; // 0: 0x34 (->0x3434), 1: public 0x3444, 2: private 0x0741, 3: raw forced 0x3434 via registers

static RadioEvents_t RadioEvents;
void OnTxDone(void);
void OnTxTimeout(void);

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
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  ESP32 LoRa TX Test - Simple");
    Serial.println("  Heltec WiFi LoRa 32 V3");
    Serial.println("========================================");
    Serial.println();
    
    // Apply Heltec license BEFORE initializing MCU (required). Add debug.
    Serial.println("Applying Heltec license...");
    Mcu.setlicense(license, HELTEC_BOARD);
    Serial.println("License applied. Initializing MCU...");
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
    Serial.println("MCU init complete.");
    
    packetCount = 0;
    
    // Set up callbacks
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    // Initialize radio
    Serial.println("Initializing SX1262 radio...");
    Radio.Init(&RadioEvents);
    Serial.println("Radio.Init() done.");
    Radio.SetChannel(RF_FREQUENCY);
    Serial.printf("SetChannel(%d) done.\n", RF_FREQUENCY);
    // Initial sync word mode will be handled per-packet; do first apply here for clarity.
    Radio.SetSyncWord(0x34);
    Serial.println("Initial sync word request: 0x34 (ESP API). Will rotate modes per packet.\n");
    // Read back LoRa sync word registers (0x0740 MSB, 0x0741 LSB) using direct SPI driver in Heltec lib
    uint8_t msb = readReg(0x0740); // LoRa sync word MSB
    uint8_t lsb = readReg(0x0741); // LoRa sync word LSB
    Serial.printf("Readback after SetSyncWord(0x34): MSB=0x%02X LSB=0x%02X (combined 0x%02X%02X)\n", msb, lsb, msb, lsb);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
    Serial.println("SetTxConfig() done.");
    
    Serial.println("📡 LoRa Configuration:");
    Serial.printf("   Frequency:    %d MHz\n", RF_FREQUENCY / 1000000);
    Serial.printf("   TX Power:     %d dBm\n", TX_OUTPUT_POWER);
    Serial.printf("   SF:           %d\n", LORA_SPREADING_FACTOR);
    Serial.printf("   Bandwidth:    %s\n", 
                  LORA_BANDWIDTH == 0 ? "125 kHz" : 
                  LORA_BANDWIDTH == 1 ? "250 kHz" : "500 kHz");
    Serial.printf("   Coding Rate:  4/%d\n", LORA_CODINGRATE + 4);
    Serial.printf("   Preamble:     %d symbols\n", LORA_PREAMBLE_LENGTH);
    Serial.printf("   CRC:          Enabled\n");
    Serial.printf("   IQ Inversion: %s\n", LORA_IQ_INVERSION_ON ? "Yes" : "No");
    Serial.println("========================================\n");
    
    Serial.println("✅ Setup complete!");
    Serial.printf("Will transmit every %d seconds\n\n", TX_INTERVAL_MS / 1000);
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    if (lora_idle == true) {
        delay(TX_INTERVAL_MS);
        
        packetCount++;
        
        // Rotate sync word test modes BEFORE building packet
        // Mode mapping:
        // 0 -> API SetSyncWord(0x34) (expect driver writes 0x34,0x34)
        // 1 -> API SetSyncWord(0x44) then manual patch to get 0x3444 (public)
        // 2 -> API SetSyncWord(0x41) attempt to reach private 0x0741 (will observe transformation)
        // 3 -> Raw register write 0x34,0x34 bypassing helper (force exact 0x3434) then resend
        uint8_t msb, lsb;
        if(syncMode == 0) {
            Radio.SetSyncWord(0x34);
            msb = readReg(0x0740); lsb = readReg(0x0741);
            Serial.printf("[SyncMode 0] Applied SetSyncWord(0x34). Reg MSB=0x%02X LSB=0x%02X\n", msb, lsb);
        } else if(syncMode == 1) {
            Radio.SetSyncWord(0x44);
            msb = readReg(0x0740); lsb = readReg(0x0741);
            Serial.printf("[SyncMode 1] Applied SetSyncWord(0x44). Reg MSB=0x%02X LSB=0x%02X\n", msb, lsb);
        } else if(syncMode == 2) {
            Radio.SetSyncWord(0x41);
            msb = readReg(0x0740); lsb = readReg(0x0741);
            Serial.printf("[SyncMode 2] Applied SetSyncWord(0x41). Reg MSB=0x%02X LSB=0x%02X\n", msb, lsb);
        } else if(syncMode == 3) {
            // Raw write 0x34,0x34 to registers bypassing transformation
            writeReg(0x0740, 0x34);
            writeReg(0x0741, 0x34);
            msb = readReg(0x0740); lsb = readReg(0x0741);
            Serial.printf("[SyncMode 3] Raw write 0x34,0x34. Reg MSB=0x%02X LSB=0x%02X\n", msb, lsb);
        }
        syncMode = (syncMode + 1) % 4;

        // Build packet with current sync mode embedded for visibility
        snprintf(txpacket, sizeof(txpacket), 
                 "{\"test\":\"esp32\",\"packet\":%lu,\"uptime\":%lu,\"syncMode\":%u,\"regMSB\":%u,\"regLSB\":%u}", 
                 packetCount, millis() / 1000, (unsigned)( (syncMode+3) % 4 ), msb, lsb);
        
        Serial.println("\n📡 ========================================");
        Serial.printf("📡 TX Packet #%lu\n", packetCount);
        Serial.println("📡 ========================================");
    Serial.printf("Message: %s\n", txpacket);
        Serial.printf("Length:  %d bytes\n", strlen(txpacket));
        Serial.println("Transmitting...");
        uint32_t start = millis();
        Radio.Send((uint8_t *)txpacket, strlen(txpacket));
        Serial.printf("Radio.Send() invoked (elapsed pre-return %lu ms). Waiting for IRQ...\n", millis()-start);
        lora_idle = false;
    }
    
    // CRITICAL: Process radio interrupts
    Radio.IrqProcess();
    // Optional tiny heartbeat while waiting
    if(!lora_idle) {
        static uint32_t lastBeat=0; uint32_t now=millis();
        if(now - lastBeat > 750) { Serial.print("..."); lastBeat = now; }
    }
}
