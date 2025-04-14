/*  ------------ V2 -----------
 - Has both modes
 - got rid of rgb values by % with numbers less than 1. It was stupid IMO

------------- TODO -----------
 - Improve code readability
 - multiple commands in one message by parsing with a semicolon
 - new function to just send a number of loops you want across the entire strip
 - allow for motion sensor to turn then off automatically
 - keep data through power cycles usng EEPROM
}
*/

#include <BluetoothSerial.h>
#include <Adafruit_NeoPixel.h>

#define PIN 19
#define NUM_PIXELS 370
#define MAX_BRIGHTNESS 200
#define delayValue 10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

double stepSize = 0.07;
String command = "";

boolean enable[NUM_PIXELS];
double colorValue[] = {0, 0, 0};
double rgb[] = {100, 0, 200}; // 0 - 255

int mode = 1; // mode 1 is tricolor, mode 2 has rgb controls

double a = 0;
double b = M_PI * 0.01;
double c = 75;
double d = c;

BluetoothSerial BluetoothDevice;

void Send(String data) {

  BluetoothDevice.print("\n");
  for (int i = 0; i < data.length(); i++) {
    BluetoothDevice.print(data.charAt(i));
  }
}

boolean equals(String one, String two) {

  if (one.length() == two.length()) {
    for (int i = 0; i < one.length(); i++) {
      if (one.charAt(i) != two.charAt(i)) {
        return false;
      }
    }
  }
  else {
    return false;
  }

  return true;
}

void process(String info) { // readability is a 0/10 in this bad boy

  if (equals(info.substring(0, 5), "step=")) { // command to change speed that sin wave moves
    stepSize = info.substring(5).toDouble();

  } else if (info.charAt(0) == 'a') { // sets horizontal position. little use unless step=0
    a = info.substring(2).toDouble();

  } else if (info.charAt(0) == 'b') { // horizontal stretch/compression
    b = M_PI * info.substring(2).toDouble();

  } else if (equals(info.substring(0, 7), "period=") == true) { // set period
    b = (2 * M_PI) / info.substring(7).toDouble();

  } else if (info.charAt(0) == 'c') { // vertical streth/compression
    c = info.substring(2).toDouble();
    d = c; // optional. I use this cause its easy

  } else if (info.charAt(0) == 'd') { // displaces brightness up/down
    d = c + info.substring(2).toDouble();

  } else if (equals(info.substring(0, 7), "enable(") == true) { // enable/disable leds
    if (info.indexOf("-") == -1) { // if only working with one light
      enable[info.substring(info.indexOf('(') + 1, info.indexOf(')')).toInt()] = !enable[info.substring(info.indexOf('(') + 1, info.indexOf(')')).toInt()];
    
    } else { // change line of pixels
      for (int i = info.substring(info.indexOf('(') + 1, info.indexOf('-')).toInt(); i < info.substring(info.indexOf('-') + 1, info.indexOf(')')).toInt(); i++) {
        enable[i] = !enable[i];
      }
    }

  } else if (equals("read", info)) { // read current values from the board
    Send("Mode = " + String(mode));
    Send("Step = " + String(stepSize));
    Send("a = " + String(a));
    Send("b = " + String(b / M_PI));
    Send("c = " + String(c));
    Send("d = " + String(d));

    if (mode == 2) {
      Send("RGB = (" + String(rgb[0]) + ", " + String(rgb[1]) + ", " + String(rgb[2]) + ")");
    }

  } else if (equals("on", info)) { // enable all lights
    for (int i = 0; i < NUM_PIXELS; i++) {
      enable[i] = true;
    }

  } else if (equals("off", info)) { // disable all lights
    for (int i = 0; i < NUM_PIXELS; i++) {
      enable[i] = false;
    }
  
  } else if (equals(info.substring(0, 4), "mode")) { // change control mode
    mode = info.substring(5).toInt();
  
  } else if (equals(info.substring(0, 3), "rgb")) {

    rgb[0] = (command.substring(command.indexOf('(') + 1, command.indexOf(','))).toDouble();
    rgb[1] = (command.substring(command.indexOf(',') + 1, command.lastIndexOf(','))).toDouble();
    rgb[2] = (command.substring(command.lastIndexOf(',') + 1, command.indexOf(')'))).toDouble();
  }
}

void setup() {

  pixels.begin();
  pixels.setPixelColor(3, 10, 10, 10); // see how long the loop after this takes without having pixels.show in it
  pixels.show();

  for (int i = 0; i < NUM_PIXELS; i++) {
    enable[i] = true;
    pixels.setPixelColor(i, 2, 2, 2);
  }
  pixels.show(); // this has been moved outside ^this^ loop

  Serial.begin(115200);                  // idk if this is needed. Test
  BluetoothDevice.begin("🍆FAT NUTS🍆"); // bluetooth device name
}

void loop() {

  while (BluetoothDevice.available()) { // get messages from bluetooth
    command += (char)BluetoothDevice.read();
  }
  command.toLowerCase(); // set command to lower case
  for (int i = 0; i < command.length(); i++) { // take out spacebar
    if (command.charAt(i) == ' ') {
      command.remove(i, 1);
      i--;
    }
  }

  if (!command.equals("")) { // if command has contents
    process(command); // use command
    command = ""; // clear so we can use it on next read
  }

  for (int i = 0; i < NUM_PIXELS; i++) { // for all pixels

    if (enable[i] == true) { // if pixel enabled

      switch (mode) {
      case 1: // TriColor Calculations

        for (int j = 0; j < 3; j++) { // for all 3 colors

          switch (j) { // huge calculation
          case 0:
            colorValue[j] = c * (cos(b * (i + a))) + d;
            break;

          case 1:
            colorValue[j] = c * (cos(b * (i + a - (((2 * M_PI) / b) / 3)))) + d; // subtract one third of the period in order to keep waves off each other
            break;

          case 2:
            colorValue[j] = c * (cos(b * (i + a + (((2 * M_PI) / b) / 3)))) + d;
            break;

          default: // how about you do absolutely nothing
            Send("Something has gone terribly wrong");
            break;
          }

          if (colorValue[j] > MAX_BRIGHTNESS) { // max brightness
            colorValue[j] = MAX_BRIGHTNESS;
          } else if (colorValue[j] < 0) { // min brightness
            colorValue[j] = 0;
          }
        }
        break;

      case 2:
        for (int k = 0; k < 3; k++) { // scales color output depending on what the current color coefficients are
          colorValue[k] = (c * (cos(b * (i + a))) + d) * (rgb[k] / 255);

          if (colorValue[k] > MAX_BRIGHTNESS) { // max brightness
            colorValue[k] = MAX_BRIGHTNESS;
          }
          else if (colorValue[k] < 0) { // lower limit
            colorValue[k] = 0;
          }
        }

        break;

      default:
        Send("Select a Valid Mode:");
        Send("1 : TriColor");
        Send("2 : RGB Control");
        break;
      }

      pixels.setPixelColor(i, pixels.Color(colorValue[0], colorValue[1], colorValue[2]));
    }
    else { // if pixel disabled
      pixels.setPixelColor(i, 0, 0, 0);
    }
  }
  delay(delayValue);
  pixels.show();

  a += stepSize; // change a value to cycle smoothly
  if (a > M_PI * (1 / b)) {
    a = ((M_PI * -1) * (1 / b)) + stepSize;
  
  } else if (a < ((M_PI * -1) * (1 / b))) {
    a = ((M_PI * (1 / b))) - stepSize; // set to big
  }
}
