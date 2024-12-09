#include <Adafruit_NeoPixel.h> 
#define LED_PIN 6 //change to match board setup 
#define LED_COUNT 9 //change to # of lights on LED strip you want to display the color 
#define BRIGHTNESS 10 //set in range 0-255 

#include <Adafruit_NeoPixel.h> 
#ifdef __AVR__ 
#include <avr/power.h> 
#endif 

//Declare our NeoPixel strip object: 
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); 


void setup() { 
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000) 
  clock_prescale_set(clock_div_1); 
#endif
 
  strip.begin(); 
  strip.show(); 
  strip.setBrightness(BRIGHTNESS); 
} 

 
//color is light purple 
//Change color values to test other colors! 
uint8_t red = 240; 
uint8_t green = 117; 
uint8_t blue = 128; 
 

void loop() { 
  for(int i=0; i<strip.numPixels(); i++) {  //For each pixel in strip... 
    strip.setPixelColor(i, red, green, blue); //Set pixel's color in memory 
    delay(50); 
    strip.show(); //Update LED strip colors           
  } 

} 