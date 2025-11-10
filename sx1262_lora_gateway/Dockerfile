ARG BUILD_FROM
FROM $BUILD_FROM

# Install requirements
RUN apk add --no-cache \
    python3 \
    py3-pip \
    py3-spidev \
    py3-rpi-gpio \
    py3-paho-mqtt

# Install Python packages
RUN pip3 install --no-cache-dir \
    pyLoRa \
    RPi.GPIO

# Copy data
COPY run.sh /
COPY lora_gateway.py /

RUN chmod a+x /run.sh

CMD [ "/run.sh" ]
