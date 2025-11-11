/**
 * LoRa Multi-Scenario Diagnostic
 *
 * Automatically cycles through combinations of:
 *  - RF switch pin pairs (TXEN,RXEN)
 *  - Spreading Factors (SF7, SF9, SF12)
 *  - Bandwidth (125kHz, 250kHz)
 *  - Sync words (0x34, 0x12, 0x44)
 *  - TX power (14, 20 dBm)
 *
 * Each scenario transmits a numbered packet and waits for TX callback
 * or times out after the configured timeout. Logs detailed status so
 * you can correlate which configuration succeeds (if any).
 */

#ifndef WIFI_LORA_32_V3
#define WIFI_LORA_32_V3
#endif

#include "LoRaWan_APP.h"

// Base frequency (adjust if needed)
#define BASE_FREQ 915000000UL

// LoRa constants
static const uint8_t SYNC_WORDS[] = {0x34, 0x12, 0x44};
static const uint8_t SPREADING_FACTORS[] = {7, 9, 12};
static const uint8_t BANDWIDTHS[] = {0, 1}; // 0=125kHz,1=250kHz
static const int8_t TX_POWERS[] = {14, 20};

// Candidate RF switch pin pairs (-1 means skip pin)
struct RfPair { int8_t txen; int8_t rxen; const char* label; };
static const RfPair RF_PAIRS[] = {
    {-1, -1, "auto"},
    {21, 22, "21/22"},
    {14, 13, "14/13"},
    {7, 6,   "7/6"}
};

// Scenario index trackers
size_t idxRf = 0, idxSf = 0, idxBw = 0, idxSw = 0, idxTxp = 0;

// Runtime state
static RadioEvents_t RadioEvents;
bool txInProgress = false;
bool txCompleted = false;
bool txTimedOut = false;
uint32_t txStart = 0;
uint32_t scenarioNumber = 0;

// Forward declarations
void nextScenario();
void applyScenario();
void startTx();
void rfSwitchToTx();
void rfSwitchToRx();
void rfSwitchInit();

// Airtime estimation (approx.)
float estimateAirtimeMs(uint8_t sf, uint8_t bwIndex, uint8_t cr, uint16_t payloadLen, uint16_t preamble=8) {
    // bwIndex: 0->125k,1->250k
    float bw = (bwIndex==0)?125000.0f:250000.0f;
    float tSym = (float)(1 << sf) / bw * 1000.0f; // ms
    float tPreamble = (preamble + 4.25f) * tSym;
    // payload symbol calculation (LoRa spec approx.)
    int crDenom = cr + 4; // coding rate denominator part
    float payloadSym = (8.0f + ( (float) ( ( (8 * payloadLen) - (4 * sf) + 28 + 16 - 20 ) ) / (4.0f * (sf)) ) ) * crDenom;
    if (payloadSym < 0) payloadSym = 0; // guard
    float tPayload = payloadSym * tSym;
    return tPreamble + tPayload; // ms
}

// === RF SWITCH MANAGEMENT ===
int8_t curTxEn = -1;
int8_t curRxEn = -1;

void rfSwitchInit() {
    if (curTxEn >= 0) pinMode(curTxEn, OUTPUT);
    if (curRxEn >= 0) pinMode(curRxEn, OUTPUT);
}
void rfSwitchToRx() {
    if (curTxEn >= 0) digitalWrite(curTxEn, LOW);
    if (curRxEn >= 0) digitalWrite(curRxEn, HIGH);
}
void rfSwitchToTx() {
    if (curRxEn >= 0) digitalWrite(curRxEn, LOW);
    if (curTxEn >= 0) digitalWrite(curTxEn, HIGH);
}

// === CALLBACKS ===
void OnTxDone(void) {
    txInProgress = false;
    txCompleted = true;
    rfSwitchToRx();
    Serial.println("   -> ✅ OnTxDone() callback fired");
}
void OnTxTimeout(void) {
    txInProgress = false;
    txTimedOut = true;
    rfSwitchToRx();
    Serial.println("   -> ⚠️ OnTxTimeout() callback fired");
}
void OnRxDone(uint8_t*, uint16_t, int16_t, int8_t) {}
void OnRxTimeout(void) {}
void OnRxError(void) {}

