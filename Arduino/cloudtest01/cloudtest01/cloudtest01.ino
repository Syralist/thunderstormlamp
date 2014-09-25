#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>

#define PIN 9
#define NUMPIXEL 21

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB | NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //Serial.begin(9600);
  Timer1.initialize(150000);
  Timer1.attachInterrupt(flash);
}

volatile unsigned long flashCount = 0;
volatile unsigned long loopCount = 0;

void flash() {
  uint32_t color;
  for(uint16_t i = 0; i<NUMPIXEL; i++)
  {
    strip.setPixelColor(i, 0);
  }
  switch(loopCount%4)
  {
    case 0:
    color = strip.Color(127,127,127);
    break;
    case 1:
    color = strip.Color(127,0,0);
    break;
    case 2:
    color = strip.Color(0,127,0);
    break;
    case 3:
    color = strip.Color(0,0,127);
    break;
    default:
    break;
  }
  strip.setPixelColor(flashCount%NUMPIXEL,color);
  strip.show();
  flashCount++;
  if(flashCount%NUMPIXEL == 0)
  {
    loopCount++;
  }
}

void loop() {
  
}
