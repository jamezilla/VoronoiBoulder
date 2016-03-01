#include "Nodule.h"

#define DEFAULT_HUE_MODE                   HueModeRainbow
#define DEFAULT_HUE_UPDATE_INTERVAL        100
#define DEFAULT_SATURATION_MODE            SaturationModeConstant
#define DEFAULT_SATURATION_UPDATE_INTERVAL 100
#define DEFAULT_VALUE_MODE                 ValueModeConstant
#define DEFAULT_VALUE_UPDATE_INTERVAL      100

Nodule::Nodule()
  : hue(UINT8_MAX),
    hueMode(DEFAULT_HUE_MODE),
    hueUpdateInterval(DEFAULT_HUE_UPDATE_INTERVAL),
    leds(nullptr),
    noiseSeed(0),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(0),
    previousTime(0),
    saturation(UINT8_MAX),
    saturationMode(DEFAULT_SATURATION_MODE),
    saturationUpdateInterval(DEFAULT_SATURATION_UPDATE_INTERVAL),
    value(UINT8_MAX),
    valueMode(DEFAULT_VALUE_MODE),
    valueUpdateInterval(DEFAULT_VALUE_UPDATE_INTERVAL)
{}


Nodule::Nodule(CRGB *leds)
  : hue(128),
    hueMode(DEFAULT_HUE_MODE),
    hueUpdateInterval(DEFAULT_HUE_UPDATE_INTERVAL),
    leds(leds),
    noiseSeed(random16()),
    noiseSpeedAccumulator(0),
    noiseSpeedIncrement(MAX_SPEED * random16() / (float)UINT16_MAX),
    previousTime(0),
    saturation(UINT8_MAX),
    saturationMode(DEFAULT_SATURATION_MODE),
    saturationUpdateInterval(DEFAULT_SATURATION_UPDATE_INTERVAL),
    value(UINT8_MAX),
    valueMode(DEFAULT_VALUE_MODE),
    valueUpdateInterval(DEFAULT_VALUE_UPDATE_INTERVAL)
{}


void Nodule::setHue(Hue hue) {
  this->hue = hue;
}


void Nodule::setHueMode(HueMode mode) {
  this->hueMode = mode;
}


void Nodule::setHueUpdateInterval(uint16_t interval) {
  this->hueUpdateInterval = interval;
}


void Nodule::setSaturation(Saturation saturation) {
  this->saturation = saturation;
}


void Nodule::setSaturationMode(SaturationMode mode) {
  this->saturationMode = mode;
}


void Nodule::setSaturationUpdateInterval(uint16_t interval) {
  this->saturationUpdateInterval = interval;
}


void Nodule::setValue(Value value) {
  this->value = value;
}


void Nodule::setValueMode(ValueMode mode) {
  this->valueMode = mode;
}


void Nodule::setValueUpdateInterval(uint16_t interval) {
  this->valueUpdateInterval = interval;
}


void Nodule::update() {
  uint32_t now = millis();
  uint32_t elapsed = now - previousTime;

  // has enough time elapsed for us to update?
  if (elapsed < this->hueUpdateInterval) {
    return;
  }

  updateHue(elapsed);
  updateSaturation(elapsed);
  updateValue(elapsed);
  updateLeds();

  // keep track of when we last updated
  this->previousTime = now;
}


void Nodule::updateHue(uint32_t &elapsed) {
  // has enough time elapsed for us to update?
  if (elapsed < this->hueUpdateInterval) {
    return;
  }

  // update the hue
  switch(this->hueMode) {
  case HueModeRainbow:
    updateRainbow();
    break;
  case HueModePerlin:
  default:
    updatePerlin();
    break;
  }
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


void Nodule::updateSaturation(uint32_t &elapsed) {
  // has enough time elapsed for us to update?
  if (elapsed < this->saturationUpdateInterval) {
    return;
  }

  // update the saturation
  switch(this->saturationMode) {
  case SaturationModeConstant:
  default:
    // noop
    break;
  }
}


void Nodule::updateValue(uint32_t &elapsed) {
  // has enough time elapsed for us to update?
  if (elapsed < this->valueUpdateInterval) {
    return;
  }

  // update the value
  switch(this->valueMode) {
  case ValueModeConstant:
  default:
    // noop
    break;
  }
}
