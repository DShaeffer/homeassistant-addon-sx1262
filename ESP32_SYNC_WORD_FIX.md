# ESP32 LoRa Sync Word Fix

## Problem
Your ESP32 and Raspberry Pi gateway have **mismatched sync words**, causing messages to be ignored.

- **Gateway**: Was expecting sync word `0x12` (decimal 18)
- **ESP32 Heltec**: Not setting sync word → using library default (likely `0x34`)

## Solution
Both devices must use the **same sync word: `0x34`** (standard LoRa private network)

---

## ✅ Gateway Fix (DONE)
I've updated the gateway code and config to use sync word `0x34`.

**File Changes:**
- `config.yaml`: Changed default from `0x12` to `0x34`
- `lora_gateway.py`: Updated to parse hex strings properly

---

## 🔧 ESP32 Fix (YOU NEED TO DO THIS)

### Option 1: Use Radio.SetSyncWord() [RECOMMENDED]

Add this line in your `initLoRa()` function **AFTER** `Radio.Init()` and **BEFORE** `Radio.SetTxConfig()`:

```cpp
void initLoRa() {
    if (loraInitialized) return;
    
    Serial.println("\n📡 Initializing LoRa (SX1262)...");
    
    // Set up callbacks
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    
    // Initialize radio
    Radio.Init(&RadioEvents);
    
    // *** ADD THIS LINE ***
    // Set sync word to 0x34 (standard LoRa private network)
    Radio.SetSyncWord(0x34);
    
    // Configure TX parameters
    Radio.SetTxConfig(
        MODEM_LORA,                     // Modem type
        LORA_TX_POWER,                  // Power in dBm
        // ... rest of config
```

### Option 2: Use Low-Level SX126x Commands

If `Radio.SetSyncWord()` doesn't exist in your Heltec library, use this alternative:

```cpp
// Add after Radio.Init(&RadioEvents);

// Write sync word registers directly (SX126x specific)
// For sync word 0x34:
//   MSB = (0x34 & 0xF0) | 0x04 = 0x34
//   LSB = (0x34 << 4) | 0x04 = 0x344
uint8_t syncWord[2];
syncWord[0] = 0x34;  // MSB
syncWord[1] = 0x44;  // LSB
Radio.Write(0x0740, syncWord, 2);  // REG_LORA_SYNC_WORD_MSB = 0x0740
```

---

## 📋 Updated Configuration

### Home Assistant Add-on Config:
```yaml
lora_frequency: 915.0
lora_spreading_factor: 7
lora_bandwidth: 125000
lora_coding_rate: 5
lora_sync_word: 0x34          # ← Changed from 0x12
lora_tx_power: 20
```

### ESP32 Config (after adding SetSyncWord):
```cpp
#define LORA_FREQUENCY 915000000      // ✅ Matches
#define LORA_TX_POWER 20              // ✅ Matches
#define LORA_SPREADING_FACTOR 7       // ✅ Matches
#define LORA_BANDWIDTH 0              // ✅ Matches (0 = 125 kHz)
#define LORA_CODINGRATE 1             // ✅ Matches (1 = 4/5)
// NEW: Sync word set via Radio.SetSyncWord(0x34) in code
```

---

## 🧪 Testing Steps

1. **Update ESP32 code**: Add `Radio.SetSyncWord(0x34);` to `initLoRa()`
2. **Upload to ESP32**: Flash the updated code
3. **Update Home Assistant**: 
   - Go to Settings → Add-ons → SX1262 LoRa Gateway
   - Click "Configuration" tab
   - Change `lora_sync_word` from `18` to `0x34`
   - Click "Save"
   - Restart the add-on
4. **Monitor logs**: Go to "Log" tab in the add-on
5. **Send test message**: Press reset on ESP32 or wait for next sensor transmission
6. **Look for**: `LoRa RX: XXX bytes, RSSI=...dBm, SNR=...dB`

---

## 🔍 Why Sync Word Matters

The **sync word** is like a "channel ID" for LoRa packets:
- Receiver **only accepts** packets with matching sync word
- Packets with wrong sync word are **silently ignored** (no error!)
- Standard values:
  - `0x34` = LoRa private network (our choice)
  - `0x12` = Custom/non-standard (old config)
  - `0x3444` = LoRaWAN public network

---

## 📚 Reference: LoRa Parameter Mapping

| Parameter | ESP32 Heltec | Gateway (Waveshare) |
|-----------|--------------|---------------------|
| Frequency | 915000000 Hz | 915.0 MHz |
| Spreading Factor | 7 | 7 |
| Bandwidth | 0 (enum: 125kHz) | 125000 (Hz) |
| Coding Rate | 1 (enum: 4/5) | 5 (denominator) |
| Sync Word | 0x34 (via SetSyncWord) | 0x34 (hex string) |
| TX Power | 20 dBm | 20 dBm |

The Heltec library uses **enums** (0, 1, 2...) while Waveshare uses **actual values**.
Both are correct - just different APIs!

---

## ✅ Expected Result

After the fix, you should see in the Home Assistant add-on logs:

```
2025-11-10 12:34:56 - INFO - Gateway ready! Listening for LoRa messages...
2025-11-10 12:35:12 - INFO - LoRa RX: 287 bytes, RSSI=-45dBm, SNR=9.5dB
2025-11-10 12:35:12 - INFO - Published sensor data: Level=42cm, Battery=3.75V
```

Good luck! 🎉
