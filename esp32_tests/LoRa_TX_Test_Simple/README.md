# Simple LoRa TX Test

## Purpose
Minimal LoRa transmitter to verify basic functionality.

## What it does
- Transmits a JSON message every 5 seconds
- No sensor reading, no display, no deep sleep
- Just pure LoRa transmission

## How to use
1. Open this folder in Arduino IDE (File → Open → `LoRa_TX_Test_Simple.ino`)
2. Select board: "Heltec WiFi LoRa 32 V3"
3. Upload
4. Open Serial Monitor (115200 baud)
5. Watch for "✅ TX Done" messages

## Expected output
```
========================================
  ESP32 LoRa TX Test - Simple
  Heltec WiFi LoRa 32 V3
========================================

📡 Initializing LoRa Radio (SX1262)
✅ Radio.Init() complete
✅ Sync Word: 0x34
...
📡 TX Packet #1
Message: {"test":"esp32","packet":1,"uptime":5,"rssi":0}
✅ TX Done
```

## What to check on gateway
After uploading this, check Home Assistant gateway logs for:
- "📡 Preamble detected!"
- "📡 Valid header detected!"
- "✅ LoRa RX: XX bytes"

If you see these, communication is working!
