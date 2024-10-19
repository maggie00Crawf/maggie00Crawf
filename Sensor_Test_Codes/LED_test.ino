#include <Adafruit_NeoPixel.h>
#define LED_PIN 6
#define LED_COUNT 9
#define BRIGHTNESS 10

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)



void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

//color is light purple
uint8_t red = 240;
uint8_t green = 117;
uint8_t blue = 128;
int wait = 10; //time held on color


void loop() {

  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, red, green, blue);         //  Set pixel's color (in RAM)
    delay(wait);              //  Pause for a moment
    strip.show();             //  Update strip to match          
  }

}

