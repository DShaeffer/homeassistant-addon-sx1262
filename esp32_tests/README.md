# ESP32 LoRa Testing Guide

## Problem Summary
Gateway receives NO LoRa packets from ESP32, despite both devices initializing successfully.

## What We've Fixed
1. ✅ All LoRa parameters matched (frequency, SF, BW, CR)
2. ✅ Sync word standardized to 0x34
3. ✅ Packet parameters matched (preamble=8, explicit header, CRC, IQ)
4. ✅ Gateway sync word format corrected (0x3434 vs 0x3444)

## Current Status - v1.3.4
**Gateway waiting for 0x3434 sync word**
**ESP32 transmitting with 0x34 sync word** (which *should* expand to 0x3434)

## Testing Steps

### 1. Upload Simple Test Sketch
Use `LoRa_TX_Test_Simple.ino` - this eliminates all complexity:
- No sensor reading
- No deep sleep
- No display updates
- Just pure LoRa transmission every 5 seconds

**Upload this first to verify basic LoRa functionality!**

### 2. Monitor Serial Output
Look for:
```
✅ Radio.Init() complete
✅ Sync Word: 0x34
✅ Radio.SetTxConfig() complete
✅ Frequency: 915000000 Hz
📡 TX Packet #1
Message: {"test":"esp32","packet":1,"uptime":5,"rssi":0}
Length:  XX bytes
Transmitting...
✅ TX Done
```

### 3. Check Gateway Logs (v1.3.4)
Should see:
```
2025-11-10 XX:XX:XX - INFO - Setting sync word to 0x3434 (ESP32 format: 0x34 in both bytes)...
2025-11-10 XX:XX:XX - INFO - LoRa configured:
2025-11-10 XX:XX:XX - INFO -   Sync Word: 0x3434 (ESP32 format)
2025-11-10 XX:XX:XX - INFO - 💓 Heartbeat - Listening... (checked 0 packets so far)
```

If packets received:
```
2025-11-10 XX:XX:XX - INFO - 📡 Preamble detected!
2025-11-10 XX:XX:XX - INFO - 📡 Valid header detected!
2025-11-10 XX:XX:XX - INFO - ✅ LoRa RX: XX bytes, RSSI=-XXdBm, SNR=XXdB
```

### 4. If Still No Reception

#### Check Physical Setup
- [ ] ESP32 antenna connected to SMA connector
- [ ] Raspberry Pi antenna connected to HAT
- [ ] Both antennas are 915MHz rated (not 868MHz or 433MHz!)
- [ ] Devices within 25 feet line-of-sight

#### Verify Hardware
- [ ] Waveshare HAT is 915MHz variant (check product label/order)
- [ ] Green LED on HAT indicates power
- [ ] No loose connections on GPIO pins

#### Test with SDR (Optional)
If you have an RTL-SDR dongle:
```bash
# Tune to 915 MHz and watch for transmissions
rtl_sdr -f 915000000 -s 250000 -g 20 - | python plot_spectrum.py
```

#### Debug Sync Word on ESP32
The Heltec `Radio.SetSyncWord(0x34)` *should* set both bytes to 0x34.

But if the library has a bug, it might be setting 0x3444 instead!

**Try this in the test sketch:**
```cpp
// In initLoRa(), after Radio.SetSyncWord(0x34), add:
Serial.println("\n🔍 Verifying sync word setting...");
// Option 1: Try setting as 16-bit explicitly
Radio.SetSyncWord(0x3434);  // Force both bytes
Serial.println("✅ Set to 0x3434 explicitly");
```

## Next Steps

1. **Upload LoRa_TX_Test_Simple.ino** to ESP32
2. **Update gateway to v1.3.4** in Home Assistant
3. **Restart both devices**
4. **Watch for preamble detection in gateway logs**

If you see "📡 Preamble detected!" but CRC errors, we're VERY close - means radios are hearing each other but have a parameter mismatch.

If you see NO IRQ activity at all, likely a hardware or sync word issue.

## Sync Word Theory

### SX126x Sync Word Format
The SX126x chip uses a **2-byte sync word** (MSB + LSB):
- Public network: `0x3444`
- Private network: `0x1424` (default)
- Custom: Any 16-bit value

### Library Implementations

**Waveshare LoRaRF (Gateway):**
```python
def setSyncWord(self, syncWord: int):
    if syncWord <= 0xFF:  # If single byte
        buf = (
            (syncWord & 0xF0) | 0x04,   # MSB
            (syncWord << 4) | 0x04      # LSB
        )
    # Example: 0x34 → 0x3444
```

**Heltec Radio API (ESP32):**
```cpp
void Radio.SetSyncWord(uint8_t syncWord);
// Expected behavior: 0x34 → 0x3434 (both bytes = 0x34)
// But we need to VERIFY this!
```

### The Critical Question
Does Heltec `SetSyncWord(0x34)` actually set `0x3434` or `0x3444`?

**Test this by trying both in the simple sketch!**

