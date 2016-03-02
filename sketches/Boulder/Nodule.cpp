#include "Nodule.h"
#include "Generators.h"

void Nodule::setHue(uint8_t hue) {
  this->hue.value = hue;
}


void Nodule::setHueMode(Mode mode) {
  this->hue.valueMode = mode;
}


void Nodule::setHueIntervalMode(Mode mode) {
  this->hue.intervalMode = mode;
}


void Nodule::setHueInterval(uint16_t interval) {
  this->hue.interval = interval;
}


void Nodule::setSaturation(uint8_t saturation) {
  this->saturation.value = saturation;
}


void Nodule::setSaturationIntervalMode(Mode mode) {
  this->saturation.intervalMode = mode;
}


void Nodule::setSaturationMode(Mode mode) {
  this->saturation.valueMode = mode;

  switch(mode) {
  case ModeSinusoidal:
    Generators::jumpToSineLutPosition(this->saturation.value, this->saturation.valueLutIndex);
    break;
  }
}


void Nodule::setSaturationInterval(uint16_t interval) {
  this->saturation.interval = interval;
}


void Nodule::setValue(uint8_t value) {
  this->value.value = value;
}


void Nodule::setValueIntervalMode(Mode mode) {
  this->value.intervalMode = mode;
}


void Nodule::setValueMode(Mode mode) {
  this->value.valueMode = mode;
}


void Nodule::setValueInterval(uint16_t interval) {
  this->value.interval = interval;
}


// now must be passed in, otherwise the nodules will accumulate error and drop
// out of sync with each other
void Nodule::update(uint32_t now) {
  this->now = now;

  updateChannel(this->hue);
  updateChannel(this->saturation);
  updateChannel(this->value);

  updateLeds();
}

void Nodule::updateChannel(Channel &channel) {
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


void Nodule::updateLeds() {
  for (uint8_t j = 0; j < LEDS_PER_NODULE; j++) {
    this->leds[j] = CHSV(this->hue.value, this->saturation.value, this->value.value);
  }
}
