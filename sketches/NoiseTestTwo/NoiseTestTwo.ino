#include <FastLED.h>
#include "config.h"
#include "Nodule.h"

// The leds
CRGB   leds[NUM_LEDS];
Nodule nodules[NUM_NODES];

void setup() {
  FastLED.addLeds<WS2812B, 6, RGB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  for (uint8_t i = 0; i < NUM_NODES; i++) {
    CRGB *nodeStart = &(leds[i*NUM_LEDS_PER_NODE]);
    nodules[i] = Nodule(nodeStart);
  }
}

void loop() {
  for (uint8_t i = 0; i < NUM_NODES; i++) {
    nodules[i].update();
  }

  FastLED.show();
}
