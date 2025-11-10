#!/usr/bin/env python3
"""
SX1262 LoRa Gateway for Home Assistant
Receives data from ESP32 water sensor bridge and publishes to MQTT
"""

import os
import sys
import json
import time
import logging
from datetime import datetime
import paho.mqtt.client as mqtt

# Add LoRaRF to path
sys.path.append('/LoRaRF')
# Some GPIO libraries check for Raspberry Pi revision via /proc/device-tree.
# When running inside Home Assistant add-on containers the device tree may not
# be visible at import time. Set a reasonable default revision so the lgpio
# / RPi.GPIO compatibility layer doesn't raise on import.
os.environ.setdefault('RPI_LGPIO_REVISION', 'a020d3')

# Import LoRaRF SX126x driver
from LoRaRF import SX126x

# Configuration from environment variables
LORA_FREQ = float(os.getenv('LORA_FREQ', '915.0'))
LORA_SF = int(os.getenv('LORA_SF', '7'))
LORA_BW = int(os.getenv('LORA_BW', '125000'))
LORA_CR = int(os.getenv('LORA_CR', '5'))
# Sync word: default 52 (decimal) = 0x34 (hex) = standard LoRa private network
LORA_SW = int(os.getenv('LORA_SW', '52'))
LORA_POWER = int(os.getenv('LORA_POWER', '20'))

MQTT_HOST = os.getenv('MQTT_HOST', 'core-mosquitto')
MQTT_PORT = int(os.getenv('MQTT_PORT', '1883'))
MQTT_USER = os.getenv('MQTT_USER', '')
MQTT_PASS = os.getenv('MQTT_PASS', '')
MQTT_PREFIX = os.getenv('MQTT_PREFIX', 'lora/water_sensor')

LOG_LEVEL = os.getenv('LOG_LEVEL', 'info').upper()

