#include "Nodule.h"

#define DEFAULT_UPDATE_INTERVAL 100
#define DEFAULT_HUE_MODE        Perlin
#define DEFAULT_SATURATION_MODE Constant

Nodule::Nodule()
  : hue(UINT8_MAX),
    leds(nullptr),
    hueMode(Rainbow),
    noiseSeed(0),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(0),
    previousTime(0),
    saturation(UINT8_MAX),
    saturationMode(DEFAULT_SATURATION_MODE),
    updateInterval(DEFAULT_UPDATE_INTERVAL),
    value(UINT8_MAX)
{}

Nodule::Nodule(CRGB *leds)
  : hue(UINT8_MAX),
    leds(leds),
    hueMode(Rainbow),
    noiseSeed(random16()),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(MAX_SPEED * random16() / (float)UINT16_MAX),
    previousTime(0),
    saturation(UINT8_MAX),
    saturationMode(DEFAULT_SATURATION_MODE),
    updateInterval(DEFAULT_UPDATE_INTERVAL),
    value(UINT8_MAX)
{}

void Nodule::setUpdateInterval(uint16_t updateInterval) {
  this->updateInterval = updateInterval;
}

void Nodule::setHueMode(HueMode hueMode) {
  this->hueMode = hueMode;
}

void Nodule::setSaturationMode(SaturationMode saturationMode) {
  this->saturationMode = saturationMode;
}

void Nodule::update() {
  uint32_t now = millis();

  // has enough time elapsed for us to update?
  if ((now - previousTime) < this->updateInterval) {
    return;
  }

  // what hueMode are we in?
  switch(this->hueMode) {
  case Rainbow:
    updateRainbow();
    break;

  case Perlin:
  default:
    updatePerlin();
    break;

  }

  // paint the leds
  updateLeds();

  // keep track of when we last updated
  this->previousTime = now;
}

void Nodule::updateLeds() {
  for (uint8_t j = 0; j < NUM_LEDS_PER_NODE; j++) {
    this->leds[j] = CHSV(this->hue, this->saturation, this->value);
  }
}

void Nodule::updatePerlin() {
  this->hue = inoise8(noiseSeed);

  // accumulate time
  this->noiseSpeedAccumulator += this->noiseSpeedIncrement;

  // advance time - either 0 or 1 since we're casting a float of value: 0 < noiseSpeedAccumulator < 1
  this->noiseSeed += (uint16_t) this->noiseSpeedAccumulator;

  // clamp to less than 1
  if (this->noiseSpeedAccumulator > MAX_SPEED) {
    this->noiseSpeedAccumulator = MAX_SPEED - this->noiseSpeedAccumulator;
  }
}

void Nodule::updateRainbow() {
  this->hue += 1;
}
