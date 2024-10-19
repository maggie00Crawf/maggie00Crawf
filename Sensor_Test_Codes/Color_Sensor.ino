/******************************************************************************
ISL29125_basics.ino
Simple example for using the ISL29125 RGB sensor library.
Jordan McConnell @ SparkFun Electronics
11 Apr 2014
https://github.com/sparkfun/SparkFun_ISL29125_Breakout_Arduino_Library

This example declares an SFE_ISL29125 object called RGB_sensor. The 
object/sensor is initialized with a basic configuration so that it continuously
samples the light intensity of red, green and blue spectrums. These values are
read from the sensor every 2 seconds and printed to the Serial monitor.

Developed/Tested with:
Arduino Uno
Arduino IDE 1.0.5

Requires:
SparkFun_ISL29125_Arduino_Library

This code is beerware.
Distributed as-is; no warranty is given. 
******************************************************************************/

#include <Wire.h>
#include "SparkFunISL29125.h"

// Declare sensor object
SFE_ISL29125 RGB_sensor;

void setup()
{

  // Initialize serial communication
  Serial.begin(115200);


  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init()){
    Serial.println("Sensor Initialization Successful\n\r");}
  

  //Config settings
  //CFG1_MODE can be changed to read signle color values instead of RGB
  //CFG1_10KLUX can be changed to 375 for dark environments
  //CFG2 and CFG3 can be changed- see data sheet
    RGB_sensor.config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG3_NO_INT);

}


// Calibration values (must be done for every new environment, or when lighting changes)
unsigned int redhigh = 3800;
unsigned int redlow = 200;
unsigned int greenhigh = 4600;
unsigned int greenlow = 300;
unsigned int bluehigh = 2800;
unsigned int bluelow = 200;

// Declare RGB Values 
int redVal = 0;
int greenVal = 0;
int blueVal = 0;

// Read sensor values for each color and print them to serial monitor
void loop()
{
  //Read sensor values for ambient lightign color
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();

  // //Print raw values for calibration
  // Serial.print("Raw Values: ");
  // Serial.print("("); Serial.print(red);
  // Serial.print(", "); Serial.print(green);
  // Serial.print(", "); Serial.print(blue);Serial.println(")");
  // Serial.println();


  /////////Color declaration for LEDs
  // map read RGB values between 0 and 255 based on high and low calibration values
  int redV = map(red, redlow, redhigh, 0, 255);
  int greenV = map(green, greenlow, greenhigh, 0, 255);
  int blueV = map(blue, bluelow, bluehigh, 0, 255);

  // Constrain to values between 0 and 255
  redVal = constrain(redV, 0, 255);
  greenVal = constrain(greenV, 0, 255);
  blueVal = constrain(blueV, 0, 255);

  // Print out readings related to each color
  Serial.print("Red: "); Serial.println(redVal,DEC);
  Serial.print("Green: "); Serial.println(greenVal,DEC);
  Serial.print("Blue: "); Serial.println(blueVal,DEC);

  //Prints color code in the form (R, G, B)
  Serial.print("Color: ("); Serial.print(redVal,DEC);
  Serial.print(", "); Serial.print(greenVal,DEC);
  Serial.print(", "); Serial.print(blueVal,DEC);Serial.println(")");
  Serial.println();
  delay(100);

}
