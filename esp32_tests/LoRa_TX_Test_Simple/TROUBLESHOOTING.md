# Heltec WiFi LoRa 32 V3 - TX & Sync Word Troubleshooting

## Problem A: TX Never Completes (Timeouts at 3-4 seconds)

If you're seeing this output:
```
⏳ Waiting for TX complete... 500 ms
⏳ Waiting for TX complete... 1000 ms
...
⏳ Waiting for TX complete... 3500 ms
❌ TX did not complete within expected window
```

The radio is trying to transmit but the `OnTxDone()` callback is never firing.

---

## Solution Checklist

### ✅ 1. Verify Correct Board Selection in Arduino IDE

**THIS IS THE MOST COMMON ISSUE!**

Go to: **Tools → Board → Heltec ESP32 Arduino → WiFi LoRa 32(V3)**

**MUST be V3, not V2!** The V2 and V3 have different:
- ESP32 chips (V2 = ESP32, V3 = ESP32-S3)
- LoRa chips (V2 = SX1276, V3 = SX1262)
- Pin mappings

If you select the wrong board, the DIO1 interrupt pin will be wrong and callbacks won't fire.

---

### ✅ 2. Install Latest Heltec ESP32 Board Library

1. In Arduino IDE: **Tools → Board → Boards Manager**
2. Search for **"Heltec ESP32"**
3. Install the **latest version** (should be 0.0.7 or newer for V3 support)
4. Restart Arduino IDE after installing

---

### ✅ 3. Check USB Cable and Port Selection

- Use a **data cable** (not charge-only)
- Select correct COM port: **Tools → Port**
- Upload speed: Try **921600** or **115200** if having issues

---

### ✅ 4. Verify Antenna Connection

The SX1262 has built-in PA (Power Amplifier) protection, but:
- **Connect antenna BEFORE powering on**
- Use a proper 915 MHz antenna (not 868 MHz or 433 MHz)
- Check antenna connector is secure

Without an antenna, the radio may timeout due to high VSWR.

---

### ✅ 5. Power Supply Issues

The Heltec V3 can draw 120-150 mA when transmitting at 20 dBm.

**If powered by USB:**
- Use a good quality USB cable
- Try a powered USB hub
- Some laptop USB ports don't provide enough current

**If powered by battery:**
- Check voltage is 3.7-4.2V (single LiPo) or 5V (regulated)
- Ensure battery can supply at least 200 mA

---

### ✅ 6. Try Lower TX Power First

Edit the code to reduce power while testing:

```cpp
#define LORA_TX_POWER 14              // Try 14 dBm instead of 20
```

This reduces current draw from ~150 mA to ~50 mA. If this fixes it, you have a power supply issue.

---

### ✅ 7. Factory Reset / Re-flash

Sometimes the ESP32 flash gets corrupted:

1. **Erase flash:** Tools → Erase Flash → "All Flash Contents"
2. **Re-upload** the sketch
3. **Power cycle** the board (unplug/replug USB)

---

## Problem B: Packets Never Received by Gateway (No IRQ / No Preamble)

### Root Causes
| Cause | Symptom | Fix |
|-------|---------|-----|
| Sync word transform | Gateway expecting 0x3434, ESP sending 0x3424 | Read ESP regs 0x0740/0x0741; set gateway force override 0x3424 |
| Parameter mismatch | No header detected | Match SF/BW/CR/preamble exactly |
| Off frequency | RSSI stays ~ -120 dBm | Verify center freq & region; adjust antenna |
| Antenna / RF path | No RSSI fluctuation | Reseat antennas; check board orientation |
| Power instability | Random hangs | Lower TX power to 14 dBm, stable USB source |

### Sync Word Diagnostic Table
| API Request | Observed MSB | Observed LSB | Combined | Notes |
|-------------|--------------|--------------|---------|-------|
| SetSyncWord(0x34) | 0x34 | 0x24 | 0x3424 | Heltec transform (private variant) |
| SetSyncWord(0x44) | 0x44 | 0x24 | 0x4424 | Second byte fixed to 0x24 |
| Raw write 0x34,0x34 | 0x34 | 0x34 | 0x3434 | Forced canonical (may be overridden later) |

