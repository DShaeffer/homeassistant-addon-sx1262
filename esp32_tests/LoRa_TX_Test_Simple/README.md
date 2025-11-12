# Heltec ESP32 SX1262 Simple LoRa TX Diagnostic Example

This sketch provides a minimal transmitter plus advanced diagnostics to ensure your Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262) links correctly with a Python gateway (e.g. Home Assistant add-on).

## Features
* Periodic JSON packet (default every 6s)
* License activation for Heltec V3 boards
* Sync word rotation & register readback
* Direct SX1262 register access (0x0740 / 0x0741)
* Embedded diagnostic fields (sync mode and register bytes) in each packet
* Clear TX timing and IRQ status prints

## Why Sync Word Diagnostics?
Different libraries transform the single-byte LoRa sync word into two on-air bytes. We observed that requesting `SetSyncWord(0x34)` via Heltec's API produced register values `MSB=0x34, LSB=0x24` (not `0x34 0x34`). If the gateway listens for `0x3434`, it never detects preambles/headers.

This sketch cycles through four modes to help you discover the actual pattern:
| Mode | Action | Expected Register Bytes | Purpose |
|------|--------|-------------------------|---------|
| 0 | `SetSyncWord(0x34)` | (observed) 0x34 0x24 | Baseline Heltec transformation |
| 1 | `SetSyncWord(0x44)` | (observed) 0x44 0x24 | Confirms second byte fixed? |
| 2 | `SetSyncWord(0x41)` | (observe) ? 0x? 0x24 | Tests private vs public patterns |
| 3 | Raw write `0x34,0x34` | 0x34 0x34 | Force canonical 0x3434 |

Each packet embeds `syncMode`, `regMSB`, and `regLSB` so the gateway can log and display what was sent.

## Using the Sketch
1. Open `LoRa_TX_Test_Simple.ino` in Arduino IDE.
2. Select board: "Heltec WiFi LoRa 32 V3".
3. Verify/enter your Heltec license array (4x `uint32_t`).
4. Upload.
5. Open Serial Monitor @ 115200 baud.
6. Capture the first 4 packets and note register readbacks.

## Sample Output (Annotated)
```
Applying Heltec license...
License applied. Initializing MCU...
MCU init complete.
Initializing SX1262 radio...
SetChannel(915000000) done.
Initial sync word request: 0x34 (ESP API). Will rotate modes per packet.
Readback after SetSyncWord(0x34): MSB=0x14 LSB=0x24 (combined 0x1424)
📡 TX Packet #1
[SyncMode 0] Applied SetSyncWord(0x34). Reg MSB=0x34 LSB=0x24
Message: {"test":"esp32","packet":1,..."regMSB":52,"regLSB":36}
✅ TX Done
```

> Note: Decimal 52 = 0x34, decimal 36 = 0x24.

## Matching the Gateway
On the Python gateway, provide one of:
* `lora_sync_word_force: 0x3424` (forces register bytes 0x34 0x24)
* OR raw bytes: `lora_sync_word_msb: 0x34`, `lora_sync_word_lsb: 0x24`
* To test canonical LoRa private/public patterns, try `0x0741` or `0x3444`.

If you later lock raw registers to `0x34,0x34` (Mode 3) and the gateway listens on `0x3434` but still no packets, the Heltec library may be overriding the value post-write; keep using the force/raw pair method.

## Verifying Reception
Gateway log should start showing:
```
📡 Preamble detected!
📡 Valid header detected!
✅ LoRa RX: <N> bytes, RSSI=-XXdBm, SNR=Y.YdB
```
If not:
1. Confirm frequency (915.0 vs 868.0 etc.).
2. Ensure antennas properly seated.
3. Verify register bytes match gateway configuration.
4. Reduce distance and obstacles.
5. Lower data rate by increasing SF (e.g. 9 or 10) for testing.

## Licensing
Heltec V3 boards require the license array before `Mcu.begin()`. This example calls:
```cpp
Mcu.setlicense(license, HELTEC_BOARD);
Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
```
If omitted you may see warnings or limited functionality.

## Adjusting Parameters
Edit the defines near the top of the sketch:
```cpp
#define RF_FREQUENCY 915000000
#define TX_OUTPUT_POWER 20
#define LORA_SPREADING_FACTOR 7
#define LORA_BANDWIDTH 0      // 0->125kHz
#define LORA_CODINGRATE 1     // 1->4/5
```

## Extending This Example
* Add sensor readings and include them in the JSON.
* Implement deep sleep between transmissions for battery.
* Add ACK / simple downlink by switching to RX after TX.

## Troubleshooting Quick List
| Symptom | Action |
|---------|--------|
| No packets on gateway | Mirror actual sync registers using force/raw overrides |
| RSSI ~ -120 dBm | Antenna/connectivity or off-frequency |
| Packet CRC errors | Ensure CRC enabled on both sides |
| Header errors | Match SF/BW/CR and preamble length |

## License
MIT – adapt and share.

## Attribution
Based on Heltec examples and enhanced for diagnostic clarity in the Home Assistant LoRa gateway context.

