#include <BluetoothSerial.h>
#include <Adafruit_NeoPixel.h>
#define PIN 19
#define NUM_PIXELS 300
#define MAX_BRIGHTNESS 200
#define delayValue 10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


double stepSize = 0.07;
String command = "";

boolean enable [NUM_PIXELS];
double colorCoefficient [] = {1, 1, 1};
double colorValue [] = {0,0,0};
double intensity;

double a = 0;
double b = M_PI * 0.07;
double c = 75;
double d = c;


BluetoothSerial BluetoothDevice;


void Send (String data) {

    BluetoothDevice.print("\n");
    for (int i = 0; i < data.length(); i++){
        BluetoothDevice.print(data.charAt(i));
    }
}
boolean equals (String one, String two) {

    if (one.length() == two.length()){
        for (int i = 0; i < one.length(); i++){
            if (one.charAt(i) != two.charAt(i)){
                return false;
            }
        }
    } else {
        return false;
    }

    return true;

}

void setup() {

    pixels.begin();
    pixels.show();

    for (int i = 0; i < NUM_PIXELS - 1; i++){
        enable[i] = true;

        pixels.setPixelColor(i, pixels.Color(random(100), random(100), random(100)));
        pixels.show();
    }

    Serial.begin(115200);
    BluetoothDevice.begin("Ram Ranch Really Rocks"); // bluetooth device name

}

void loop() {

    command = ""; // clear string to set it to new data

    while(BluetoothDevice.available()){ // get messages from bluetooth
        command += (char) BluetoothDevice.read();
    }
    command.toLowerCase(); // set command to lower case
     // take spacebar out of command
    for (int i = 0; i < command.length(); i++) { // take out spacebar
        if (command.charAt(i) == ' ') {
            command.remove(i,1);
            i--;
        }
    }


    if (!command.equals("")){ // if command has contents

        if (equals(command.substring(0,5), "step=")){ // command to change speed that sin wave moves
            stepSize = command.substring(5).toDouble();        

        } else if (command.substring(0, command.indexOf('(')).equals("rgb")){ // change color brightness, if bigger than 1, treat as rgb value

            if ((command.substring(command.indexOf('(') + 1, command.indexOf(','))).toDouble() > 1) {
                colorCoefficient[0] = (command.substring(command.indexOf('(') + 1, command.indexOf(','))).toDouble() / 255;
            } else {
                colorCoefficient[0] = (command.substring(command.indexOf('(') + 1, command.indexOf(','))).toDouble();
            }

            if ((command.substring(command.indexOf(',') + 1, command.lastIndexOf(','))).toDouble() > 1) {
                colorCoefficient[1] = (command.substring(command.indexOf(',') + 1, command.lastIndexOf(','))).toDouble() / 255;
            } else {
                colorCoefficient[1] = (command.substring(command.indexOf(',') + 1, command.lastIndexOf(','))).toDouble();
            }

            if ((command.substring(command.lastIndexOf(',') + 1, command.indexOf(')'))).toDouble() > 1) {
                colorCoefficient[2] = (command.substring(command.lastIndexOf(',') + 1, command.indexOf(')'))).toDouble() / 255;
            } else {
                colorCoefficient[2] = (command.substring(command.lastIndexOf(',') + 1, command.indexOf(')'))).toDouble();
            }
                                                                    
        } else if (command.charAt(0) == 'a'){
            a = command.substring(2).toDouble();

        } else if (command.charAt(0) == 'b'){
            b = M_PI * command.substring(2).toDouble();

        } else if (equals (command.substring(0,7), "period=") == true) { // set period
            b = (2 * M_PI) / command.substring(7).toDouble();

        } else if (command.charAt(0) == 'c'){
            c = command.substring(2).toDouble();
            d = c; // optional. I use this cause its easy
        
        } else if (command.charAt(0) == 'd') { // allows for constant colors, and to have none turned off
            d = c + command.substring(2).toDouble();

        } else if (equals (command.substring(0,7), "enable(") == true) { // enable/disable leds

            if (command.indexOf("-") == -1) { // if only working with one light
                enable [command.substring(command.indexOf('(') + 1, command.indexOf(')')).toInt()] = !enable [command.substring(command.indexOf('(') + 1, command.indexOf(')')).toInt()];

            } else { // change line of them
                for (int i = command.substring(command.indexOf('(') + 1, command.indexOf('-')).toInt(); i < command.substring(command.indexOf('-') + 1, command.indexOf(')')).toInt(); i++) {
                    enable [i] = !enable [i];
                }

            }

        } else if (equals("read", command)){ // read current values from the board

            Send ("Red = " + String(colorCoefficient[0]));
            Send ("Green = " + String(colorCoefficient[1]));
            Send ("Blue = " + String(colorCoefficient[2]));

            Send ("Step = " + String(stepSize));

            Send ("a = " + String(a)); // Kinda useless since it changes so much
            Send ("b = " + String(b / M_PI));
            Send ("c = " + String(c));
            Send ("d = " + String(d));

        } else if (equals("on", command)) { // enable all lights

            for (int i = 0; i < NUM_PIXELS; i++){
                enable [i] = true;
            }
        
        } else if (equals("off", command)) { // disable all lights
        
            for (int i = 0; i < NUM_PIXELS; i++){
                enable [i] = false;
            }
        
        }
    }

    for (int i = 0; i < NUM_PIXELS; i++){ // for all pixels

        if (enable[i] == true) {
            intensity = c * (cos(b * (i + a))) + d; // the big calculation


            for (int i = 0; i < (sizeof(colorValue) / sizeof(double)); i++){ // scales color output depending on what the current color coefficients are
                colorValue[i] = intensity * colorCoefficient[i];

                if (colorValue[i] > MAX_BRIGHTNESS){ // max brightness
                    colorValue[i] = MAX_BRIGHTNESS;
                }else if (colorValue[i] < 0){ // lower limit
                    colorValue[i] = 0;
                }
                
            }

            pixels.setPixelColor(i, pixels.Color(colorValue[0], colorValue[1], colorValue[2]));
        } else {
            pixels.setPixelColor(i, 0, 0, 0);
        }

    }
    delay (delayValue);
    pixels.show();

    a += stepSize; // change a value to cycle smoothly
    if (a > M_PI * (1/b)){
        a = ((M_PI * -1) * (1/b)) + stepSize;
    }

}
