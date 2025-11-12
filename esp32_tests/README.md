# ESP32 LoRa Test Examples

This folder contains example Arduino sketches for testing LoRa communication with the SX1262 Gateway Home Assistant add-on.

## Recommended Example

### [Heltec_Gateway_Test](Heltec_Gateway_Test/)
**Use this first!** A clean, minimal example for testing the gateway:
- Ready-to-use with Heltec WiFi LoRa 32 V3 boards
- Simple JSON payload generation
- Clear serial output
- Complete documentation

## Development/Diagnostic Examples

### [LoRa_TX_Test_Simple](LoRa_TX_Test_Simple/)
Simplified test with diagnostic features:
- Basic LoRa transmission
- Sync word diagnostics and register readback
- Useful for troubleshooting sync word issues
- Includes troubleshooting guide

### [LoRa_Hardware_Test](LoRa_Hardware_Test/)
Hardware validation sketch:
- Tests SX1262 radio initialization
- Verifies LoRa parameters
- Antenna and signal testing

### [LoRa_TX_Test_SyncWord_Diagnostic](LoRa_TX_Test_SyncWord_Diagnostic/)
Advanced sync word testing:
- Rotates through different sync word values
- Register-level diagnostics
- For resolving compatibility issues

### [LoRa_Max_Power_Beacon](LoRa_Max_Power_Beacon/)
Range testing beacon:
- Maximum power transmission
- Continuous beacon mode
- For testing maximum range

### [LoRa_TX_MultiScenario_Diagnostic](LoRa_TX_MultiScenario_Diagnostic/)
Comprehensive diagnostic tool:
- Multiple test scenarios
- Advanced parameter testing
- Development/debugging use

## Quick Start

1. **Start with [Heltec_Gateway_Test](Heltec_Gateway_Test/)**
   - Follow the README in that folder
   - Get your Heltec license
   - Match gateway configuration
   - Upload and test

2. **Check Gateway Logs**
   - Look for: `✅ LoRa RX: XX bytes, RSSI=-XXdBm, SNR=XXdB`
   - Verify MQTT topics are created

3. **If Issues Arise**
   - See [LoRa_TX_Test_Simple/TROUBLESHOOTING.md](LoRa_TX_Test_Simple/TROUBLESHOOTING.md)
   - Try sync word override: `lora_sync_word_force: "0x3424"`

## Common Issues

### No Packets Received

1. **Check Frequency**: US=915MHz, EU=868MHz - must match exactly
2. **Verify Spreading Factor**: Must be identical on both devices
3. **Check Antenna**: Ensure proper connection and correct frequency rating
4. **Start Close**: Test with devices < 10 meters apart
5. **Sync Word**: Try forcing `0x3424` in gateway config

### Weak Signal

1. **Increase TX Power**: Try 20 dBm (max)
2. **Increase SF**: Higher spreading factor = longer range (SF9, SF10)
3. **Check Antenna**: Proper 915MHz/868MHz antenna connected
4. **Reduce Obstacles**: Line of sight is best

### License Errors (Heltec V3)

- Get license from: https://resource.heltec.cn/search
- Enter chip ID shown in serial monitor
- Use new 4-value format: `{0x12345678, 0x9ABCDEF0, ...}`

## Hardware Tested

- ✅ Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- ✅ Heltec WiFi LoRa 32 V2
- ✅ TTGO LoRa boards
- ✅ Generic ESP32 + SX126x combinations

## Gateway Documentation

See [../sx1262_lora_gateway/README.md](../sx1262_lora_gateway/README.md) for complete gateway documentation.

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

