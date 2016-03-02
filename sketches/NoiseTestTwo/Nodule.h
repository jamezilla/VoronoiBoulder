#pragma once

#include <FastLED.h>
#include "config.h"

#define HUE_MODE            ModeSawtooth
#define HUE_INTERVAL        1U

#define SATURATION_MODE     ModeConstant
#define SATURATION_INTERVAL 10U

#define VALUE_MODE          ModeSawtooth
#define VALUE_INTERVAL      10U

class Nodule {

public:
  typedef enum Mode {
    ModeConstant,
    ModePerlin,
    ModeReverseSawtooth,
    ModeSawtooth,
    ModeSinusoidal
  } Mode;

  struct Channel {
    uint8_t  lutIndex;          // for lookup table generators, our current index into the table
    Mode     mode;              // which generator we're using for mutating 'value'
    uint16_t noiseSeed;         // where do we start in the noise field?
    uint32_t previousUpdate;    // the time of our last update
    uint16_t updateInterval;    // how often to update this value - in milliseconds
    uint8_t  value;             // the actual value of the channel
  };

  Nodule() {};
  Nodule(CRGB *leds) : leds(leds) {};

  void setHue(uint8_t hue);
  void setHueMode(Mode mode);
  void setHueUpdateInterval(uint16_t interval);

  void setSaturation(uint8_t saturation);
  void setSaturationMode(Mode mode);
  void setSaturationUpdateInterval(uint16_t interval);

  void setValue(uint8_t value);
  void setValueMode(Mode mode);
  void setValueUpdateInterval(uint16_t interval);

  void update(uint32_t now);
  void updateLeds();

private:
  void updateChannel(Channel &channel);

  // our slice of the LED array
  CRGB     *leds      = nullptr;
  uint32_t now       = 0;

  // the channel data structures
  Channel  hue        = { 0, HUE_MODE,        random16(), 0, HUE_INTERVAL,        random8() };
  Channel  saturation = { 0, SATURATION_MODE, random16(), 0, SATURATION_INTERVAL, UINT8_MAX };
  Channel  value      = { 0, VALUE_MODE,      random16(), 0, SATURATION_INTERVAL, UINT8_MAX };

};
