//color of LED will be adjusted based on color of ambient lighting
//trying to get pyton hour value to chose which color LEDs become
//Goal is for led and natural light to
//Color sensor needs to be calibrated to room lighting before use


//////////Library and Board Setup
//LED setup
#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"
#define LED_PIN 6      //pmw pin
#define LED_COUNT 3    //number of individual LED's
#define BRIGHTNESS 50  //max 255
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Color sensor setup
#include <Wire.h>
#include "SparkFunISL29125.h"
SFE_ISL29125 RGB_sensor;

//Noise Sensor Setup
const int Noise_pin = A0;




void setup() {
Serial.begin(9600);
strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
strip.show();   // Turn OFF all pixels ASAP
strip.setBrightness(BRIGHTNESS);
if (RGB_sensor.init()) {
Serial.println("Sensor Initialization Successful\n\r");
}
}



///////////Global variables declaration
// Calibration values (must be done for every new environment, or when lighting changes)
unsigned int redhigh = 3800;
unsigned int redlow = 200;
unsigned int greenhigh = 4600;
unsigned int greenlow = 300;
unsigned int bluehigh = 2800;
unsigned int bluelow = 200;
//2-way python-arduinocomunication declarations
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
const byte numChars = 64;
char color_receivedChars[numChars];
bool newData = false;
int red_show, green_show, blue_show = 255;
int lux = 0;
//Noise Level value
long Noise_Level;



void loop() {
delay(100);
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


////Color declaration for LEDs
//Map read RGB values between 0 and 255 based on high and low calibration values
int redV = map(red, redlow, redhigh, 0, 255);
int greenV = map(green, greenlow, greenhigh, 0, 255);
int blueV = map(blue, bluelow, bluehigh, 0, 255);

// Constrain to values between 0 and 255
int redVal = constrain(redV, 0, 255);
int greenVal = constrain(greenV, 0, 255);
int blueVal = constrain(blueV, 0, 255);
delay(100);
// Read_Noise_Level();

//Print color code to Serial Monitor in form "REDGREBLU"
Serial.print("=");
color_3digit(redVal); color_3digit(greenVal); color_3digit(blueVal); 
Serial.println("=");
// Serial.print("#"); Serial.print(redVal, HEX); Serial.print(greenVal, HEX); Serial.print(blueVal, HEX); Serial.print(".");
// Serial.println(Noise_Level);


/////////////////////////////////////python code processing///////////////////////////////////////////////////////
//Read color codes from Python script
color_recvWithStartEndMarkers();
Serial.println(color_receivedChars);
parseColorValues(color_receivedChars);

//Serial.print(red_show);
showLights();
Calculate_Lux(red, green, blue);
delay(100);
//Print in JSON string form
Serial.print("{");
Serial.print("\"color\":\"");
Serial.print(red_show);
Serial.print(",");
Serial.print(green_show);
Serial.print(",");
Serial.print(blue_show);
Serial.print(",");
Serial.print(lux);   
Serial.print("\"");
Serial.println("}");

}



void parseColorValues(char input[]) {
// red_show = (input[0] - '0') * 100 + (input[1] - '0') * 10 + (input[2] - '0');
// blue_show = (input[3] - '0') * 100 + (input[4] - '0') * 10 + (input[5] - '0');
// green_show = (input[6] - '0') * 100 + (input[7] - '0') * 10 + (input[8] - '0');

// Split the number into three 3-digit parts
red_show = atoi(strncpy(new char[3], input, 3));
green_show = atoi(strncpy(new char[3], input + 3, 3));
blue_show = atoi(strncpy(new char[3], input + 6, 3));

// Serial.print("Red: ");
// Serial.println(red_show);
// Serial.print("Blue: ");
// Serial.println(blue_show);
// Serial.print("Green: ");
// Serial.println(green_show);
}


//formats RGB values in 3 digits 
void color_3digit(int color) {
if (color < 100){ Serial.print("0");}
if (color < 10){ Serial.print("0");}
Serial.print(color);
}


//Example--> startMarker = "[C:" and endMarker = "]"
char color_recvWithStartEndMarkers() {
char startMarker = '[C:';
char endMarker = ']';
static boolean recvInProgress = false;
static byte ndx = 0;
char rc;

while (Serial.available() > 0 && newData == false) {
rc = Serial.read();

if (recvInProgress == true) {
if (rc != endMarker) {
  color_receivedChars[ndx] = rc;
  ndx++;
  if (ndx >= numChars) {
    ndx = numChars - 1;
  }
} else {
  color_receivedChars[ndx] = '\0';  // terminate the string
  recvInProgress = false;
  ndx = 0;
  newData = true;
}
}

else if (rc == startMarker) {
recvInProgress = true;
}
}
}


void showLights() {
if (newData == true) {
newData = false;
//prints ligth intensity to LEDs
for (int i = 0; i < LED_COUNT; i++) {
strip.setPixelColor(i, red_show, green_show, blue_show);
}
strip.show();
}
}


void Calculate_Lux(unsigned int red, unsigned int green, unsigned int blue) {
lux = (red + green + blue)/3;
}

// void Read_Noise_Level() {
//   //Sound sensor output
//   for(int i=0; i<32; i++)
//   {Noise_Level += analogRead(Noise_pin);}
//   Noise_Level >>= 5;
//   // delay(50);
// }

