# ESP32 DVB Monitor
A simple depatures monitor for the 'Dresdner Verkehrsbetriebe AG (DVB)'.
It uses some APIs listed at [kiliankoe/vvo](https://github.com/kiliankoe/vvo).

![Example Build](https://github.com/andiikaa/dvb-on-esp32/raw/master/doc/example.JPG =300x "Example")

The final project just needs a good looking housing.

## Hardware
- ESP32 Breakout Board (~5,60€ on ebay)
- 1.8" 128X160 SPI TFT LCD Display Modul (~5,30€ on ebay)
- some wires
- breadboard

### Display hints
- 3.3V supply voltage
- ST7735

It seems there are different versions of the display. 
I have v1.1 and have no problems to use it without level shifters.
But I must say, that it has not the best readability, especially if you look from different angles.

## Wiring on ESP32

TFT CS       4
TFT SCK      18 (SCLK)
TFT SDA      19 (MOSI)
TFT RESET    16 
TFT AO       17 (DC)

For VCC i use the 3,3V provided by the ESP32. 
For LED i also use the 3,3V output but with an 150Ω resistor.

## Problems

Slight flickering, when updating the display. I tried to reduce it by drawing a rectangle over existing text in a row before updating it.

## Additional Links
- https://github.com/espressif/arduino-esp32
- http://esp32.net/
