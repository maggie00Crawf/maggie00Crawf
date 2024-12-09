/*
Author: Hardit Sabharwal , Maggie Crawford
Date: 18th Oct , 2024
Project: Health Hub -Arduino Sensor Solution
*/

#include <Wire.h>
#include "rgb_lcd.h"
#include "Arduino_SensorKit.h"
#include <DFRobot_MAX30102.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "SparkFunISL29125.h"
#include "WS2812_Definitions.h"
#include <DFRobot_BMX160.h>


//PIN AND SENSOR DECLARATIONS
rgb_lcd lcd;
const int pinSound = A0;  // pin of LED
DFRobot_MAX30102 particleSensor;
SFE_ISL29125 RGB_sensor;

// simple sensor variables
int SoundVal = 0;
int temp = 0;
int humidity = 0;

// lcd display variables
const int colorR = 0;
const int colorG = 0;
const int colorB = 255;
char myString[] = "potato";
const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;
boolean changeCol = false;

// heart rate variables
int32_t SPO2;           //SPO2
int8_t SPO2Valid;       //Flag to display if SPO2 calculation is valid
int32_t heartRate;      //Heart-rate
int8_t heartRateValid;  //Flag to display if heart-rate calculation is valid

//LED setup
#define LED_PIN 6      //pmw pin
#define LED_COUNT 5    //number of individual LED's
#define BRIGHTNESS 50  //max 255
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// RGB sensor variables
unsigned int colorSensorRed = 0;
unsigned int colorSensorBlue = 0;
unsigned int colorSensorGreen = 0;
unsigned int lux = 0;
// Calibration values (must be done for every new environment, or when lighting changes)
unsigned int redhigh = 3800;
unsigned int redlow = 200;
unsigned int greenhigh = 4600;
unsigned int greenlow = 300;
unsigned int bluehigh = 2800;
unsigned int bluelow = 200;
int redReading = 0;
int blueReading = 0;
int greenReading = 0;

//LED DECLARATIONS
boolean changeLed = false;
int red_show, green_show, blue_show = 255;

//ACCELERATION DECLARATIONS
DFRobot_BMX160 bmx160;

void setup() {
  Serial.begin(115200);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);

  // Print a message to the LCD.
  lcd.print("hola, world!");
  delay(1000);

  // Initialize temp/humidity sensor
  Environment.setPin(4);
  Environment.begin();

  //Initialize LEDS
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  //Initialize RGB Sensor
  if (RGB_sensor.init()) {
    Serial.println("Sensor Initialization Successful\n\r");
  }
  //Initialize Oximeter Sensor

  if (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
  }

  particleSensor.sensorConfiguration(/*ledBrightness=*/150, /*sampleAverage=*/SAMPLEAVG_8,
                                     /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_200,
                                     /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);

  //Initialize Accelerometer
  if (bmx160.begin() != true){
    Serial.println("init false");
    while(1);
  }
}

void loop() {
  calculateSound();
  calculateTempHum();
  calculateHR();
  calculateLux();
  calculateColors();
  printJson();
  // Expecting: heart_rate, oxygen, temperature, humidity, noise, light

  recvWithStartEndMarkers();
  replyToPython();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;
  char colorFlag = '+';
  char ledFlag = 'C';
  static unsigned long startTime = 0;          // Track the start time for the timeout
  const unsigned long timeoutDuration = 1000;  // Timeout in milliseconds

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {

      if (millis() - startTime >= timeoutDuration) {
        recvInProgress = false;  // Timeout occurred, stop receiving
        ndx = 0;                 // Reset index
        Serial.println("ERROR: RECEIVING TIMED OUT");
        return;  // Exit function
      }

      if (rc == colorFlag) {
        changeCol = true;
        continue;
      }

      if (rc == ledFlag) {
        changeLed = true;
        continue;
      }

      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
      startTime = millis();  // Record the time when receiving starts
    }
  }
}

