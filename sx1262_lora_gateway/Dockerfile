ARG BUILD_FROM
FROM $BUILD_FROM

# Install system dependencies
RUN apk add --no-cache \
    python3 \
    py3-pip \
    py3-paho-mqtt \
    gcc \
    python3-dev \
    musl-dev \
    linux-headers

# Install Python packages
RUN pip3 install --no-cache-dir \
    pyLoRa \
    RPi.GPIO \
    spidev

# Copy data
COPY run.sh /
COPY lora_gateway.py /

RUN chmod a+x /run.sh

CMD [ "/run.sh" ]
