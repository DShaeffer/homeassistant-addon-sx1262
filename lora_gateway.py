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
from LoRa import *

# Configuration from environment variables
LORA_FREQ = float(os.getenv('LORA_FREQ', '915.0'))
LORA_SF = int(os.getenv('LORA_SF', '7'))
LORA_BW = int(os.getenv('LORA_BW', '125000'))
LORA_CR = int(os.getenv('LORA_CR', '5'))
LORA_SW = int(os.getenv('LORA_SW', '0x12'))
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
    """Callback when LoRa message is received"""
    global stats
    
    try:
        if lora.receivedPacket():
            stats['messages_received'] += 1
            
            # Read payload
            payload = ""
            while lora.available():
                payload += chr(lora.read())
            
            # Get RSSI and SNR
            rssi = lora.packetRssi()
            snr = lora.packetSnr()
            
            logger.info(f"LoRa RX: {len(payload)} bytes, RSSI={rssi}dBm, SNR={snr}dB")
            logger.debug(f"Raw payload: {payload}")
            
            # Publish signal quality
            publish_to_mqtt(f"{MQTT_PREFIX}/rssi", str(rssi))
            publish_to_mqtt(f"{MQTT_PREFIX}/snr", str(snr))
            
            # Parse and publish sensor data
            if payload.strip():
                parse_and_publish_sensor_data(payload.strip())
                
    except Exception as e:
        logger.error(f"Error in LoRa receive handler: {e}")
        stats['errors'] += 1

def setup_mqtt():
    """Initialize MQTT connection"""
    global mqtt_client
    
    logger.info("Setting up MQTT connection...")
    mqtt_client = mqtt.Client("sx1262_lora_gateway")
    
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
        # Waveshare SX1262 HAT pinout
        # RESET: GPIO 18 (Pin 12)
        # BUSY: GPIO 23 (Pin 16)
        # DIO1: GPIO 24 (Pin 18)
        # NSS/CS: CE0 (GPIO 8, Pin 24)
        # SPI: /dev/spidev0.0
        
        lora = LoRa()
        
        # Basic configuration
        lora.setSyncWord(LORA_SW)
        lora.setFrequency(int(LORA_FREQ * 1000000))  # Convert MHz to Hz
        lora.setSpreadingFactor(LORA_SF)
        lora.setSignalBandwidth(LORA_BW)
        lora.setCodingRate4(LORA_CR)
        lora.setTxPower(LORA_POWER, PA_OUTPUT_PA_BOOST_PIN)
        
        logger.info(f"LoRa configured:")
        logger.info(f"  Frequency: {LORA_FREQ} MHz")
        logger.info(f"  Spreading Factor: {LORA_SF}")
        logger.info(f"  Bandwidth: {LORA_BW} Hz")
        logger.info(f"  Coding Rate: 4/{LORA_CR}")
        logger.info(f"  Sync Word: 0x{LORA_SW:02X}")
        logger.info(f"  TX Power: {LORA_POWER} dBm")
        
        return lora
        
    except Exception as e:
        logger.error(f"Failed to initialize LoRa radio: {e}")
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
    
    try:
        while True:
            # Check for received packets
            on_lora_receive(lora)
            
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
