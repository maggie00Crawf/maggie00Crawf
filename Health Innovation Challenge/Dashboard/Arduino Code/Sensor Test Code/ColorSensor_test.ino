#include <Wire.h> 
#include "SparkFunISL29125.h" 
SFE_ISL29125 RGB_sensor; 


void setup() 
{ 
  Serial.begin(115200); 

  if (RGB_sensor.init()){ 
    Serial.println("Sensor Initialization Successful\n\r");} 
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
 

void loop() 
{ 
  //Read sensor values for ambient lighting color 
  unsigned int red = RGB_sensor.readRed(); 
  unsigned int green = RGB_sensor.readGreen(); 
  unsigned int blue = RGB_sensor.readBlue(); 

  // //Print raw values for calibration 
  // Serial.print("Raw Values: "); 
  // Serial.print("("); Serial.print(red); 
  // Serial.print(", "); Serial.print(green); 
  // Serial.print(", "); Serial.print(blue);Serial.println(")"); 
  // Serial.println(); 


  // map read RGB values between 0 and 255 based on high and low calibration values 
  int redV = map(red, redlow, redhigh, 0, 255); 
  int greenV = map(green, greenlow, greenhigh, 0, 255); 
  int blueV = map(blue, bluelow, bluehigh, 0, 255); 


  // Constrain to values between 0 and 255 
  redVal = constrain(redV, 0, 255); 
  greenVal = constrain(greenV, 0, 255); 
  blueVal = constrain(blueV, 0, 255); 
  
 
  //Prints color code to the color being read on the sensor
  // Print out readings related to each color 
  Serial.print("Red: "); Serial.println(redVal,DEC); 
  Serial.print("Green: "); Serial.println(greenVal,DEC); 
  Serial.print("Blue: "); Serial.println(blueVal,DEC); 
} 