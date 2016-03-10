#include "ColorWipe.h"

uint16_t ColorWipe::getCurrentPixelIndex() {
  return this->currentPixelIndex;
}


void ColorWipe::resetPixelIndex() {
  this->currentPixelIndex = 0;
}


void ColorWipe::setColor(CHSV color) {
  this->color = color;
}


void ColorWipe::setCurrentPixelIndex(uint16_t index) {
  this->currentPixelIndex = index;
}


void ColorWipe::setInterval(uint16_t interval) {
  this->interval = interval;
}


void ColorWipe::update() {
  uint32_t now = millis();

  if ((now - previousUpdate) < interval) {
    // not ready to update
    return;
  }

  this->leds[currentPixelIndex] = this->color;

  previousUpdate = now;
  currentPixelIndex++;
  currentPixelIndex %= NUM_LEDS;
}
