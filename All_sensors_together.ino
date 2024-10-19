//LED setup
#include <Adafruit_NeoPixel.h>
#define LED_PIN 6
#define LED_COUNT 9
#define BRIGHTNESS 10
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Color Sensor Setup
#include <Wire.h>
#include "SparkFunISL29125.h"
SFE_ISL29125 RGB_sensor;

//Accelerometer Setup
#include <DFRobot_BMX160.h>
DFRobot_BMX160 bmx160;


void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(115200);

  //LED setup
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  //Color Sensor Setup
  if (RGB_sensor.init()){
    Serial.println("Sensor Initialization Successful\n\r");}
    RGB_sensor.config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG3_NO_INT);

//Accelerometer Setup
Serial.begin(115200);
  delay(100); 
  if (bmx160.begin() != true){
    Serial.println("init false");
    while(1);
  }
}
//No Sound sensor setup
const int Noise_pin = 2;



//color is light purple
uint8_t red = 240;
uint8_t green = 117;
uint8_t blue = 128;

//Color sensor declarations
unsigned int redhigh = 3800;
unsigned int redlow = 200;
unsigned int greenhigh = 4600;
unsigned int greenlow = 300;
unsigned int bluehigh = 2800;
unsigned int bluelow = 200;
int redVal = 0;
int greenVal = 0;
int blueVal = 0;

//Sound sensor declarations
const int pinAdc = A0;

void loop() {

  //color output
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, red, green, blue);
    strip.show();   
    delay(50);}
Serial.println("Light is on");

  //Color sensor output
  //Read sensor values for ambient lightign color
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();
  // map read RGB values between 0 and 255 based on high and low calibration values
  int redV = map(red, redlow, redhigh, 0, 255);
  int greenV = map(green, greenlow, greenhigh, 0, 255);
  int blueV = map(blue, bluelow, bluehigh, 0, 255);
  // Constrain to values between 0 and 255
  redVal = constrain(redV, 0, 255);
  greenVal = constrain(greenV, 0, 255);
  blueVal = constrain(blueV, 0, 255);
  //Prints color code in the form (R, G, B)
  Serial.print("Color: ("); Serial.print(redVal,DEC);
  Serial.print(", "); Serial.print(greenVal,DEC);
  Serial.print(", "); Serial.print(blueVal,DEC);Serial.println(")");
  Serial.println();
  delay(50);

//Accelerometer output
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
  Serial.print("A ");
  Serial.print("X: "); Serial.print(Oaccel.x    ); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Oaccel.y    ); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Oaccel.z    ); Serial.print("  ");
  Serial.println("m/s^2");
  Serial.println("");
  delay(50);

  //Sound sensor output
    long sum = 0;
    for(int i=0; i<32; i++)
    {sum += analogRead(Noise_pin);}
    sum >>= 5;
    Serial.println(sum);
    delay(50);
}