> If you see MSB=0x14 initially, ignore first transient read; subsequent per-packet rotation is authoritative.

### Gateway Overrides
Add to gateway add-on options (one method):
```yaml
lora_sync_word_force: 0x3424
```
Or:
```yaml
lora_sync_word_msb: 0x34
lora_sync_word_lsb: 0x24
```

### Verifying Alignment
1. ESP32 Serial: shows per packet `[SyncMode X] ... Reg MSB=0x.. LSB=0x..`
2. Gateway Log: prints `Sync word applied: ...` plus readback `MSB=0x.. LSB=0x..`
3. Once matched, expect: `📡 Preamble detected!`, `📡 Valid header detected!`, then `✅ LoRa RX: ...`

### If Still Silent
1. Increase SF to 9 or 10.
2. Temporarily widen BW to 250000.
3. Take instantaneous RSSI (`RSSIinst`) – should fluctuate when ESP transmits.
4. Use Mode 3 raw write forcing 0x3434 and set gateway to 0x3434 to test canonical pattern.
5. If only one pattern works, document that pattern for sharing.

## Advanced TX Debugging

### Check if Radio.Init() Worked

Add this after `Radio.Init(&RadioEvents);`:

```cpp
Radio.Standby();
delay(100);
Serial.println("Radio responding to commands...");
```

If this crashes or hangs, the SPI bus isn't working (hardware issue).

### Test with Different Spreading Factor

Higher SF = longer airtime = more reliable:

```cpp
#define LORA_SPREADING_FACTOR 10       // Try SF10 instead of SF7
```

SF7 airtime ≈ 40 ms, SF10 airtime ≈ 370 ms

---

## Hardware Issues (Rare)

If **NONE of the above work**, you may have:

1. **Damaged SX1262 chip**
   - Did you connect power backwards?
   - Did you short any pins?
   - Did you apply >5V to any GPIO?

2. **Clone/Fake board**
   - Some cheap clones have wrong schematics
   - RF switch may not be wired to DIO2
   - Return and buy from official source

3. **Defective board from factory**
   - Try a different V3 board if available
   - Contact Heltec support with serial number

---

## What the Updated Code Does

The latest version of `LoRa_TX_Test_Simple.ino` includes:

1. **Proper initialization order:**
   - `Radio.Init()` → `Radio.Standby()` → `SetSyncWord()` → `SetChannel()` → `SetTxConfig()`

2. **Better debugging:**
   - Shows elapsed time in callbacks
   - Periodic status updates during TX

3. **Proper cleanup on timeout:**
   - Forces standby mode
   - Resets RF switch to RX

4. **RF switch management:**
   - Sets TX/RX switch pins if needed (Heltec V3 uses DIO2 internally)

---

## Expected Good TX Output

When working correctly, you should see:

```
📡 TX Packet #1
Message: {"test":"esp32","packet":1,"uptime":5,"rssi":0}
Length:  47 bytes
Transmitting...
✅ TX Done (took 56 ms)

[5 second delay]

📡 TX Packet #2
Message: {"test":"esp32","packet":2,"uptime":10,"rssi":0}
Length:  48 bytes
Transmitting...
✅ TX Done (took 57 ms)
```

Typical TX time for SF7, 125kHz, ~50 byte payload = **40-60 ms**

---

## Still Not Working After Sync Fix?

1. **Try the Hardware Test sketch** first to verify SPI communication
2. **Try the Multi-Scenario Diagnostic** to test different configurations
3. **Check the Heltec forums** for similar issues
4. **Open an issue** with full Serial Monitor output

---

## Board Info

Heltec WiFi LoRa 32 V3 Specs:
- **CPU:** ESP32-S3FN8 (dual-core, 240 MHz)
- **LoRa:** Semtech SX1262
- **Frequency:** 863-928 MHz (hardware dependent)
- **TX Power:** Up to +22 dBm (150 mW)
- **Range:** 2-5 km (line of sight)

Pinout (for reference):
- SX1262 CS:   GPIO 8
- SX1262 BUSY: GPIO 13
- SX1262 DIO1: GPIO 14
- SX1262 RST:  GPIO 12
- LED:         GPIO 35
