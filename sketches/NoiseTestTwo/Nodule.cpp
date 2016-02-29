#include "Nodule.h"

Nodule::Nodule()
  : leds(nullptr),
    noiseSeed(0),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(0)
{}

Nodule::Nodule(CRGB *leds)
  : leds(leds),
    noiseSeed(random16()),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(MAX_SPEED * random16() / (float)UINT16_MAX)
{}

void Nodule::update() {
    uint8_t hue = inoise8(noiseSeed);

    for (uint8_t j = 0; j < NUM_LEDS_PER_NODE; j++) {
      this->leds[j] = CHSV(hue, UINT8_MAX, UINT8_MAX);
    }

    // accumulate time
    this->noiseSpeedAccumulator += this->noiseSpeedIncrement;

    // advance time - either 0 or 1 since we're casting a float of value: 0 < noiseSpeedAccumulator < 1
    this->noiseSeed += (uint16_t) this->noiseSpeedAccumulator;

    // clamp to less than 1
    if (this->noiseSpeedAccumulator > MAX_SPEED) {
      this->noiseSpeedAccumulator = MAX_SPEED - this->noiseSpeedAccumulator;
    }
}
