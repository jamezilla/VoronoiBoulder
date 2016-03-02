#include "Nodule.h"
#include "Generators.h"

void Nodule::setHue(uint8_t hue) {
  this->hue.value = hue;
}


void Nodule::setHueMode(Mode mode) {
  this->hue.mode = mode;
}


void Nodule::setHueUpdateInterval(uint16_t interval) {
  this->hue.updateInterval = interval;
}


void Nodule::setSaturation(uint8_t saturation) {
  this->saturation.value = saturation;
}


void Nodule::setSaturationMode(Mode mode) {
  this->saturation.mode = mode;

  switch(mode) {
  case ModeSinusoidal:
    Generators::jumpToSineLutPosition(this->saturation);
    break;
  }
}


void Nodule::setSaturationUpdateInterval(uint16_t interval) {
  this->saturation.updateInterval = interval;
}


void Nodule::setValue(uint8_t value) {
  this->value.value = value;
}


void Nodule::setValueMode(Mode mode) {
  this->value.mode = mode;
}


void Nodule::setValueUpdateInterval(uint16_t interval) {
  this->value.updateInterval = interval;
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
  if (elapsed < channel.updateInterval) {
    return;
  }

  channel.previousUpdate = now;

  // update the channel
  switch(channel.mode) {
  case ModePerlin:
    Generators::perlin(channel);
    break;
  case ModeReverseSawtooth:
    Generators::reverseSawtooth(channel);
    break;
  case ModeSawtooth:
    Generators::sawtooth(channel);
    break;
  case ModeConstant:
  default:
    // noop
    break;
  }
}


void Nodule::updateLeds() {
  for (uint8_t j = 0; j < NUM_LEDS_PER_NODE; j++) {
    this->leds[j] = CHSV(this->hue.value, this->saturation.value, this->value.value);
  }
}
