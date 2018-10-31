This project is meant for an ESP8266.

# sensor-hub
## Hardware
This first version uses only two sensors : a current probe, and a temperature/humidity sensor :
1. SCT-013 : a current transformer. Gives 50mA out for a 100A current.
2. Bosch BME280 : measures temperature and humidity.

### Current probe
Since we don't measure the mains' voltage, we cannot measure the power factor (phase between the current and voltage). So we use a hardware peak detector to ease the measuring work of the ESP32 chip.
The peak detector is done with an op-amp and a diode, the simulations and schematics can be found in the `hardware` folder.

### Temperature/Humidity
The BME280 can talk with the ESP8266 in I2C. That's what we do. I2C will work over tens of meters, with cheap ethernet cable.

## Ressources
The following links were useful in this project :
- https://poweruc.com/blogs/news/non-invasive-sensor-yhdc-sct013-000-ct-used-with-arduino-sct-013
- https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino
