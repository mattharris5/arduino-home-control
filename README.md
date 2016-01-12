# Arduino Home Control Sketches

This repo holds sketches for Arduino-based projects to control home devices that aren't Internet-enabled. They are designed to integrate with Homebridge using the http plugin. The homebridge daemon can then make these devices Homekit-compatible by communicating with them on your local network.

## firelighter-web
Turns a gas fireplace on and off, and reports state. Could be used for anything with a single on/off switch.

## radio-web
Controls power and volume for a JBL Harmony radio. Power is controlled by hijacking the built-in power button, and volume is controlled by hijacking the volume knob rotary encoder and emulating the signals to the radio's control board.