// === APPLY SCENARIO ===
void applyScenario() {
    const RfPair &r = RF_PAIRS[idxRf];
    uint8_t sf = SPREADING_FACTORS[idxSf];
    uint8_t bw = BANDWIDTHS[idxBw];
    uint8_t sw = SYNC_WORDS[idxSw];
    int8_t txp = TX_POWERS[idxTxp];

    scenarioNumber++;
    txCompleted = false;
    txTimedOut = false;

    curTxEn = r.txen;
    curRxEn = r.rxen;
    rfSwitchInit();
    rfSwitchToRx();

    Serial.println("\n====================================================");
    Serial.printf("Scenario #%lu RF=%s (TXEN=%d RXEN=%d) SF=%d BW=%s Sync=0x%02X Pwr=%d dBm\n",
                  (unsigned long)scenarioNumber, r.label, curTxEn, curRxEn, sf,
                  bw==0?"125k":"250k", sw, txp);
    Serial.println("====================================================");

    // Re-init radio each scenario
    Radio.Init(&RadioEvents);
    Radio.SetSyncWord(sw);
    Radio.SetChannel(BASE_FREQ);
    Radio.SetTxConfig(MODEM_LORA, txp, 0, bw, sf, 1, 8, false, true, 0, 0, false, 4000);

    float est = estimateAirtimeMs(sf, bw, 1, 40); // estimate for sample payload
    Serial.printf("   -> Estimated airtime (40B payload): %.1f ms\n", est);
    startTx();
}

// === START TX ===
void startTx() {
    char msg[96];
    uint8_t sf = SPREADING_FACTORS[idxSf];
    uint8_t bw = BANDWIDTHS[idxBw];
    uint8_t sw = SYNC_WORDS[idxSw];
    snprintf(msg, sizeof(msg),
             "{\"diag\":1,\"scn\":%lu,\"sf\":%u,\"bw\":%u,\"sw\":%u,\"pwr\":%d}",
             (unsigned long)scenarioNumber, sf, bw, sw, TX_POWERS[idxTxp]);
    uint8_t buf[128];
    size_t len = strlen(msg);
    memcpy(buf, msg, len);

    Serial.printf("   -> TX payload (%u bytes): %s\n", (unsigned)len, msg);
    rfSwitchToTx();
    txInProgress = true;
    txStart = millis();
    Radio.Send(buf, len);
}

// === ADVANCE SCENARIO SPACE ===
bool advanceIndices() {
    if (++idxTxp >= (sizeof(TX_POWERS)/sizeof(TX_POWERS[0]))) { idxTxp = 0; if (++idxSw >= (sizeof(SYNC_WORDS)/sizeof(SYNC_WORDS[0]))) { idxSw = 0; if (++idxBw >= (sizeof(BANDWIDTHS)/sizeof(BANDWIDTHS[0]))) { idxBw = 0; if (++idxSf >= (sizeof(SPREADING_FACTORS)/sizeof(SPREADING_FACTORS[0]))) { idxSf = 0; if (++idxRf >= (sizeof(RF_PAIRS)/sizeof(RF_PAIRS[0]))) { return false; } } } } }
    return true;
}

void nextScenario() {
    if (!advanceIndices()) {
        Serial.println("\nAll scenarios completed.");
        while (true) { delay(1000); }
    }
    applyScenario();
}

// === SETUP ===
void setup() {
    Serial.begin(115200);
    delay(800);
    Serial.println("\nLoRa Multi-Scenario Diagnostic Starting...");
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    applyScenario();
}

// === LOOP ===
void loop() {
    Radio.IrqProcess();
    if (txInProgress) {
        uint32_t elapsed = millis() - txStart;
        if (elapsed > 500 && (elapsed % 1000) < 25) {
            Serial.printf("   -> Waiting... %lu ms\n", (unsigned long)elapsed);
        }
        if (elapsed > 4500) { // manual timeout margin
            Serial.println("   -> ❌ Manual TX timeout (no callback)");
            Radio.Standby();
            txInProgress = false;
        }
    } else {
        // TX finished (either callback or manual timeout)
        static uint32_t doneAt = 0;
        if (doneAt == 0) {
            doneAt = millis();
            Serial.printf("   -> Scenario result: completed=%s timeoutFlag=%s callbackTimeout=%s\n",
                          txCompleted?"YES":"NO",
                          txTimedOut?"YES":"NO",
                          (!txCompleted && !txTimedOut)?"MANUAL":"NO");
            rfSwitchToRx();
        }
        if (millis() - doneAt > 1500) { // wait a bit before next scenario
            doneAt = 0;
            nextScenario();
        }
    }
    delay(10);
}