# Setup logging
logging.basicConfig(
    level=getattr(logging, LOG_LEVEL),
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# MQTT client
mqtt_client = None
mqtt_connected = False

# Statistics
stats = {
    'messages_received': 0,
    'messages_parsed': 0,
    'mqtt_published': 0,
    'errors': 0,
    'start_time': datetime.now().isoformat()
}

def on_mqtt_connect(client, userdata, flags, rc):
    """MQTT connection callback"""
    global mqtt_connected
    if rc == 0:
        logger.info(f"Connected to MQTT broker at {MQTT_HOST}:{MQTT_PORT}")
        mqtt_connected = True
        # Publish online status
        client.publish(f"{MQTT_PREFIX}/status", "online", retain=True)
    else:
        logger.error(f"MQTT connection failed with code {rc}")
        mqtt_connected = False

def on_mqtt_disconnect(client, userdata, rc):
    """MQTT disconnection callback"""
    global mqtt_connected
    mqtt_connected = False
    if rc != 0:
        logger.warning(f"Unexpected MQTT disconnection. Will auto-reconnect.")

def publish_to_mqtt(topic, payload, retain=False):
    """Publish message to MQTT broker"""
    global mqtt_connected, stats
    if not mqtt_connected:
        logger.warning("MQTT not connected, skipping publish")
        return False
    
    try:
        result = mqtt_client.publish(topic, payload, retain=retain)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            stats['mqtt_published'] += 1
            return True
        else:
            logger.error(f"MQTT publish failed with code {result.rc}")
            return False
    except Exception as e:
        logger.error(f"Error publishing to MQTT: {e}")
        stats['errors'] += 1
        return False

def parse_and_publish_sensor_data(payload):
    """Parse JSON sensor data and publish to individual MQTT topics"""
    try:
        data = json.loads(payload)
        stats['messages_parsed'] += 1
        
        logger.debug(f"Parsed data: {json.dumps(data, indent=2)}")
        
        # Publish complete JSON
        publish_to_mqtt(f"{MQTT_PREFIX}/data", payload)
        
        # Publish individual sensor values for easy Home Assistant integration
        if 'water' in data:
            water = data['water']
            publish_to_mqtt(f"{MQTT_PREFIX}/water/level", str(water.get('level_cm', 0)))
            publish_to_mqtt(f"{MQTT_PREFIX}/water/percent", str(water.get('percent', 0)))
            publish_to_mqtt(f"{MQTT_PREFIX}/water/raw_distance", str(water.get('raw_distance', 0)))
            publish_to_mqtt(f"{MQTT_PREFIX}/water/state", str(water.get('state', 0)))
        
        if 'battery' in data:
            battery = data['battery']
            publish_to_mqtt(f"{MQTT_PREFIX}/battery/voltage", str(battery.get('voltage', 0)))
            publish_to_mqtt(f"{MQTT_PREFIX}/battery/unit", str(battery.get('unit', 0)))
        
        if 'status' in data:
            status = data['status']
            publish_to_mqtt(f"{MQTT_PREFIX}/status/wifi", str(status.get('wifi', 0)))
            publish_to_mqtt(f"{MQTT_PREFIX}/status/updates", str(status.get('updates', 0)))
        
        # Publish timestamp
        publish_to_mqtt(f"{MQTT_PREFIX}/last_seen", datetime.now().isoformat())
        
        logger.info(f"Published sensor data: Level={data.get('water', {}).get('level_cm', 'N/A')}cm, "
                   f"Battery={data.get('battery', {}).get('voltage', 'N/A')}V")
        
        return True
        
    except json.JSONDecodeError as e:
        logger.error(f"JSON decode error: {e}")
        logger.error(f"Raw payload: {payload}")
        stats['errors'] += 1
        return False
    except Exception as e:
        logger.error(f"Error parsing sensor data: {e}")
        stats['errors'] += 1
        return False

def on_lora_receive(lora):
    """Check for received LoRa messages"""
    global stats
    
    try:
        # Check IRQ status for any activity
        try:
            irq_status = lora.getIrqStatus()
            
            # Log any IRQ activity (for debugging)
            if irq_status != 0:
                logger.debug(f"IRQ Status: 0x{irq_status:04X}")
                
                # Check for specific IRQs
                if irq_status & lora.IRQ_PREAMBLE_DETECTED:
                    logger.info("📡 Preamble detected!")
                if irq_status & lora.IRQ_HEADER_VALID:
                    logger.info("📡 Valid header detected!")
                if irq_status & lora.IRQ_HEADER_ERR:
                    logger.warning("⚠️  Header error!")
                if irq_status & lora.IRQ_CRC_ERR:
                    logger.warning("⚠️  CRC error!")
                if irq_status & lora.IRQ_TIMEOUT:
                    logger.debug("RX timeout (normal, waiting for packet)")
        except Exception as e:
            logger.error(f"Error reading IRQ status: {e}")
            return
        
        # Check status (non-blocking for continuous RX)
        try:
            status = lora.status()
            
            if status == lora.STATUS_RX_DONE:
                stats['messages_received'] += 1
                
                # Read payload
                message = []
                while lora.available() > 0:
                    message.append(lora.read())
                
                # Convert to string
                payload = bytes(message).decode('utf-8', errors='ignore')
                
                # Get RSSI and SNR
                rssi = lora.packetRssi()
                snr = lora.packetSnr()
                
                logger.info(f"✅ LoRa RX: {len(payload)} bytes, RSSI={rssi}dBm, SNR={snr}dB")
                logger.debug(f"Raw payload: {payload}")
                
                # Publish signal quality
                publish_to_mqtt(f"{MQTT_PREFIX}/rssi", str(rssi))
                publish_to_mqtt(f"{MQTT_PREFIX}/snr", str(snr))
                
                # Parse and publish sensor data
                if payload.strip():
                    parse_and_publish_sensor_data(payload.strip())
        except Exception as e:
            logger.error(f"Error checking status: {e}")
                
    except Exception as e:
        logger.error(f"Error in LoRa receive handler: {e}")
        import traceback
        traceback.print_exc()
        stats['errors'] += 1

def setup_mqtt():
    """Initialize MQTT connection"""
    global mqtt_client
    
    logger.info("Setting up MQTT connection...")
    mqtt_client = mqtt.Client(
        client_id="sx1262_lora_gateway",
        callback_api_version=mqtt.CallbackAPIVersion.VERSION1
    )
    
    # Set last will (offline status)
    mqtt_client.will_set(f"{MQTT_PREFIX}/status", "offline", retain=True)
    
    # Set callbacks
    mqtt_client.on_connect = on_mqtt_connect
    mqtt_client.on_disconnect = on_mqtt_disconnect
    
    # Set username/password if provided
    if MQTT_USER and MQTT_PASS:
        mqtt_client.username_pw_set(MQTT_USER, MQTT_PASS)
    
    try:
        mqtt_client.connect(MQTT_HOST, MQTT_PORT, 60)
        mqtt_client.loop_start()
        logger.info("MQTT loop started")
    except Exception as e:
        logger.error(f"Failed to connect to MQTT broker: {e}")
        sys.exit(1)

def setup_lora():
    """Initialize LoRa radio"""
    logger.info("Setting up SX1262 LoRa radio...")
    
    try:
        # Check if SPI device exists
        import glob
        spi_devices = glob.glob('/dev/spi*')
        logger.info(f"Available SPI devices: {spi_devices}")
        
        # Check /dev contents
        import subprocess
        try:
            dev_list = subprocess.check_output(['ls', '-la', '/dev/'], text=True)
            logger.debug(f"Contents of /dev/:\n{dev_list}")
        except:
            pass
        
        # Waveshare SX1262 HAT pinout for Raspberry Pi (based on user's configuration)
        # CS: GPIO 8 (Pin 24) - SPI0 CE0
        # BUSY: GPIO 20 (Pin 38)
        # DIO1: GPIO 16 (Pin 36)
        # RST: GPIO 18 (Pin 12)
        # TXEN: GPIO 6 (Pin 31, DIO4)
        busId = 0           # SPI bus 0
        csId = 0            # SPI CS 0 (/dev/spidev0.0) - GPIO 8
        resetPin = 18       # GPIO 18 (Pin 12) - RST
        busyPin = 20        # GPIO 20 (Pin 38) - BUSY
        irqPin = 16         # GPIO 16 (Pin 36) - DIO1
        txenPin = 6         # GPIO 6 (Pin 31) - TXEN/DIO4
        rxenPin = -1        # Not used
        
        logger.info(f"Pin configuration: RESET={resetPin}, BUSY={busyPin}, IRQ={irqPin}, TXEN={txenPin}")
        
        logger.info("Creating SX126x object...")
        lora = SX126x()
        logger.info("SX126x object created")
        
        # Initialize the radio
        logger.info("Calling lora.begin()...")
        begin_result = lora.begin(busId, csId, resetPin, busyPin, irqPin, txenPin, rxenPin)
        logger.info(f"lora.begin() returned: {begin_result}")
        
        if not begin_result:
            raise Exception("Failed to initialize SX1262 radio")
        
        logger.info("Radio initialized successfully")
        
        # Configure for Raspberry Pi RF switch
        logger.info("Configuring DIO2 RF switch...")
        lora.setDio2RfSwitch()
        logger.info("DIO2 RF switch configured")
        
        # Set frequency (convert MHz to Hz)
        logger.info(f"Setting frequency to {LORA_FREQ} MHz...")
        lora.setFrequency(int(LORA_FREQ * 1000000))
        
        # Configure modulation parameters
        logger.info(f"Setting spreading factor to {LORA_SF}...")
        lora.setSpreadingFactor(LORA_SF)
        
        logger.info(f"Setting bandwidth to {LORA_BW} Hz...")
        lora.setBandwidth(LORA_BW)
        
        logger.info(f"Setting code rate to 4/{LORA_CR}...")
        lora.setCodeRate(LORA_CR)
        
        # Set sync word - CRITICAL FIX
        # The LoRaRF library expects 16-bit sync word (MSB+LSB)
        # ESP32 Heltec Radio.SetSyncWord(0x34) sets BOTH bytes to 0x34 → 0x3434
        # If we pass 0x34 to LoRaRF, it converts to 0x3444 (different!)
        # So we must explicitly set 0x3434 to match ESP32
        logger.info(f"Setting sync word to 0x3434 (ESP32 format: 0x34 in both bytes)...")
        lora.setSyncWord(0x3434)  # Match ESP32: both MSB and LSB = 0x34
        
        # Set packet parameters to match ESP32 Heltec configuration
        logger.info("Setting packet parameters...")
        # ESP32 uses: preamble=8, variable length header, CRC on, IQ normal
        lora.setHeaderType(lora.HEADER_EXPLICIT)  # Variable length (not fixed)
        lora.setPreambleLength(8)  # Match ESP32 LORA_PREAMBLE_LENGTH
        lora.setCrcEnable(True)    # Match ESP32 CRC enabled
        lora.setInvertIq(False)    # Match ESP32 LORA_IQ_INVERSION_ON false
        logger.info("  Preamble: 8, Header: Explicit, CRC: On, IQ: Normal")
        
        # Set TX power (SX1262 supports up to +22dBm)
        logger.info(f"Setting TX power to {LORA_POWER} dBm...")
        lora.setTxPower(LORA_POWER, lora.TX_POWER_SX1262)
        
        # Set to continuous receive mode
        logger.info("Setting to continuous receive mode...")
        lora.request(lora.RX_CONTINUOUS)
        logger.info("Receive mode active (continuous)")
        
        logger.info(f"LoRa configured:")
        logger.info(f"  Frequency: {LORA_FREQ} MHz")
        logger.info(f"  Spreading Factor: {LORA_SF}")
        logger.info(f"  Bandwidth: {LORA_BW} Hz")
        logger.info(f"  Coding Rate: 4/{LORA_CR}")
        logger.info(f"  Sync Word: 0x3434 (ESP32 format)")
        logger.info(f"  Preamble Length: 8")
        logger.info(f"  Header Type: Explicit (variable length)")
        logger.info(f"  CRC: Enabled")
        logger.info(f"  IQ Inversion: Normal")
        logger.info(f"  TX Power: {LORA_POWER} dBm")
        
        return lora
        
    except Exception as e:
        logger.error(f"Failed to initialize LoRa radio: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

def publish_statistics():
    """Publish gateway statistics to MQTT"""
    stats_payload = {
        'messages_received': stats['messages_received'],
        'messages_parsed': stats['messages_parsed'],
        'mqtt_published': stats['mqtt_published'],
        'errors': stats['errors'],
        'uptime_seconds': int((datetime.now() - datetime.fromisoformat(stats['start_time'])).total_seconds()),
        'start_time': stats['start_time']
    }
    publish_to_mqtt(f"{MQTT_PREFIX}/gateway/stats", json.dumps(stats_payload))

def main():
    """Main gateway loop"""
    logger.info("========================================")
    logger.info("SX1262 LoRa Gateway for Home Assistant")
    logger.info("========================================")
    
    # Setup MQTT
    setup_mqtt()
    
    # Wait for MQTT connection
    retry_count = 0
    while not mqtt_connected and retry_count < 10:
        logger.info("Waiting for MQTT connection...")
        time.sleep(1)
        retry_count += 1
    
    if not mqtt_connected:
        logger.error("Failed to connect to MQTT broker after 10 attempts")
        sys.exit(1)
    
    # Setup LoRa
    lora = setup_lora()
    
    logger.info("Gateway ready! Listening for LoRa messages...")
    
    # Publish initial stats
    publish_statistics()
    last_stats_time = time.time()
    last_heartbeat = time.time()
    
    try:
        while True:
            # Check for received packets
            on_lora_receive(lora)
            
            # Heartbeat every 10 seconds
            if time.time() - last_heartbeat > 10:
                logger.info(f"💓 Heartbeat - Listening... (checked {stats['messages_received']} packets so far)")
                last_heartbeat = time.time()
            
            # Publish statistics every 60 seconds
            if time.time() - last_stats_time > 60:
                publish_statistics()
                last_stats_time = time.time()
            
            # Small delay to prevent CPU hogging
            time.sleep(0.1)
            
    except KeyboardInterrupt:
        logger.info("Shutting down gracefully...")
    except Exception as e:
        logger.error(f"Fatal error in main loop: {e}")
        stats['errors'] += 1
    finally:
        # Publish offline status
        if mqtt_client and mqtt_connected:
            mqtt_client.publish(f"{MQTT_PREFIX}/status", "offline", retain=True)
            publish_statistics()
            mqtt_client.loop_stop()
            mqtt_client.disconnect()
        
        logger.info("Gateway stopped")
        logger.info(f"Final stats: {stats}")

if __name__ == "__main__":
    main()
