# ESP32-WS2812B-Controller
A control system for WS2812B led strips.
---
Commands received through Bluetooth serial. Math based on [this function](https://www.desmos.com/calculator/zzhjku5rpy).
# Commands:
| Command | function | example |
| :----: | :----: | :----: |
| step=value | Changes the speed at which the sin waves move. Higher values make the wave move faster. | step=-0.5 |
| rgb(red value, green value, blue value) | Changes the color of the Leds. Color values are typically from 0-1 but this is not required. | rgb(0.5,0,1) |
| a=value | Changes the horizontal shift of the sin wave. This command starts to matter more as your b value approaches 0. | a=2.2 |
| b=value | Changes the wavelength. Gets wider as the b value approaches 0. | b=0.15 |
| c=value | Changes the amplitude of the sin wave. The higher this number, The brighter the LED. | c=100 |
| read | Sends the current values to the user over Bluetooth Serial. | read |