void replyToPython() {

  if (newData == true) {
    if (changeCol == false && changeLed == false) {
      lcd.clear();
      Serial.println(receivedChars);
      lcd.print(receivedChars);
    }
    newData = false;
  }

  if (changeCol == true) {
    unsigned long result = strtoul(receivedChars, NULL, 16);
    byte red = (result & 0xFF0000) >> 16;
    byte green = (result & 0x00FF00) >> 8;
    byte blue = (result & 0x0000FF);
    lcd.setRGB(red, green, blue);

    changeCol = false;
  }

  if (changeLed == true) {
    parseColorValues();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, red_show, green_show, blue_show);
    }
    strip.show();
    changeLed = false;
  }
  newData = false;
}

void calculateSound() {
  SoundVal = analogRead(pinSound);
}

void calculateHR() {
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
}

void calculateTempHum() {
  temp = Environment.readTemperature();
  humidity = Environment.readHumidity();
}

void calculateLux() {
  colorSensorRed = RGB_sensor.readRed();
  colorSensorGreen = RGB_sensor.readGreen();
  colorSensorBlue = RGB_sensor.readBlue();
  lux = (colorSensorRed + colorSensorGreen + colorSensorBlue) / 3;
}

void calculateAccel() {
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
}

void calculateColors() {
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();

  ////Color declaration for LEDs
  //Map read RGB values between 0 and 255 based on high and low calibration values
  int redV = map(red, redlow, redhigh, 0, 255);
  int greenV = map(green, greenlow, greenhigh, 0, 255);
  int blueV = map(blue, bluelow, bluehigh, 0, 255);

  // Constrain to values between 0 and 255
  redReading = constrain(redV, 0, 255);
  greenReading = constrain(greenV, 0, 255);
  blueReading = constrain(blueV, 0, 255);
}
void color_3digit(int color) {
  if (color < 100) { Serial.print("0"); }
  if (color < 10) { Serial.print("0"); }
  Serial.print(color);
}

void parseColorValues() {
  if (strlen(receivedChars) < 9) {
    Serial.println("ERROR: LED recieved incorrect color");
    red_show = 0;
    green_show = 0;
    blue_show = 0;
    return;
  }

  red_show = atoi(strncpy(new char[3], receivedChars, 3));
  green_show = atoi(strncpy(new char[3], receivedChars + 3, 3));
  blue_show = atoi(strncpy(new char[3], receivedChars + 6, 3));
}

void printJson() {

  // SOUND
  Serial.print("{");
  Serial.print("\"sound\":\"");
  Serial.print(SoundVal);
  Serial.print("\"");
  Serial.print(",");

  //TEMPERATURE
  Serial.print("\"temperature\":\"");
  Serial.print(temp);
  Serial.print("\"");
  Serial.print(",");

  //HUMIDITY
  Serial.print("\"humidity\":\"");
  Serial.print(humidity);
  Serial.print("\"");
  Serial.print(",");

  //LUX
  Serial.print("\"lux\":\"");
  Serial.print(lux);
  Serial.print("\"");
  Serial.print(",");

  //RGB Colors
  Serial.print("\"rgbSensor\":\"");
  color_3digit(redReading);
  color_3digit(greenReading);
  color_3digit(blueReading);
  Serial.print("\"");
  Serial.print(",");

  //OXIMETER
  Serial.print("\"oximeter\":\"");
  Serial.print(heartRateValid, DEC);
  Serial.print(",");
  Serial.print(heartRate, DEC);
  Serial.print(",");
  Serial.print(SPO2Valid, DEC);
  Serial.print(",");
  Serial.print(SPO2, DEC);
  Serial.print("\"");
  Serial.print(",");

  //ACCELEROMETER
  Serial.print("\"x\":");
  Serial.print(Oaccel.x);
  Serial.print(",");
  Serial.print("\"y\":");
  Serial.print(Oaccel.y);
  Serial.print(",");
  Serial.print("\"z\":");
  Serial.print(Oaccel.z);
  Serial.println("}");
}
/*********************************************************************************************************
    END FILE
*********************************************************************************************************/
