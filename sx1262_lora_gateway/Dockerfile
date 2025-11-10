ARG BUILD_FROM
FROM $BUILD_FROM

# Install system dependencies
RUN apk add --no-cache \
    python3 \
    py3-pip \
    gcc \
    python3-dev \
    musl-dev \
    linux-headers

# Install Python packages
RUN pip3 install --no-cache-dir \
    RPi.GPIO \
    spidev \
    paho-mqtt \
    dragino

# Copy data
COPY run.sh /
COPY lora_gateway.py /

RUN chmod a+x /run.sh

CMD [ "/run.sh" ]
