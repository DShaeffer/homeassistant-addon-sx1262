ARG BUILD_FROM
FROM $BUILD_FROM

# Install system dependencies
RUN apk add --no-cache \
    python3 \
    py3-pip \
    gcc \
    python3-dev \
    musl-dev \
    linux-headers \
    git \
    swig

# Install Python packages
RUN pip3 install --no-cache-dir \
    rpi-lgpio \
    spidev \
    paho-mqtt

# Copy LoRaRF library
COPY LoRaRF /LoRaRF

# Copy data
COPY run.sh /
COPY lora_gateway.py /

RUN chmod a+x /run.sh

CMD [ "/run.sh" ]
