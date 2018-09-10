# bacon_cape_lib
Repository of bacon_cape_lib, a C library for the Beaglebone Black's cape [Bacon Cape](https://elinux.org/Bacon_Cape).

This library is under development. At this moment it covers the following Bacon Cape features:

- LEDs turn-on/turn-off (no LEDs PWM suported at this moment)
- Slider read
- EEPROM write and read (to write EEPROM, a little hardware hack is needed: short-circuit C2 capacitor)
- 7-segment display (for alfanum chars from 0 to F)

Also, this repository contains two examples:
- lib_validator: a simple program to validate all features covered by the bacon_cape_lib
- mqtt_example: a simple program to interact with LEDs, slider and button over Internet using MQTT protocol. For this example, [Paho MQTT C library](https://github.com/eclipse/paho.mqtt.c) is required.
By doing this, you can get your Bacon Cape's information in a dashboard using [MQTT Dash App](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=pt_BR), for example:

![MQTT Dash App](mqtt_example/mqtt_dash_app_example.jpg?raw=true "MQTT Dash App interaction")