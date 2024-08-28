DSB and TDS ANTZ Server Project

 Schematic Overview

 1. DSB1820 Temperature Sensor

- VCC: Connect to 3.3V
- Signal: Connect to GPIO 33 (or any available ADC1 pin on the ESP32)
- GND: Connect to GND**

Note: Add a resistor between the VCC and Signal pins (use a 4.4kΩ or 10kΩ resistor).

 2. TDS Sensor

- VCC: Connect to 3.3V
- Signal: Connect to GPIO 32 (or any available ADC1 pin on the ESP32)
- GND: Connect to **GND**

Additional Notes

- Ensure all connections are secure and double-check the pin assignments.
- The resistor between the VCC and Signal pin for the DSB1820 sensor helps stabilize the signal.
