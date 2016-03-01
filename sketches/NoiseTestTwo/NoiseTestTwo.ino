#include <FastLED.h>
#include "config.h"
#include "Nodule.h"

uint8_t sineLUT[] = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,
  187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,
  233,234,236,238,239,240,242,243,244,245,247,248,249,249,250,251,252,252,253,253,
  253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,
  244,243,242,240,239,238,236,234,233,231,229,227,225,223,221,219,217,215,212,210,
  208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,
  152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,
  87,84,81,78,76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,
  21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,
  5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,33,35,37,39,42,44,46,49,51,54,
  56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};

// The leds
CRGB   leds[NUM_LEDS];
Nodule nodules[NUM_NODES];

void setup() {
  FastLED.addLeds<WS2812B, 6, RGB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  for (uint8_t i = 0; i < NUM_NODES; i++) {
    CRGB *nodeStart = &(leds[i*NUM_LEDS_PER_NODE]);
    nodules[i] = Nodule(nodeStart);

    Nodule::HueMode hm = (Nodule::HueMode) random(0, Nodule::HueModeSize);
    nodules[i].setHueMode(hm);

    uint16_t hui = (uint16_t) random(0, 10);
    nodules[i].setHueUpdateInterval(hui);

    Nodule::SaturationMode sm = (Nodule::SaturationMode) random(0, Nodule::SaturationModeSize);
    nodules[i].setSaturationMode(sm);

    uint16_t sui = (uint16_t) random(0, 500);
    nodules[i].setSaturationUpdateInterval(sui);

    Nodule::ValueMode vm = (Nodule::ValueMode) random(0, Nodule::ValueModeSize);
    nodules[i].setValueMode(vm);

    uint16_t vui = (uint16_t) random(0, 100);
    nodules[i].setValueUpdateInterval(vui);
  }
}

void loop() {
  for (uint8_t i = 0; i < NUM_NODES; i++) {
    nodules[i].update();
  }

  FastLED.show();
}
