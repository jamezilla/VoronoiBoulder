#pragma once

#include <FastLED.h>
#include "config.h"

// defaults

#define HUE_VALUE_MODE           ModeAlternateDrift
#define HUE_INTERVAL             500U
#define HUE_INTERVAL_MODE        ModeSinusoidal

#define SATURATION_VALUE_MODE    ModeConstant
#define SATURATION_INTERVAL      10U
#define SATURATION_INTERVAL_MODE ModeConstant

#define VALUE_VALUE_MODE         ModeConstant
#define VALUE_INTERVAL           10U
#define VALUE_INTERVAL_MODE      ModeConstant


class Cell {

public:

  // available generators

  typedef enum Mode {
    ModeAlternate,
    ModeAlternateDrift,
    ModeConstant,
    ModePerlin,
    ModeReverseSawtooth,
    ModeSawtooth,
    ModeSinusoidal
  } Mode;


  // main data structure for a channel (ie. hue, saturation, or value)

  struct Channel {
    uint16_t interval;          // how often to update this value - in milliseconds
    Mode     intervalMode;      // drive the update interval using a generator
    uint8_t  intervalLutIndex;  // for lookup table generators, our current index into the table
    uint32_t intervalNoiseSeed; // where do we start in the noise field?

    uint8_t  value;             // the actual value of the channel
    uint8_t  valueLutIndex;     // for lookup table generators, our current index into the table
    Mode     valueMode;         // which generator we're using for mutating 'value'
    uint16_t valueNoiseSeed;    // where do we start in the noise field?

    uint32_t previousUpdate;    // the time of our last update
  };


  // constructors

  Cell() {};
  Cell(CRGB *leds) : leds(leds) {};


  // instance methods

  void setHue(uint8_t hue);
  void setHueInterval(uint16_t interval);
  void setHueIntervalMode(Mode mode);
  void setHueMode(Mode mode);

  void setSaturation(uint8_t saturation);
  void setSaturationInterval(uint16_t interval);
  void setSaturationIntervalMode(Mode mode);
  void setSaturationMode(Mode mode);

  void setValue(uint8_t value);
  void setValueInterval(uint16_t interval);
  void setValueIntervalMode(Mode mode);
  void setValueMode(Mode mode);

  void update(uint32_t now);
  void updateLeds();

private:
  void updateChannel(Channel &channel);

  // our slice of the LED array
  CRGB *leds = nullptr;

  // storage for the current time of the loop (ie. millis())
  uint32_t now = 0;

  // the channel data structures
  Channel  hue        = { HUE_INTERVAL,        HUE_INTERVAL_MODE,        0, (uint32_t)random16(), random8(), 0, HUE_VALUE_MODE,        random16(), 0 };
  Channel  saturation = { SATURATION_INTERVAL, SATURATION_INTERVAL_MODE, 0, (uint32_t)random16(), UINT8_MAX, 0, SATURATION_VALUE_MODE, random16(), 0 };
  Channel  value      = { VALUE_INTERVAL,      VALUE_INTERVAL_MODE,      0, (uint32_t)random16(), UINT8_MAX, 0, VALUE_VALUE_MODE,      random16(), 0 };

};
