#include <FastLED.h>
#include "config.h"

class Nodule {

public:
  Nodule();
  Nodule(CRGB *leds);
  void update();

private:
  CRGB     *leds;
  uint16_t noiseSeed;
  float    noiseSpeedAccumulator;
  float    noiseSpeedIncrement;
};
