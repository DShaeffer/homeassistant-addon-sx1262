#!/usr/bin/with-contenv bashio

bashio::log.info "Starting SX1262 LoRa Gateway..."

# Get configuration
LORA_FREQ=$(bashio::config 'lora_frequency')
LORA_SF=$(bashio::config 'lora_spreading_factor')
LORA_BW=$(bashio::config 'lora_bandwidth')
LORA_CR=$(bashio::config 'lora_coding_rate')
LORA_SW=$(bashio::config 'lora_sync_word')
LORA_SW_FORCE=$(bashio::config 'lora_sync_word_force')
LORA_SW_MSB=$(bashio::config 'lora_sync_word_msb')
LORA_SW_LSB=$(bashio::config 'lora_sync_word_lsb')
LORA_POWER=$(bashio::config 'lora_tx_power')
MQTT_HOST=$(bashio::config 'mqtt_host')
MQTT_PORT=$(bashio::config 'mqtt_port')
MQTT_USER=$(bashio::config 'mqtt_username')
MQTT_PASS=$(bashio::config 'mqtt_password')
MQTT_PREFIX=$(bashio::config 'mqtt_topic_prefix')
LOG_LEVEL=$(bashio::config 'log_level')

bashio::log.info "LoRa Frequency: ${LORA_FREQ} MHz"
bashio::log.info "Spreading Factor: ${LORA_SF}"
bashio::log.info "Bandwidth: ${LORA_BW} Hz"
bashio::log.info "MQTT Broker: ${MQTT_HOST}:${MQTT_PORT}"

# Export config as environment variables
export LORA_FREQ LORA_SF LORA_BW LORA_CR LORA_SW LORA_SW_FORCE LORA_SW_MSB LORA_SW_LSB LORA_POWER
export MQTT_HOST MQTT_PORT MQTT_USER MQTT_PASS MQTT_PREFIX
export LOG_LEVEL

# Run the Python gateway
python3 /lora_gateway.py
