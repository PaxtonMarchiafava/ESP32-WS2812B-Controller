# ESP32-WS2812B-Controller
A control system for WS2812B led strips.
---
Commands received through Bluetooth serial. Math based on [this function](https://www.desmos.com/calculator/zzhjku5rpy).
# Commands:
| Command | function | example |
| :----: | :----: | :----: |
| step=value | Changes the speed at which the sin waves move.<br /> Higher values make the wave move faster. | step=-0.5 |
| rgb(red value, green value,<br />blue value) | Changes the color of the Leds.<br /> Color values are typically from 0-1 but this is not required. | rgb(0.5,0,1) |
| rgb(red value, green value,<br />blue value) | Changes the color of the Leds.<br /> Values from 1-255 for typical rgb usage. | rgb(16,52,166) |
| a=value | Changes the horizontal shift of the sin wave.<br /> This command starts to matter more as your b value approaches 0. | a=2.2 |
| b=value | Changes the wavelength. Gets wider as the b value approaches 0. | b=0.15 |
| c=value | Changes the amplitude of the sin wave.<br /> The higher this number, The brighter the LED. | c=100 |
| d=value | Sets an offset for the brightness | d=-100 |
| read | Sends the current values to the user over Bluetooth Serial. | read |
| enable(integer)<br /> enable(integer-integer) | Turns on/off the LED/range of LEDs. | enable(10)<br /> enable(0-100) |
| on | Enables all lights | on |
| off | Disables all lights | off |
| period=value | Changes b value to set the period to a specified size | period=50 |
