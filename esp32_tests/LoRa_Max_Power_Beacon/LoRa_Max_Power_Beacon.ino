/**
 * LoRa Maximum Power Beacon Test
 * 
 * Purpose: Transmit at maximum power with longest airtime
 * to give the best chance of detection by gateway or SDR
 * 
 * This is a last-resort test to see if ANY RF is leaving the board
 */

#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include "LoRaWan_APP.h"

static RadioEvents_t RadioEvents;
bool txInProgress = false;
uint32_t txStart = 0;
uint32_t beaconCount = 0;

void OnTxDone(void) {
    Serial.println("   -> ✅ TX Done callback!");
    txInProgress = false;
}

void OnTxTimeout(void) {
    Serial.println("   -> ⚠️ TX Timeout callback!");
    txInProgress = false;
}

void OnRxDone(uint8_t*, uint16_t, int16_t, int8_t) {}
void OnRxTimeout(void) {}
void OnRxError(void) {}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("  Maximum Power Beacon Test");
    Serial.println("========================================\n");
    Serial.println("This test transmits at maximum settings:");
    Serial.println("  - SF12 (longest symbols)");
    Serial.println("  - 125 kHz bandwidth");
    Serial.println("  - 22 dBm TX power (maximum)");
    Serial.println("  - ~200 byte payload");
    Serial.println("  - Airtime: ~2 seconds per packet\n");
    Serial.println("If gateway doesn't detect THIS, RF path is dead.\n");
    
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    
    Radio.Init(&RadioEvents);
    Radio.SetChannel(915000000);
    Radio.SetSyncWord(0x34);
    
    // Maximum visibility settings
    Radio.SetTxConfig(
        MODEM_LORA,
        22,        // Maximum power
        0,
        0,         // 125 kHz
        12,        // SF12 (slowest, longest range)
        1,         // CR 4/5
        8,         // Preamble
        false,
        true,
        0,
        0,
        false,
        5000       // Long timeout
    );
    
    Serial.println("Radio configured. Starting beacon...\n");
    delay(1000);
}

void loop() {
    Radio.IrqProcess();
    
    if (!txInProgress) {
        beaconCount++;
        
        // Create long payload for max airtime
        char msg[256];
        snprintf(msg, sizeof(msg),
                 "{\"beacon\":1,\"count\":%lu,\"uptime\":%lu,"
                 "\"sf\":12,\"pwr\":22,\"freq\":915000000,"
                 "\"padding\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                 "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
                 "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\"}",
                 (unsigned long)beaconCount, (unsigned long)(millis()/1000));
        
        Serial.println("========================================");
        Serial.printf("Beacon #%lu - SF12, 22dBm, ~2sec airtime\n", (unsigned long)beaconCount);
        Serial.println("========================================");
        Serial.printf("Payload: %u bytes\n", (unsigned)strlen(msg));
        Serial.println("Transmitting...");
        
        uint8_t buf[256];
        size_t len = strlen(msg);
        memcpy(buf, msg, len);
        
        txInProgress = true;
        txStart = millis();
        Radio.Send(buf, len);
        
        delay(100);
    }
    
    if (txInProgress) {
        uint32_t elapsed = millis() - txStart;
        if (elapsed > 1000 && (elapsed % 1000) < 20) {
            Serial.printf("   -> Waiting... %lu ms\n", (unsigned long)elapsed);
        }
        if (elapsed > 5500) {
            Serial.println("   -> ❌ Manual timeout (no callback)\n");
            Radio.Standby();
            txInProgress = false;
            delay(3000); // Wait before next beacon
        }
    }
    
    delay(10);
}
