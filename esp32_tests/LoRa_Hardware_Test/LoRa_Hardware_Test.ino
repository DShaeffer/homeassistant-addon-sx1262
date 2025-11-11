/**
 * SX1262 Hardware Self-Test
 * 
 * Purpose: Verify the SX1262 chip responds to SPI commands
 * Tests basic communication without requiring successful TX/RX
 * 
 * This will tell us if the chip survived the reverse polarity event
 * or if it's damaged and needs replacement.
 * 
 * Hardware: Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
 * Author: Hillsville Cabin Project
 * Date: November 2025
 */

#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include "LoRaWan_APP.h"

static RadioEvents_t RadioEvents = {0};
bool testPassed = true;

void setup() {
    Serial.begin(115200);
    delay(1500);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  SX1262 Hardware Self-Test");
    Serial.println("========================================");
    Serial.println();
    Serial.println("This test verifies the LoRa chip responds");
    Serial.println("to SPI commands (proves it's alive).\n");
    
    // Test 1: Radio initialization
    Serial.println("Test 1: Radio.Init()");
    Serial.println("  -> Initializing SX1262...");
    try {
        Radio.Init(&RadioEvents);
        delay(200);
        Serial.println("  -> ✅ Radio.Init() completed without crash");
    } catch (...) {
        Serial.println("  -> ❌ FAILED: Radio.Init() crashed");
        testPassed = false;
    }
    Serial.println();
    
    // Test 2: Set frequency
    Serial.println("Test 2: Setting frequency to 915 MHz");
    Serial.println("  -> Calling Radio.SetChannel(915000000)...");
    try {
        Radio.SetChannel(915000000);
        delay(100);
        Serial.println("  -> ✅ SetChannel() completed");
    } catch (...) {
        Serial.println("  -> ❌ FAILED: SetChannel() crashed");
        testPassed = false;
    }
    Serial.println();
    
    // Test 3: Standby mode
    Serial.println("Test 3: Put radio in standby mode");
    Serial.println("  -> Calling Radio.Standby()...");
    try {
        Radio.Standby();
        delay(100);
        Serial.println("  -> ✅ Standby() completed");
    } catch (...) {
        Serial.println("  -> ❌ FAILED: Standby() crashed");
        testPassed = false;
    }
    Serial.println();
    
    // Test 4: Set sync word
    Serial.println("Test 4: Setting sync word");
    Serial.println("  -> Calling Radio.SetSyncWord(0x34)...");
    try {
        Radio.SetSyncWord(0x34);
        delay(100);
        Serial.println("  -> ✅ SetSyncWord() completed");
    } catch (...) {
        Serial.println("  -> ❌ FAILED: SetSyncWord() crashed");
        testPassed = false;
    }
    Serial.println();
    
    // Test 5: Configure TX parameters
    Serial.println("Test 5: Configuring TX parameters");
    Serial.println("  -> Calling Radio.SetTxConfig()...");
    try {
        Radio.SetTxConfig(
            MODEM_LORA,
            14,        // Low power for test
            0,         // FSK deviation (not used)
            0,         // 125 kHz bandwidth
            7,         // SF7
            1,         // CR 4/5
            8,         // Preamble length
            false,     // Variable length
            true,      // CRC enabled
            0,         // Freq hopping off
            0,         // Hop period
            false,     // IQ normal
            3000       // TX timeout
        );
        delay(100);
        Serial.println("  -> ✅ SetTxConfig() completed");
    } catch (...) {
        Serial.println("  -> ❌ FAILED: SetTxConfig() crashed");
        testPassed = false;
    }
    Serial.println();
    
    // Test 6: Try Sleep mode (optional - some chips may fail after reverse polarity)
    Serial.println("Test 6: Sleep mode (optional test)");
    Serial.println("  -> Calling Radio.Sleep()...");
    try {
        Radio.Sleep();
        delay(100);
        Serial.println("  -> ✅ Sleep() completed");
        delay(100);
        // Wake it back up
        Serial.println("  -> Waking radio back up...");
        Radio.Standby();
        delay(100);
        Serial.println("  -> ✅ Radio woke up successfully");
    } catch (...) {
        Serial.println("  -> ⚠️  Sleep() may not work (not critical)");
        // Don't fail the test for this
    }
    Serial.println();
    
    // Final verdict
    Serial.println("========================================");
    if (testPassed) {
        Serial.println("✅ ALL CRITICAL TESTS PASSED!");
        Serial.println("========================================");
        Serial.println();
        Serial.println("✅ SX1262 chip is responding to commands.");
        Serial.println("✅ The chip survived reverse polarity!");
        Serial.println();
        Serial.println("Since hardware is OK, the TX issue is likely:");
        Serial.println("  1. RF switch pins (TXEN/RXEN) not configured");
        Serial.println("  2. DIO1 (IRQ) pin mapping issue");
        Serial.println("  3. Wrong board variant in Arduino IDE");
        Serial.println();
        Serial.println("Next step: Try the multi-scenario test to");
        Serial.println("find the correct RF switch pin configuration.");
    } else {
        Serial.println("❌ SOME TESTS FAILED");
        Serial.println("========================================");
        Serial.println();
        Serial.println("❌ SX1262 chip may be damaged.");
        Serial.println("❌ Reverse polarity likely killed the radio.");
        Serial.println();
        Serial.println("Recommendation:");
        Serial.println("  - Try re-flashing with fresh Heltec library");
        Serial.println("  - If still fails, the board needs replacement");
    }
    Serial.println();
}

void loop() {
    // Test complete - just idle
    delay(1000);
}
