#include <FastLED.h>
#include "config.h"
#include "Nodule.h"

CRGB   leds[NUM_LEDS];
Nodule nodules[NUM_NODES];

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, 6, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  for (uint8_t i = 0; i < NUM_NODES; i++) {
    CRGB *nodeStart = &(leds[i*NUM_LEDS_PER_NODE]);
    nodules[i] = Nodule(nodeStart);
    nodules[i].setHueUpdateInterval(i*7+20);
    nodules[i].setValueMode(Nodule::ModeSinusoidal);
    nodules[i].setValueUpdateInterval(i*33);
  }
}

void loop() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < NUM_NODES; i++) {
    nodules[i].update(now);
  }

  FastLED.show();
}
