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
    swig \
    make

# Build and install lgpio C library (needed by rpi-lgpio)
RUN git clone https://github.com/joan2937/lg.git /tmp/lg \
    && make -C /tmp/lg \
    && make -C /tmp/lg install \
    && cp -a /usr/local/lib/liblgpio.so* /usr/lib/ || true \
    && rm -rf /tmp/lg

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
