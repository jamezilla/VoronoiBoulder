#include "Cell.h"
#include "Generators.h"

//
// Hue
//

void Cell::setHue(uint8_t hue) {
  this->hue.value = hue;
}


void Cell::setHueInterval(uint16_t interval) {
  this->hue.interval = interval;
}


void Cell::setHueIntervalMode(Mode mode) {
  this->hue.intervalMode = mode;
}


void Cell::setHueMin(uint8_t hueMin) {
  this->hue.valueMin = hueMin;
}


void Cell::setHueMax(uint8_t hueMax) {
  this->hue.valueMax = hueMax;
}


void Cell::setHueMode(Mode mode) {
  this->hue.valueMode = mode;
}


//
// Saturation
//

void Cell::setSaturation(uint8_t saturation) {
  this->saturation.value = saturation;
}


void Cell::setSaturationInterval(uint16_t interval) {
  this->saturation.interval = interval;
}


void Cell::setSaturationIntervalMode(Mode mode) {
  this->saturation.intervalMode = mode;
}


void Cell::setSaturationMin(uint8_t saturationMin) {
  this->saturation.valueMin = saturationMin;
}


void Cell::setSaturationMax(uint8_t saturationMax) {
  this->saturation.valueMax = saturationMax;
}


void Cell::setSaturationMode(Mode mode) {
  this->saturation.valueMode = mode;

  switch(mode) {
  case ModeSinusoidal:
    Generators::jumpToSineLutPosition(this->saturation.value, this->saturation.valueLutIndex);
    break;
  }
}


//
// Value
//

void Cell::setValue(uint8_t value) {
  this->value.value = value;
}


void Cell::setValueInterval(uint16_t interval) {
  this->value.interval = interval;
}


void Cell::setValueIntervalMode(Mode mode) {
  this->value.intervalMode = mode;
}


void Cell::setValueMin(uint8_t valueMin) {
  this->value.valueMin = valueMin;
}


void Cell::setValueMax(uint8_t valueMax) {
  this->value.valueMax = valueMax;
}


void Cell::setValueMode(Mode mode) {
  this->value.valueMode = mode;
}


// now must be passed in, otherwise the cells will accumulate error and drop
// out of sync with each other
void Cell::update(uint32_t now) {
  this->now = now;

  updateChannel(this->hue);
  updateChannel(this->saturation);
  updateChannel(this->value);

  updateLeds();
}

void Cell::updateChannel(Channel &channel) {
  uint32_t elapsed = now - channel.previousUpdate;

  // has enough time elapsed for us to update?
  if (elapsed < channel.interval) {
    return;
  }

  channel.previousUpdate = now;


  // update the value

  switch(channel.valueMode) {
  case ModeAlternate:
    Generators::alternate(channel.value);
    break;
  case ModeAlternateDrift:
    Generators::alternateDrift(channel.value);
    break;
  case ModePerlin:
    Generators::perlin(channel.value, channel.valueNoiseSeed);
    break;
  case ModeReverseSawtooth:
    Generators::reverseSawtooth(channel.value);
    break;
  case ModeSawtooth:
    Generators::sawtooth(channel.value);
    break;
  case ModeSinusoidal:
    Generators::sinusoidal(channel.value, channel.valueLutIndex);
    break;
  case ModeConstant:
  default:
    // noop
    break;
  }


  // update the interval

  switch(channel.intervalMode) {
  // case ModeAlternateComplimentary:
  //   Generators::alternateComplimentary(channel.interval);
  //   break;
  case ModeReverseSawtooth:
    Generators::reverseSawtooth(channel.interval);
    break;
  case ModeSawtooth:
    Generators::sawtooth(channel.interval);
    break;
  case ModeSinusoidal:
    Generators::sinusoidal(channel.interval, channel.intervalLutIndex);
    break;
  case ModeConstant:
  default:
    // noop
    break;
  }
}


void Cell::updateLeds() {
  // map the numbers so they're between min/max for each channel
  uint8_t hue = map(this->hue.value, 0, UINT8_MAX, this->hue.valueMin, this->hue.valueMax);
  uint8_t saturation = map(this->saturation.value, 0, UINT8_MAX, this->saturation.valueMin, this->saturation.valueMax);
  uint8_t value = map(this->value.value, 0, UINT8_MAX, this->value.valueMin, this->value.valueMax);

  for (uint8_t j = 0; j < this->numLeds; j++) {
    this->leds[j] = CHSV(hue, saturation, value);
  }
}
