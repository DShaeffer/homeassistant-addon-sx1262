# Sync Word Diagnostic Test

## Purpose
Test different sync word values to determine which format the Heltec library uses.

## What it does
Cycles through 3 different sync word values, transmitting with each:
1. 0x34 (expected to work with gateway 0x3434)
2. 0x12 (alternate test)
3. 0x44 (to test if gateway is actually listening for 0x3444)

Each test runs for 10 seconds before switching to the next.

## How to use
1. Open this folder in Arduino IDE (File → Open → `LoRa_TX_Test_SyncWord_Diagnostic.ino`)
2. Select board: "Heltec WiFi LoRa 32 V3"
3. Upload
4. Open Serial Monitor (115200 baud)
5. Watch which sync word causes the gateway to receive packets

## Expected output
```
========================================
  LoRa Sync Word Diagnostic Test
========================================
This test will transmit with different sync words:
1. Sync Word 0x34 (default custom)
2. Sync Word 0x12 (alternate test)
3. Sync Word 0x44 (to test 0x3444)

📡 Configuring LoRa with Sync Word: 0x34
📡 Test #1: Sync Word 0x34
Message: {"test":"sync_word","sync":"0x34","test_num":1,"uptime":5}
✅ TX Done

Waiting 10 seconds before next test...

📡 Configuring LoRa with Sync Word: 0x12
...
```

## What this reveals
Check the gateway logs to see **which sync word it receives**:

- **If 0x34 works**: Gateway fix (v1.3.4) is correct
- **If 0x44 works**: Heltec library uses 0x3444 format, gateway needs adjustment
- **If 0x12 works**: Unexpected! Need to investigate further
- **If none work**: Hardware/physical issue (antennas, distance, frequency)
