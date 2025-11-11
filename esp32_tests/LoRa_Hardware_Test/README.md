# LoRa Hardware Self-Test

## Purpose
Verifies the SX1262 chip can respond to basic SPI commands. This determines if the chip survived the reverse polarity event.

## What it tests
1. Radio.Init() - Basic SPI communication
2. SetChannel() - Frequency register writes
3. Standby() - Mode switching
4. SetSyncWord() - Configuration registers
5. SetTxConfig() - Parameter configuration
6. Sleep/Wake cycle (optional)

## How to use
1. Open `LoRa_Hardware_Test.ino` in Arduino IDE
2. Select board: Heltec WiFi LoRa 32 V3
3. Upload
4. Open Serial Monitor at 115200 baud
5. Watch the test results

## Expected output

### If chip is OK:
```
✅ ALL CRITICAL TESTS PASSED!
✅ SX1262 chip is responding to commands.
✅ The chip survived reverse polarity!
```

### If chip is damaged:
```
❌ SOME TESTS FAILED
❌ SX1262 chip may be damaged.
❌ Reverse polarity likely killed the radio.
```

## What each result means

**All tests pass:**
- The SX1262 chip is alive and communicating via SPI
- The issue is configuration (RF switch pins, IRQ mapping, etc.)
- Next step: Run the multi-scenario diagnostic to find working config

**Any test fails:**
- The SX1262 chip is damaged
- Most likely from the reverse polarity event
- Board needs replacement - the LoRa section is dead

## Next steps

If tests pass:
- Upload `LoRa_TX_MultiScenario_Diagnostic.ino` to find the correct RF switch pins
- Once found, update the simple test with those pins
- Your sensor bridge should work after that

If tests fail:
- Contact the seller for a replacement
- The reverse polarity damaged the SX1262 or its support circuitry
