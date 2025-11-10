/**
 * LoRa Sync Word Diagnostic Test
 * 
 * Purpose: Test BOTH possible sync word interpretations
 * 
 * This sketch will transmit with BOTH:
 * 1. SetSyncWord(0x34) - default Heltec method
 * 2. Public network mode (if available)
 * 
 * Hardware: Heltec WiFi LoRa 32 V3
 */

#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include "LoRaWan_APP.h"

// Test configuration
#define LORA_FREQUENCY 915000000
#define LORA_TX_POWER 20
#define LORA_SPREADING_FACTOR 7
#define LORA_BANDWIDTH 0
#define LORA_CODINGRATE 1
#define LORA_PREAMBLE_LENGTH 8
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

// Optional RF switch pins for clone boards
#ifndef LORA_TXEN_PIN
#define LORA_TXEN_PIN -1
#endif
#ifndef LORA_RXEN_PIN
#define LORA_RXEN_PIN -1
#endif

static RadioEvents_t RadioEvents;
bool txInProgress = false;
int currentTest = 0;
uint32_t lastTxTime = 0;
uint32_t txStartTime = 0;

// RF switch helpers
void rfSwitchInit() {
    if (LORA_TXEN_PIN >= 0) pinMode(LORA_TXEN_PIN, OUTPUT);
    if (LORA_RXEN_PIN >= 0) pinMode(LORA_RXEN_PIN, OUTPUT);
}
void rfSwitchToRx() {
    if (LORA_TXEN_PIN >= 0) digitalWrite(LORA_TXEN_PIN, LOW);
    if (LORA_RXEN_PIN >= 0) digitalWrite(LORA_RXEN_PIN, HIGH);
}
void rfSwitchToTx() {
    if (LORA_RXEN_PIN >= 0) digitalWrite(LORA_RXEN_PIN, LOW);
    if (LORA_TXEN_PIN >= 0) digitalWrite(LORA_TXEN_PIN, HIGH);
}

void OnTxDone(void) {
    Serial.println("✅ TX Done\n");
    txInProgress = false;
}

void OnTxTimeout(void) {
    Serial.println("⚠️  TX Timeout\n");
    txInProgress = false;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {}
void OnRxTimeout(void) {}
void OnRxError(void) {}

void initLoRa(uint8_t syncWord) {
    Serial.println("\n========================================");
    Serial.printf("📡 Configuring LoRa with Sync Word: 0x%02X\n", syncWord);
    Serial.println("========================================");
    
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    
    Radio.Init(&RadioEvents);
    Radio.SetSyncWord(syncWord);
    
    Radio.SetTxConfig(
        MODEM_LORA,
        LORA_TX_POWER,
        0,
        LORA_BANDWIDTH,
        LORA_SPREADING_FACTOR,
        LORA_CODINGRATE,
        LORA_PREAMBLE_LENGTH,
        LORA_FIX_LENGTH_PAYLOAD_ON,
        true,
        0,
        0,
        LORA_IQ_INVERSION_ON,
        3000
    );
    
    Radio.SetChannel(LORA_FREQUENCY);
    
    rfSwitchInit();
    rfSwitchToRx();

    Serial.printf("✅ LoRa ready: 915MHz, SF7, BW125, Sync=0x%02X\n", syncWord);
}

void transmitTest(uint8_t syncWord, int testNumber) {
    char message[128];
    snprintf(message, sizeof(message), 
             "{\"test\":\"sync_word\",\"sync\":\"0x%02X\",\"test_num\":%d,\"uptime\":%lu}", 
             syncWord, testNumber, millis() / 1000);
    
    uint8_t txBuffer[128];
    int len = strlen(message);
    memcpy(txBuffer, message, len);
    
    Serial.println("📡 ========================================");
    Serial.printf("📡 Test #%d: Sync Word 0x%02X\n", testNumber, syncWord);
    Serial.println("📡 ========================================");
    Serial.printf("Message: %s\n", message);
    Serial.printf("Length:  %d bytes\n", len);
    Serial.println("Transmitting...");
    
    rfSwitchToTx();
    txInProgress = true;
    txStartTime = millis();
    Radio.Send(txBuffer, len);
    
    // Wait a bit for transmission to start
    delay(50);
    
    // Note: OnTxDone() callback will be called by Radio.IrqProcess() in loop()
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  LoRa Sync Word Diagnostic Test");
    Serial.println("========================================");
    Serial.println();
    Serial.println("This test will transmit with different sync words:");
    Serial.println("1. Sync Word 0x34 (default custom)");
    Serial.println("2. Sync Word 0x12 (alternate test)");
    Serial.println("3. Sync Word 0x44 (to test 0x3444)");
    Serial.println();
    Serial.println("Check which one the gateway receives!");
    Serial.println("========================================\n");
    
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    
    lastTxTime = millis();
}

void loop() {
    static int testCycle = 0;
    
    // CRITICAL: Process radio events to service callbacks
    Radio.IrqProcess();

    // Monitor TX progress and recover if stuck
    if (txInProgress) {
        uint32_t elapsed = millis() - txStartTime;
        if (elapsed > 500 && (elapsed % 500) < 15) {
            Serial.printf("⏳ Waiting for TX complete... %lu ms (sync test)\n", elapsed);
        }
        if (elapsed > 3500) {
            Serial.println("❌ TX stuck beyond timeout. Forcing standby (sync test).\n");
            Radio.Standby();
            txInProgress = false;
            rfSwitchToRx();
        }
    }
    
    if (!txInProgress && millis() - lastTxTime >= 10000) {
        testCycle++;
        
        uint8_t testSyncWords[] = {0x34, 0x12, 0x44};
        int syncIndex = (testCycle - 1) % 3;
        uint8_t syncWord = testSyncWords[syncIndex];
        
        // Reinitialize with new sync word
        initLoRa(syncWord);
        
        // Transmit test packet
        transmitTest(syncWord, testCycle);
        
        Serial.println("\nWaiting 10 seconds before next test...\n");
        lastTxTime = millis();
    }
    
    delay(10);
}
