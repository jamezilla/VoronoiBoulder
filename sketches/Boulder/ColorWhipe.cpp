#include "ColorWhipe.h"

uint16_t ColorWhipe::getCurrentPixelIndex() {
  return this->currentPixelIndex;
}


void ColorWhipe::resetPixelIndex() {
  this->currentPixelIndex = 0;
}


void ColorWhipe::setColor(CHSV color) {
  this->color = color;
}


void ColorWhipe::setCurrentPixelIndex(uint16_t index) {
  this->currentPixelIndex = index;
}


void ColorWhipe::setInterval(uint16_t interval) {
  this->interval = interval;
}


void ColorWhipe::update() {
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
