#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

class ColorWhipe {

public:
  ColorWhipe(CRGB *leds) : leds(leds) {};

  uint16_t  getCurrentPixelIndex();
  void      resetPixelIndex();
  void      setColor(CRGB color);
  void      setColor(CHSV color);
  void      setCurrentPixelIndex(uint16_t index);
  void      setInterval(uint16_t interval);
  void      update();

private:
  CRGB      color             = CRGB(0,0,0);
  uint16_t  currentPixelIndex = 0;
  uint32_t  interval          = 0;
  CRGB      *leds             = nullptr;
  uint32_t  previousUpdate    = 0;       // the time of our last update

};
