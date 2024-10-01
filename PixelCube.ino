// Animated Fire Example - OctoWS2811 Library
//  http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
// 
// Based on the simple algorithm explained here:
//  http://caraesnaur.github.io/fire/
//
// This example code is in the public domain.

#include <OctoWS2811.h>

// The display size and color to use
const unsigned int width = 8;
const unsigned int height = 8;
const unsigned int numPins = 5;
byte pinList[numPins] = {2, 3, 4, 5, 6};

// OctoWS2811 objects
const int ledsPerPin = width * height;
DMAMEM int displayMemory[ledsPerPin * numPins * 3 / 4];
int drawingMemory[ledsPerPin * numPins * 3 / 4];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerPin, displayMemory, drawingMemory, config, numPins, pinList);

uint32_t canvas[ledsPerPin * numPins];

// Run setup once
void setup() {
  // turn on the display
  leds.begin();
  leds.show();
  randomSeed(analogRead(0));
}

elapsedMillis msec;

uint8_t H=0, S=191, V=0;
uint8_t MAX_V = 0x30;

// Run repetitively
void loop() {
  
  if (msec >= 45) {
    msec = 0;
    int i;

    // Find an unlit pixel
    i = random(0, (ledsPerPin * numPins) - 1);
    while((canvas[i] & 0xff) != 0) {
      i += random(0, ledsPerPin);
      i %= (ledsPerPin * numPins);
    }

    H = (H + 1) % 192;

    canvas[i] = 0x80000000 | (H << 16) | (S << 8) | V;

    for(i = 0; i < (ledsPerPin * numPins); i++) {
      if((canvas[i] & 0x80000000) && ((canvas[i] & 0xFF) < MAX_V)) {
        canvas[i] = (canvas[i] & 0xFFFFFF00) | ((canvas[i] & 0xFF) + 1);
      } else if((canvas[i] & 0xFF) > 0) {
        canvas[i] = (canvas[i] & 0x7FFFFF00) | ((canvas[i] & 0xFF) - 1teen);
      }
      leds.setPixel(i, setLedColorHSV(((canvas[i] >> 16) & 0xFF), ((canvas[i] >> 8) & 0xFF), (canvas[i] & 0xFF)));
    }
    leds.show();
  }
}

uint32_t setLedColorHSV(uint8_t h, uint8_t s, uint8_t v) {
  uint8_t R;
  uint8_t G;
  uint8_t B;
  
  // this is the algorithm to convert from RGB to HSV
  //h = (h * 192) / 256;  // 0..191
  unsigned int i = h / 32;   // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
  uint8_t pv = v * sInv / 256;  // pv will be in range 0 - 255
  uint8_t qv = v * (256 - s * f / 256) / 256;
  uint8_t tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
  case 0:
    R = v;
    G = tv;
    B = pv;
    break;
  case 1:
    R = qv;
    G = v;
    B = pv;
    break;
  case 2:
    R = pv;
    G = v;
    B = tv;
    break;
  case 3:
    R = pv;
    G = qv;
    B = v;
    break;
  case 4:
    R = tv;
    G = pv;
    B = v;
    break;
  case 5:
    R = v;
    G = pv;
    B = qv;
    break;
  }
  return (R << 16) | (G << 8) | B;
}
