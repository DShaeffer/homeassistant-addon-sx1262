/**
 * ESP32 Serial Relay - ME201W Sensor Debug Tool
 * 
 * Purpose: Forward all serial data from ME201W sensor (UART1) to USB Serial (UART0)
 *          so you can see the raw sensor output in Arduino IDE Serial Monitor
 * 
 * Hardware: Any ESP32 board
 * 
 * Connections:
 *   - ME201W GND   → ESP32 GND  (MUST connect!)
 *   - ME201W TX    → ESP32 GPIO 5 (RX1)
 *   - ME201W 5V    → DISCONNECTED when USB is connected!
 *   - ESP32 USB    → Computer (powers ESP32 and provides Serial Monitor)
 * 
 * Usage:
 *   1. Upload this sketch to ESP32
 *   2. Open Arduino IDE Serial Monitor at 115200 baud
 *   3. You'll see raw ME201W data exactly as it's transmitted
 * 
 * Author: Hillsville Cabin Project
 * Date: November 2025
 */

// ME201W Serial Configuration
#define SENSOR_SERIAL_RX 5        // GPIO5 for sensor RX
#define SENSOR_SERIAL_TX -1       // Not needed (read-only)
#define SENSOR_BAUD_RATE 115200   // ME201W baud rate

// Create hardware serial instance for sensor
HardwareSerial sensorSerial(1);

// Statistics
unsigned long bytesReceived = 0;
unsigned long linesReceived = 0;
unsigned long lastStatTime = 0;

void setup() {
  // USB Serial (UART0) for Serial Monitor at 115200
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n\n========================================");
  Serial.println("  ME201W Serial Relay - Debug Tool");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Waiting for ME201W sensor data...");
  Serial.printf("Sensor RX: GPIO %d @ %d baud\n", SENSOR_SERIAL_RX, SENSOR_BAUD_RATE);
  Serial.println("All sensor data will be displayed below:");
  Serial.println("----------------------------------------\n");
  
  // Initialize sensor serial (UART1)
  sensorSerial.begin(SENSOR_BAUD_RATE, SERIAL_8N1, SENSOR_SERIAL_RX, SENSOR_SERIAL_TX);
  
  lastStatTime = millis();
}

void loop() {
  // Forward all data from sensor to USB Serial Monitor
  while (sensorSerial.available()) {
    char c = sensorSerial.read();
    Serial.print(c);  // Print exactly as received
    bytesReceived++;
    
    // Count lines
    if (c == '\n') {
      linesReceived++;
    }
  }
  
  // Print statistics every 10 seconds
  if (millis() - lastStatTime > 10000) {
    lastStatTime = millis();
    
    Serial.println("\n========================================");
    Serial.printf("Statistics: %lu bytes, %lu lines received\n", bytesReceived, linesReceived);
    
    if (bytesReceived == 0) {
      Serial.println("⚠️  WARNING: No data received!");
      Serial.println("   Check connections:");
      Serial.println("   - ME201W TX → ESP32 GPIO 5");
      Serial.println("   - ME201W GND → ESP32 GND");
      Serial.println("   - ME201W should transmit every ~45 seconds");
    }
    
    Serial.println("========================================\n");
    
    // Reset counters
    bytesReceived = 0;
    linesReceived = 0;
  }
  
  delay(1);  // Small delay to prevent watchdog issues
}
